#include "TcpAsyncClientBase.h"

#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::asio::ip;

namespace unhg
{
namespace tcp
{

// ---------------------------------------------------------------------------------------------
TcpAsyncClientBase::TcpAsyncClientBase () :  
		m_ssl_context     (boost::asio::ssl::context::sslv23)
// ---------------------------------------------------------------------------------------------
{
}

// ---------------------------------------------------------------------------------------------
TcpAsyncClientBase::~TcpAsyncClientBase ()
// ---------------------------------------------------------------------------------------------
{
}


///                                                  PRIVATE


// --------------------------------------------------------------------------------------------
void TcpAsyncClientBase::ErrorHandle (const std::string& error)
// --------------------------------------------------------------------------------------------
{
}


}
}
