#ifndef _TCP_ASYNC_CLIENT_BASE_
#define _TCP_ASYNC_CLIENT_BASE_

#include "types.h"

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/noncopyable.hpp>

namespace unhg
{
namespace tcp
{

class TcpAsyncClientBase : private boost::noncopyable
{
protected:

	boost::asio::io_service   m_service;
	boost::asio::ssl::context m_ssl_context;

public:

	TcpAsyncClientBase  ();
	~TcpAsyncClientBase ();

protected:

	virtual void ErrorHandle (const std::string& error);

};

}
}

#endif

