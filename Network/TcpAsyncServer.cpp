#include "TcpAsyncServer.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/function.hpp>

using namespace boost::asio::ip;

namespace unhg
{
namespace tcp
{

// -----------------------------------------------------------------------------------------
TcpAsyncServer::TcpAsyncServer () :
// -----------------------------------------------------------------------------------------
	m_acceptor (m_service), m_new_session (), m_running (false)
{
	// setup like https
	m_ssl_context.set_default_verify_paths();
}

// --------------------------------------------------------------------------------------------
TcpAsyncServer::~TcpAsyncServer ()
// --------------------------------------------------------------------------------------------
{
	Stop ();
}

// -------------------------------------------------------------------------------------------
void TcpAsyncServer::Start (const char* address, ushort port)
// -------------------------------------------------------------------------------------------
{
	boost::asio::ip::tcp::resolver resolver (m_service);
	boost::asio::ip::tcp::resolver::query query (address, boost::lexical_cast<std::string> (port));
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);

	m_acceptor.close ();
	m_sessions.clear ();

	m_acceptor.open (endpoint.protocol());
	m_acceptor.set_option (boost::asio::ip::tcp::acceptor::reuse_address (true));
	m_acceptor.bind (endpoint);
	m_acceptor.listen ();

	StartAccept ();
}

// -------------------------------------------------------------------------------------------
void TcpAsyncServer::Stop ()
// -------------------------------------------------------------------------------------------
{
	m_running = false;
}


///                                                  PRIVATE


// --------------------------------------------------------------------------------------------
void TcpAsyncServer::StartAccept ()
// --------------------------------------------------------------------------------------------
{
	m_running = true;

	m_new_session.reset (GetNewSession());
	m_new_session -> SetErrorHandle (boost::bind (&TcpAsyncServer::ErrorHandle, this, _1));

	m_acceptor.async_accept (m_new_session -> GetSocket().lowest_layer(), 
		boost::bind (&TcpAsyncServer::AcceptHandle, this, boost::asio::placeholders::error));
}

// ----------------------------------------------------------------------------------------------
TcpSession* TcpAsyncServer::GetNewSession ()
// ----------------------------------------------------------------------------------------------
{
	return new TcpSession (m_service, m_ssl_context);
}

// ------------------------------------------------------------------------------------------
void TcpAsyncServer::AcceptHandle (const boost::system::error_code& error)
// ------------------------------------------------------------------------------------------
{
	if (!m_acceptor.is_open())
		return;

	if (error)
		ErrorHandle (error.message());
	else
	{
		m_new_session -> StartRecieve ();
		boost::lock_guard<boost::mutex> lock (m_session_mtx);
		m_sessions.insert (m_new_session);
	}

	StartAccept ();
}

}
}