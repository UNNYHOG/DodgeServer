#include "TcpMessage.h"

#include <boost/bind.hpp>

namespace unhg
{
namespace tcp
{

// ----------------------------------------------------------------------------------------------
TcpMessage::TcpMessage ()
// ----------------------------------------------------------------------------------------------
{
	memset (m_data, 0, HEADER_SIZE + MAX_BODY_SIZE);
}

// ----------------------------------------------------------------------------------------------
TcpMessage::~TcpMessage ()
// ----------------------------------------------------------------------------------------------
{
}

// --------------------------------------------------------------------------------------------
void TcpMessage::SetData (const char* data, size_t size)
// --------------------------------------------------------------------------------------------
{
	if (size > MAX_BODY_SIZE)
		throw std::exception ("Message size is too large");

	memset (m_data, 0, HEADER_SIZE + MAX_BODY_SIZE);
	memcpy (m_data + HEADER_SIZE, data, size);
	m_body_size = size;
}

// ----------------------------------------------------------------------------------------------
void TcpMessage::Encode ()
// ----------------------------------------------------------------------------------------------
{
	char header[TcpMessage::HEADER_SIZE + 1] = "";
	std::sprintf (header, "%4u", GetDataSize ());
    memcpy (m_data, header, TcpMessage::HEADER_SIZE);
}

// ----------------------------------------------------------------------------------------------
void TcpMessage::Decode ()
// ----------------------------------------------------------------------------------------------
{
	char header[TcpMessage::HEADER_SIZE + 1] = "";

	strncat (header, m_data, TcpMessage::HEADER_SIZE);
	m_body_size = atoi (header);

	if (m_body_size > TcpMessage::MAX_BODY_SIZE)
		throw std::exception ("Message size is too large");
}

}
}