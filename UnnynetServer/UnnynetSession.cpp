#include "UnnynetSession.h"
#include "UnnynetMessage.h"

#include "../MySqlServer/DataBaseServer.h"

namespace unhg 
{
namespace unnynet
{

// -----------------------------------------------------------------------------------------------
UnnynetSession::UnnynetSession (size_t id, boost::asio::io_service& service, boost::asio::ssl::context& ssl_context, db::DatabaseServer* database) :
// -----------------------------------------------------------------------------------------------
	 unhg::tcp::TcpSession (service, ssl_context), m_id (id), m_database (database)
{
	m_last_time = m_start_time = boost::chrono::system_clock::now ();
}

// -----------------------------------------------------------------------------------------------
UnnynetSession::~UnnynetSession ()
// -----------------------------------------------------------------------------------------------
{
}

// --------------------------------------------------------------------------------------------
void UnnynetSession::RecieveHandle (unhg::tcp::TcpMessagePtr msg)
// --------------------------------------------------------------------------------------------
{
	m_last_time = boost::chrono::system_clock::now ();

	UnnynetMessagePtr umsg = boost::static_pointer_cast<UnnynetMessage>(msg);
	switch (umsg -> GetMsgType())
	{
		case UnnynetMessage::Register:
			break;
		case UnnynetMessage::Login:
			break;
		case UnnynetMessage::LoginResponse:
			break;
		case UnnynetMessage::Simple:
		{
			SimpleMessagePtr sumsg = boost::static_pointer_cast<SimpleMessage>(umsg);
			switch (sumsg -> GetID())
			{
			case SimpleMessage::MSG_OK:
				break;
			case SimpleMessage::MSG_ERR:
				break;
			}
		}
		break;
	}
}

}
}