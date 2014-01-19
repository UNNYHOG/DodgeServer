#ifndef _TCP_ASYNC_SERVER_H_
#define _TCP_ASYNC_SERVER_H_

#include "TcpAsyncClientBase.h"
#include "TcpSession.h"

#include <set>
#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>

namespace unhg
{
namespace tcp
{

class TcpAsyncServer :  public TcpAsyncClientBase
{
private:

	boost::asio::ip::tcp::acceptor m_acceptor;
	TcpSessionPtr m_new_session;

protected:

	bool m_running;
	boost::mutex m_session_mtx;
	std::set<TcpSessionPtr> m_sessions;

	typedef std::set<TcpSessionPtr>::iterator session_iter;
	typedef std::set<TcpSessionPtr>::const_iterator session_citer;

public:

	TcpAsyncServer ();
	~TcpAsyncServer ();

	virtual void Start (const char* address, ushort port);
	virtual void Stop ();

protected:

	virtual TcpSession* GetNewSession ();

private:

	void StartAccept ();
	void AcceptHandle  (const boost::system::error_code& error);
};

}
}

#endif

