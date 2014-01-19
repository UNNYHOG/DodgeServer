#include "../MySqlServer/DatabaseServer.h"
#include "UnnynetServer.h"
#include "UnnynetSession.h"
#include "Version.h"
#include "Utilities.h"

#include <fstream>
#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

using namespace boost::asio::ip;

namespace unhg 
{
namespace unnynet
{

#define ERROR_FILE_NAME  "error.log"
#define CONFIG_FILE_NAME "config.ini"

// ------------------------------------------------------------------------------------------------
UnnynetServer::UnnynetServer () : m_session_indexer (0), m_cmd_parser (this)
// ------------------------------------------------------------------------------------------------
{
	ReadConfig ();
}

// ------------------------------------------------------------------------------------------------
UnnynetServer::~UnnynetServer ()
// ------------------------------------------------------------------------------------------------
{
}

// ------------------------------------------------------------------------------------------------
void UnnynetServer::Start (const char* address, ushort port)
// ------------------------------------------------------------------------------------------------
{
	try
	{
		m_sessions_thread.interrupt ();
		m_sessions_thread.join ();

		m_start_time = boost::chrono::system_clock::now ();
		m_sessions_thread = boost::thread (boost::bind (&UnnynetServer::SessionManagerProc, this));

		m_database.reset (new db::DatabaseServer(m_db_host, m_db_user, m_db_pass, m_db_name));

		m_task_scheduller.reset (new TaskScheduller(m_service, boost::bind(&UnnynetServer::ErrorHandle, this, _1)));

		if ((address == 0) || (port == 0))
			TcpAsyncServer::Start (m_address.c_str(), m_port);
		else
			TcpAsyncServer::Start (address, port);
	}
	catch (std::exception& e)
	{
		std::string err = e.what ();
		ErrorHandle (err);
		Stop ();
	}
}

// -------------------------------------------------------------------------------------------
void UnnynetServer::Stop ()
// -------------------------------------------------------------------------------------------
{
	m_service.stop();
	m_sessions_thread.interrupt ();
	m_sessions_thread.join ();
	m_task_scheduller.reset ();
	TcpAsyncServer::Stop ();
}

// ------------------------------------------------------------------------------------------------
void UnnynetServer::Command ()
// ------------------------------------------------------------------------------------------------
{
	std::string str;
	std::cout << Status () << std::endl;
	while (IsRunning())
	{
		str.clear ();
		std::cout << "Dodge Server>";
		getline (std::cin, str);
		std::cout << ParseCommand (str) << std::endl;
	}
}

// ------------------------------------------------------------------------------------------------
std::string UnnynetServer::Status ()
// ------------------------------------------------------------------------------------------------
{
	std::stringstream ss;
	ss << "Dodge Server version " << DODGE_SERVER_VERSION;
	if (m_running)
	{
		size_t sc = 0;
		{
			boost::lock_guard<boost::mutex> lock (m_session_mtx);
			sc = m_sessions.size ();
		}
		ss << " running\n\tuptime: ";
		boost::chrono::duration_format_daytime(ss, boost::chrono::system_clock::now() - m_start_time);
		ss << "\n\tsessions: " << sc << "\n";
	}
	else ss << " stopped.";
	return ss.str(); 
}


///                                                PRIVATE


// ------------------------------------------------------------------------------------------------
void UnnynetServer::ReadConfig ()
// ------------------------------------------------------------------------------------------------
{
	try
	{
		boost::property_tree::ptree pt;
		boost::property_tree::ini_parser::read_ini (CONFIG_FILE_NAME, pt);
		
		m_port = pt.get<int> ("ssl.port");
		m_session_max_time = pt.get<uint> ("session.max_time");

		m_db_host = pt.get<std::string> ("db.host"); 
		m_db_user = pt.get<std::string> ("db.user");
		m_db_pass = pt.get<std::string> ("db.pass");
		m_db_name = pt.get<std::string> ("db.name");
	}
	catch (std::exception& e)
	{
		std::string err = e.what();
		ErrorHandle (err);
	}
}

// ------------------------------------------------------------------------------------------------
unhg::tcp::TcpSession* UnnynetServer::GetNewSession ()
// ------------------------------------------------------------------------------------------------
{
	return new UnnynetSession (m_session_indexer++, m_service, m_ssl_context, m_database.get());
}

// ----------------------------------------------------------------------------------------------
void UnnynetServer::SessionManagerProc ()
// ----------------------------------------------------------------------------------------------
{
	while (true)
	{
		boost::this_thread::sleep_for (boost::chrono::seconds (m_session_max_time));
		{
			std::list<UnnynetSessionPtr> to_delete;
			boost::lock_guard<boost::mutex> lock (m_session_mtx);
			boost::chrono::system_clock::time_point now = boost::chrono::system_clock::now ();
			
			for (session_citer i = m_sessions.begin (); i != m_sessions.end (); ++i)
			{
				UnnynetSessionPtr p = boost::static_pointer_cast<UnnynetSession> (*i);
				if ((now - p -> GetLastTime ()) >= boost::chrono::seconds (m_session_max_time))
					to_delete.push_back (p);
			}

			for (std::list<UnnynetSessionPtr>::iterator i = to_delete.begin (); i != to_delete.end (); ++i)
				m_sessions.erase (*i);
		}
	}
}

// ----------------------------------------------------------------------------------------------
std::string UnnynetServer::ParseCommand (const std::string& command)
// ----------------------------------------------------------------------------------------------
{
	return m_cmd_parser.Parse (command);
}

// ----------------------------------------------------------------------------------------------
std::string UnnynetServer::ShowSessions ()
// ----------------------------------------------------------------------------------------------
{
	std::stringstream ss;
	boost::lock_guard<boost::mutex> lock (m_session_mtx);
	for (session_citer i = m_sessions.begin (); i != m_sessions.end (); ++i)
	{
		UnnynetSessionPtr p = boost::static_pointer_cast<UnnynetSession>(*i);
		ss << "id: " << p -> GetID ();
		ss << " address: " << p -> GetSocket().lowest_layer().remote_endpoint().address().to_string() << ":"
			<< p -> GetSocket().lowest_layer().remote_endpoint().port ();
		ss << " start: " << boost::chrono::time_fmt (boost::chrono::timezone::local, "%H:%M:%S") << p -> GetStartTime ();
		ss << " last access: " << boost::chrono::time_fmt (boost::chrono::timezone::local, "%H:%M:%S") << p -> GetLastTime () << "\n";
	}
	return ss.str();
}

// ----------------------------------------------------------------------------------------------
bool UnnynetServer::KillSession (size_t id)
// ----------------------------------------------------------------------------------------------
{
	boost::lock_guard<boost::mutex> lock (m_session_mtx);
	for (session_citer i = m_sessions.begin (); i != m_sessions.end (); ++i)
	{
		UnnynetSessionPtr p = boost::static_pointer_cast<UnnynetSession>(*i);
		if (p -> GetID() == id)
		{
			m_sessions.erase (*i);
			return true;
		}
	}
	return false;
}

// ------------------------------------------------------------------------------------------------
void UnnynetServer::ErrorHandle (const std::string& error)
// ------------------------------------------------------------------------------------------------
{
	std::ofstream m_err_file;
	boost::lock_guard<boost::mutex> lock (m_err_mtx);

	try
	{
		m_err_file = std::ofstream (ERROR_FILE_NAME, std::ios_base::app);
		m_err_file << boost::chrono::time_fmt (boost::chrono::timezone::local, "%d.%m.%y %H:%M:%S") << "[" << boost::chrono::system_clock::now() << "] " << error << std::endl;
	}
	catch (std::exception& e)
	{
		std::cerr << error << std::endl;
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown error during writing error" << std::endl;
	}
		
	m_err_file.close ();
}

}
}

