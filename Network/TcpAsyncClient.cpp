#include "TcpAsyncClient.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::asio::ip;

namespace unhg
{
namespace tcp
{

// -----------------------------------------------------------------------------------------------
TcpAsyncClient::TcpAsyncClient (const char* local_cert_name) :
// -----------------------------------------------------------------------------------------------
	TcpAsyncClientBase () 		
{
	m_ssl_context.load_verify_file (local_cert_name);

	m_session = TcpSessionPtr (GetNewSession ());
	m_session -> GetSocket().set_verify_mode (boost::asio::ssl::verify_peer);	
	m_session -> GetSocket().set_verify_callback (boost::bind (&TcpAsyncClient::VerifyCertificate, this, _1, _2));
	m_session -> SetErrorHandle (boost::bind (&TcpAsyncClient::ErrorHandle, this, _1));
}

// -----------------------------------------------------------------------------------------------
void TcpAsyncClient::Connect (const char* server_name, ushort server_port)
// -----------------------------------------------------------------------------------------------
{
	Stop ();
	m_thread = boost::thread (boost::bind(&boost::asio::io_service::run, &m_service));

	boost::asio::ip::tcp::resolver resolver (m_service);
	boost::asio::ip::tcp::resolver::query query (server_name, boost::lexical_cast<std::string> (server_port));

	boost::asio::async_connect (m_session -> GetSocket().lowest_layer(), resolver.resolve (query), 
		 boost::bind (&TcpAsyncClient::ConnectHandle, this, boost::asio::placeholders::error));
}

// -----------------------------------------------------------------------------------------------
TcpAsyncClient::~TcpAsyncClient ()
// -----------------------------------------------------------------------------------------------
{
	m_service.stop ();
}

// -----------------------------------------------------------------------------------------------
void TcpAsyncClient::Stop ()
// -----------------------------------------------------------------------------------------------
{
	m_session -> GetSocket().lowest_layer().close();
	m_service.stop ();
	m_thread.join ();
}


///                                                  PRIVATE


// ----------------------------------------------------------------------------------------------
TcpSession* TcpAsyncClient::GetNewSession ()
// ----------------------------------------------------------------------------------------------
{
	return new TcpSession (m_service, m_ssl_context);
}

// ----------------------------------------------------------------------------------------------
bool TcpAsyncClient::VerifyCertificate (bool preverified, boost::asio::ssl::verify_context& ctx)
// ----------------------------------------------------------------------------------------------
{
	return true;
}

// ----------------------------------------------------------------------------------------------
void TcpAsyncClient::ConnectHandle (const boost::system::error_code& error)
// ----------------------------------------------------------------------------------------------
{
	if (!error)
	{
		m_session -> GetSocket().async_handshake (boost::asio::ssl::stream_base::client,
			boost::bind (&TcpAsyncClient::HandshakeHandle, this,
			boost::asio::placeholders::error));
	}
	else
		ErrorHandle (error.message());
}

// --------------------------------------------------------------------------------------------
void TcpAsyncClient::HandshakeHandle (const boost::system::error_code& error)
// --------------------------------------------------------------------------------------------
{
	if (!error)
		m_session -> StartRecieve ();
	else
		ErrorHandle (error.message());
}

}
}