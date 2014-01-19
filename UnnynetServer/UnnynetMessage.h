#ifndef _UNNYNET_MESSAGE_H_
#define _UNNYNET_MESSAGE_H_

#include <string>

#include "../Network/types.h"
#include "../Network/TcpMessage.h"

namespace unhg 
{
namespace unnynet
{

class UnnynetMessage : public unhg::tcp::TcpMessage
{
public:

	enum MsgType
	{
		None,
		Register,
		Login,
		LoginResponse,
		Simple
	};

	enum { CRC_SIZE  = 4 };
	enum { TYPE_SIZE = 2 };
	enum { UNNYMSG_SIZE = 6 }; // CRC_SIZE + TYPE_SIZE

public:

	MsgType GetMsgType() const
	{ 
		//char header_crc[UnnynetMessage::CRC_SIZE + 1] = "";
		char header_type[UnnynetMessage::TYPE_SIZE + 1] = "";
		//strncat (header_crc , m_data + TcpMessage::HEADER_SIZE, UnnynetMessage::CRC_SIZE);
		strncat (header_type, m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::CRC_SIZE, UnnynetMessage::TYPE_SIZE);
		return (MsgType)atoi(header_type);
	};

protected:

	void EncodeType (MsgType type)
	{
		char header[UnnynetMessage::UNNYMSG_SIZE + 1] = "";
		std::sprintf (header, "%4u", 0);
		std::sprintf (header + UnnynetMessage::CRC_SIZE, "%2u", (size_t)type);
		memcpy (m_data + TcpMessage::HEADER_SIZE, header, UnnynetMessage::UNNYMSG_SIZE);
	}
};

class RegisterRequest : public UnnynetMessage
{
public:

	std::string GetUserID () const
	{ 
		return m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE; 
	};

	void SetData (const std::string& token) 
	{ 
		strcpy (m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, token.c_str());
	};

protected:

	virtual void Encode ()
	{
		UnnynetMessage::Encode();
		UnnynetMessage::EncodeType(Register);
	}

};

class LoginRequest : public UnnynetMessage
{
public: 

	enum { USERID_SIZE = 10 };

public:

	ulong GetUserID () const 
	{
		char header[LoginRequest::USERID_SIZE + 1] = "";
		strncat (header , m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, LoginRequest::USERID_SIZE);
		return (ulong)atol (header);
	};

	void SetData (ulong user_id) 
	{
		char header[LoginRequest::USERID_SIZE + 1] = "";
		std::sprintf (header, "%10u", user_id);
		memcpy (m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, header, LoginRequest::USERID_SIZE);
	};

protected:

	virtual void Encode ()
	{
		UnnynetMessage::Encode();
		UnnynetMessage::EncodeType(Login);
	}

};

class LoginResponse : public UnnynetMessage
{
public: 

	enum { USERID_SIZE = 10 };

public:

	ulong GetUserID () const 
	{
		char header[LoginRequest::USERID_SIZE + 1] = "";
		strncat (header , m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, LoginRequest::USERID_SIZE);
		return (ulong)atol (header);
	};

	void SetData (ulong user_id) 
	{
		char header[LoginRequest::USERID_SIZE + 1] = "";
		std::sprintf (header, "%10u", user_id);
		memcpy (m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, header, LoginRequest::USERID_SIZE);
	};

protected:

	virtual void Encode ()
	{
		UnnynetMessage::Encode();
		UnnynetMessage::EncodeType(UnnynetMessage::LoginResponse);
	}

};

class SimpleMessage : public UnnynetMessage
{
public:

	enum MsgID
	{
		None,
		MSG_OK,
		MSG_ERR
	};

	enum { MSGID_SIZE = 2 };

public:

	MsgID GetID () const 
	{ 
		char header[SimpleMessage::MSGID_SIZE + 1] = "";
		strncat (header , m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, SimpleMessage::MSGID_SIZE);
		return (MsgID)atol (header);
	}

	void SetData (MsgID id) 
	{ 
		char header[LoginRequest::USERID_SIZE + 1] = "";
		std::sprintf (header, "%2u", (size_t)id);
		memcpy (m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, header, SimpleMessage::MSGID_SIZE);
	};	

protected:

