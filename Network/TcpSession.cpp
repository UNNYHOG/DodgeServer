#include "TcpSession.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::asio::ip;

namespace unhg
{
namespace tcp
{

// -----------------------------------------------------------------------------------------------
TcpSession::TcpSession (boost::asio::io_service& service, boost::asio::ssl::context& ssl_context) :
// -----------------------------------------------------------------------------------------------
	m_socket (service, ssl_context)
{
}

// -----------------------------------------------------------------------------------------------
TcpSession::~TcpSession ()
// -----------------------------------------------------------------------------------------------
{
	m_socket.lowest_layer().close ();
}

// -----------------------------------------------------------------------------------------------
void TcpSession::StartRecieve ()
// -----------------------------------------------------------------------------------------------
{
	boost::asio::async_read (	m_socket,
								boost::asio::buffer (m_rec_msg -> m_data, TcpMessage::HEADER_SIZE),
								boost::bind (&TcpSession::RecieveHeaderHandle, this,
									boost::asio::placeholders::error ));
}

// -----------------------------------------------------------------------------------------------
void TcpSession::StartSend (const TcpMessagePtr& msg)	
// -----------------------------------------------------------------------------------------------
{
	try
	{
		boost::lock_guard<boost::mutex> lock (m_msg_mtx);
		bool sending = !m_data_queue.empty ();
		m_data_queue.push (msg);

		if (!sending)
		{
			m_data_queue.front() -> Encode();
			boost::asio::async_write (	m_socket, 
										boost::asio::buffer (m_data_queue.front() -> GetData(), 
											TcpMessage::HEADER_SIZE + m_data_queue.front() -> GetDataSize()), 
										boost::bind (&TcpSession::SendHandle, this,
												boost::asio::placeholders::error, 
												boost::asio::placeholders::bytes_transferred ));
		}
	}
	catch (std::exception& exp)
	{
		std::string err = exp.what();
		ErrorHandle (err);
	}
}

// ---------------------------------------------------------------------------------------------
void TcpSession::SetErrorHandle (error_callback_t error_callback)
// ---------------------------------------------------------------------------------------------
{
	m_error_callback = error_callback;
}


///                                                  PRIVATE


// ----------------------------------------------------------------------------------------------
void TcpSession::SendHandle (const boost::system::error_code& error, std::size_t bytes_trfd)
// ----------------------------------------------------------------------------------------------
{
	if (!error)
	{
		boost::lock_guard<boost::mutex> lock (m_msg_mtx);
		SendHandle (m_data_queue.front());
		m_data_queue.pop ();

		if (!m_data_queue.empty())
		{
			m_data_queue.front() -> Encode();
			boost::asio::async_write (	m_socket, 
										boost::asio::buffer (m_data_queue.front() -> GetData(), 
											TcpMessage::HEADER_SIZE + m_data_queue.front() -> GetDataSize()), 
										boost::bind (&TcpSession::SendHandle, this,
												boost::asio::placeholders::error, 
												boost::asio::placeholders::bytes_transferred ));
		}
	}
	else
	{
		{
			boost::lock_guard<boost::mutex> lock (m_msg_mtx);
			m_data_queue.pop ();
		}
		ErrorHandle (error.message());
	}
}

// ----------------------------------------------------------------------------------------------
void TcpSession::RecieveHeaderHandle (const boost::system::error_code& error)
// ----------------------------------------------------------------------------------------------
{
	if (!error)
	{
		try 
		{ 
			m_rec_msg -> Decode ();
			boost::asio::async_read (	m_socket,
										boost::asio::buffer (m_rec_msg -> m_data + TcpMessage::HEADER_SIZE, 
											m_rec_msg -> GetDataSize()),
										boost::bind (&TcpSession::RecieveBodyHandle, this,
											boost::asio::placeholders::error ));
		}
		catch (std::exception exp)
		{
			std::string err = exp.what();
			ErrorHandle (err);
		}
	}
	else
		ErrorHandle (error.message());
}

// ----------------------------------------------------------------------------------------------
void TcpSession::RecieveBodyHandle (const boost::system::error_code& error)
// ----------------------------------------------------------------------------------------------
{
	if (!error)
	{
		RecieveHandle (m_rec_msg);
		
		memset (m_rec_msg -> m_data, 0, TcpMessage::HEADER_SIZE + TcpMessage::MAX_BODY_SIZE);
		boost::asio::async_read (	m_socket,
										boost::asio::buffer (m_rec_msg -> m_data, TcpMessage::HEADER_SIZE),
										boost::bind (&TcpSession::RecieveHeaderHandle, this,
											boost::asio::placeholders::error ));
	}
	else
		ErrorHandle (error.message());
}

// -------------------------------------------------------------------------------------------
void TcpSession::SendHandle (TcpMessagePtr msg)
// -------------------------------------------------------------------------------------------
{
}

// -------------------------------------------------------------------------------------------
void TcpSession::RecieveHandle (TcpMessagePtr msg)
// -------------------------------------------------------------------------------------------
{
}

// --------------------------------------------------------------------------------------------
void TcpSession::ErrorHandle (const std::string& error)
// --------------------------------------------------------------------------------------------
{
	if (m_error_callback)
		m_error_callback (error);
}

}
}