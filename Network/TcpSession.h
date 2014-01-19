#ifndef _SSL_SESSION_
#define _SSL_SESSION_

#include "types.h"
#include "TcpMessage.h"

#include <queue>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/thread.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace boost { namespace asio { namespace ip { namespace ssl {
	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket;
} } } }

namespace unhg
{
namespace tcp
{

class TcpSession : private boost::noncopyable
{
public:

	typedef boost::function<void (const std::string& error)> error_callback_t;

private:

	boost::asio::ip::ssl::socket m_socket;
	TcpMessagePtr m_rec_msg;
	std::queue<TcpMessagePtr> m_data_queue;
	boost::mutex m_msg_mtx;
	error_callback_t m_error_callback;

public:

	TcpSession  (boost::asio::io_service& service, boost::asio::ssl::context& ssl_context);
	~TcpSession ();

	void StartSend (const TcpMessagePtr& msg);
	void StartRecieve ();

	boost::asio::ip::ssl::socket& GetSocket () { return m_socket; }

	void SetErrorHandle (error_callback_t error_callback);

protected:

	virtual void ErrorHandle (const std::string& error);

	virtual void SendHandle    (TcpMessagePtr msg);
	virtual void RecieveHandle (TcpMessagePtr msg);

private:

	void SendHandle          (const boost::system::error_code& error, size_t bytes_trfd);
	void RecieveHeaderHandle (const boost::system::error_code& error);
	void RecieveBodyHandle   (const boost::system::error_code& error);

};

typedef boost::shared_ptr<TcpSession> TcpSessionPtr;

}
}

#endif