	virtual void Encode ()
	{
		UnnynetMessage::Encode();
		UnnynetMessage::EncodeType(UnnynetMessage::Simple);
	}

};

typedef boost::shared_ptr<UnnynetMessage>  UnnynetMessagePtr;
typedef boost::shared_ptr<RegisterRequest> RegisterRequestPtr;
typedef boost::shared_ptr<LoginRequest>    LoginRequestPtr;
typedef boost::shared_ptr<LoginResponse>   LoginResponsePtr;
typedef boost::shared_ptr<SimpleMessage>   SimpleMessagePtr;


///										IMPLEMENTATION

/*
// ----------------------------------------------------------------------------------------------
void UnnynetMessage::Encode ()
// ----------------------------------------------------------------------------------------------
{
	TcpMessage::Encode();

	char header[UnnynetMessage::UNNYMSG_SIZE + 1] = "";
	std::sprintf (header, "%4u", m_crc);
	std::sprintf (header + UnnynetMessage::CRC_SIZE, "%2u", (size_t)m_type);
    memcpy (m_data + TcpMessage::HEADER_SIZE, header, UnnynetMessage::UNNYMSG_SIZE);
}

// ----------------------------------------------------------------------------------------------
void UnnynetMessage::Decode ()
// ----------------------------------------------------------------------------------------------
{
	TcpMessage::Decode();

	char header_crc[UnnynetMessage::CRC_SIZE + 1] = "";
	char header_type[UnnynetMessage::TYPE_SIZE + 1] = "";

	strncat (header_crc , m_data + TcpMessage::HEADER_SIZE, UnnynetMessage::CRC_SIZE);
	strncat (header_type, m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::CRC_SIZE, UnnynetMessage::TYPE_SIZE);

	m_crc  = atoi (header_crc);
	m_type = (MsgType)atoi(header_type);
}

// ----------------------------------------------------------------------------------------------
void RegisterRequest::Encode ()
// ----------------------------------------------------------------------------------------------
{
	UnnynetMessage::Encode();
	strcpy (m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, m_token.c_str());
}

// ----------------------------------------------------------------------------------------------
void RegisterRequest::Decode ()
// ----------------------------------------------------------------------------------------------
{
	UnnynetMessage::Decode();
	m_token = m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE;
}

// ----------------------------------------------------------------------------------------------
void LoginRequest::Encode ()
// ----------------------------------------------------------------------------------------------
{
	UnnynetMessage::Encode();

	char header[LoginRequest::USERID_SIZE + 1] = "";
	std::sprintf (header, "%10u", m_userID);
    memcpy (m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, header, LoginRequest::USERID_SIZE);
}

// ----------------------------------------------------------------------------------------------
void LoginRequest::Decode ()
// ----------------------------------------------------------------------------------------------
{
	UnnynetMessage::Decode();

	char header[LoginRequest::USERID_SIZE + 1] = "";
	strncat (header , m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, LoginRequest::USERID_SIZE);

	m_userID = (ulong)atol (header);
}

// ----------------------------------------------------------------------------------------------
void LoginResponse::Encode ()
// ----------------------------------------------------------------------------------------------
{
	UnnynetMessage::Encode();
	
	char header[LoginRequest::USERID_SIZE + 1] = "";
	std::sprintf (header, "%10u", m_userID);
    memcpy (m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, header, LoginRequest::USERID_SIZE);
}

// ----------------------------------------------------------------------------------------------
void LoginResponse::Decode ()
// ----------------------------------------------------------------------------------------------
{
	UnnynetMessage::Decode();

	char header[LoginRequest::USERID_SIZE + 1] = "";
	strncat (header , m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, LoginRequest::USERID_SIZE);

	m_userID = (ulong)atol (header);
}

// ----------------------------------------------------------------------------------------------
void SimpleMessage::Encode ()
// ----------------------------------------------------------------------------------------------
{
	UnnynetMessage::Encode();
	
	char header[LoginRequest::USERID_SIZE + 1] = "";
	std::sprintf (header, "%2u", (size_t)m_id);
	memcpy (m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, header, SimpleMessage::MSGID_SIZE);
}

// ----------------------------------------------------------------------------------------------
void SimpleMessage::Decode ()
// ----------------------------------------------------------------------------------------------
{
	UnnynetMessage::Decode();

	char header[SimpleMessage::MSGID_SIZE + 1] = "";
	strncat (header , m_data + TcpMessage::HEADER_SIZE + UnnynetMessage::UNNYMSG_SIZE, SimpleMessage::MSGID_SIZE);

	m_id = (MsgID)atol (header);
}
*/

}
}

#endif