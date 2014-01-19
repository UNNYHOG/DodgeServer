#ifndef _TCP_MESSAGE_H_
#define _TCP_MESSAGE_H_

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

namespace unhg
{
namespace tcp
{

class TcpMessage
{
public:

	friend class TcpSession;

	enum { HEADER_SIZE   = 4   }; // header: 4 body len
	enum { MAX_BODY_SIZE = 508 };

protected:

	size_t  m_body_size;
	char    m_data [HEADER_SIZE + MAX_BODY_SIZE];

public:

	TcpMessage  ();
	~TcpMessage ();

	size_t      GetDataSize () const { return m_body_size;          };
	const char* GetData     () const { return m_data + HEADER_SIZE; };

	void SetData (const char* data, size_t size);

protected:

	virtual void Encode ();
	virtual void Decode ();

};

typedef boost::shared_ptr<TcpMessage> TcpMessagePtr;

}
}

#endif

