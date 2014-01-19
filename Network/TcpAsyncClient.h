#ifndef _TCP_ASYNC_CLIENT_
#define _TCP_ASYNC_CLIENT_

#include "TcpAsyncClientBase.h"
#include "TcpSession.h"

#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

namespace unhg
{
namespace tcp
{

class TcpAsyncClient : public TcpAsyncClientBase
{
private:

	boost::thread m_thread;

protected:

	TcpSessionPtr m_session;

public:

	TcpAsyncClient (const char* local_cert_name);
	~TcpAsyncClient ();

	void Connect (const char* server_name, ushort server_port);
	void Stop ();

protected:

	virtual TcpSession* GetNewSession ();

	virtual bool VerifyCertificate (bool preverified, boost::asio::ssl::verify_context& ctx);
	virtual void ConnectHandle     (const boost::system::error_code& error);
	virtual void HandshakeHandle   (const boost::system::error_code& error);
};

}
}

#endif