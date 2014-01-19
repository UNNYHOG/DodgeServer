#ifndef _UNNYNET_SESSION_H_
#define _UNNYNET_SESSION_H_

#include <ctime>
#include <boost/asio.hpp>

#include "../Network/TcpSession.h"

namespace unhg 
{

namespace db
{
	class DatabaseServer;
}

namespace unnynet
{

class UnnynetSession : public unhg::tcp::TcpSession
{
private:

	size_t m_id;
	boost::chrono::system_clock::time_point  m_start_time;
	boost::chrono::system_clock::time_point  m_last_time;
	db::DatabaseServer* m_database;

public:

	UnnynetSession  (size_t id, boost::asio::io_service& service, boost::asio::ssl::context& ssl_context, db::DatabaseServer* database);
	~UnnynetSession ();
		
	size_t GetID () const { return m_id; };
	boost::chrono::system_clock::time_point GetStartTime () const { return m_start_time; };
	boost::chrono::system_clock::time_point GetLastTime  () const { return m_last_time;  };

protected:

	virtual void RecieveHandle (unhg::tcp::TcpMessagePtr msg);

};

typedef boost::shared_ptr<UnnynetSession> UnnynetSessionPtr;

}
}

#endif
