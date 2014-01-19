#ifndef _UNNYNET_SERVER_H_
#define _UNNYNET_SERVER_H_

#include "../Network/types.h"
#include "../Network/TcpAsyncServer.h"
#include "CommandParser.h"
#include "TaskScheduller.h"

#define BOOST_CHRONO_DONT_PROVIDES_DEPRECATED_IO_SINCE_V2_0_0

#include <map>
#include <string>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/regex.hpp>

namespace unhg 
{

namespace db
{
	class DatabaseServer;
	typedef boost::shared_ptr<DatabaseServer> DatabaseServerPtr;
}

namespace unnynet
{

class UnnynetServer : public unhg::tcp::TcpAsyncServer
{
private:
	
	friend class CommandParser;

	ushort					m_port;
	std::string				m_address;
	std::string             m_db_host;
	std::string             m_db_user;
	std::string             m_db_pass;
	std::string             m_db_name;

	boost::mutex			m_err_mtx;
	size_t					m_session_indexer;
	CommandParser			m_cmd_parser;
	TaskSchedullerPtr       m_task_scheduller;
	uint                    m_session_max_time;
	boost::thread			m_sessions_thread;
	boost::chrono::system_clock::time_point m_start_time;
	db::DatabaseServerPtr m_database;

public:

	UnnynetServer  ();
	~UnnynetServer ();

	virtual void Start (const char* address = null, ushort port = 0);
	virtual void Stop ();
	void Command ();
		
	bool IsRunning () const { return m_running; };

private:

	void ReadConfig ();
	void SessionManagerProc ();
	
	virtual unhg::tcp::TcpSession* GetNewSession ();
	virtual void ErrorHandle (const std::string& error);

	std::string Status ();
	std::string ParseCommand (const std::string& command);
	std::string ShowSessions ();
	bool KillSession (size_t id);

};

}
}

#endif

