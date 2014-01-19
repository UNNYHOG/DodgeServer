#ifndef _FACEBOOK_GRAPH_H_
#define _FACEBOOK_GRAPH_H_

#include <string>
#include <list>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/property_tree/ptree.hpp>

namespace unhg 
{
namespace unnynet
{

class FacebookGraph
{
private:

	boost::asio::io_service&  m_service;
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> m_socket;

public:
		
	FacebookGraph (boost::asio::io_service& service, boost::asio::ssl::context ssl_context);
	boost::property_tree::ptree GetRequest (std::string user, std::string token, std::list<std::string> fields);

private:

	void SendRequest (std::string& user, std::string& token, std::list<std::string>& fields);
	boost::property_tree::ptree FacebookGraph::ParseResponse ();
};

}
}

#endif