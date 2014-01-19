#include "FacebookGraph.h"

#include <boost/property_tree/json_parser.hpp>
#include <boost/lexical_cast.hpp>

namespace unhg 
{
namespace unnynet
{

typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket;

bool VerifyCertificate (bool preverified, boost::asio::ssl::verify_context& ctx);
void ParseHTTPHeader (boost::asio::streambuf& response, std::istream& response_stream, ssl_socket& socket);

// ----------------------------------------------------------------------------------------------
FacebookGraph::FacebookGraph (boost::asio::io_service& service, boost::asio::ssl::context ssl_context) : 
// ----------------------------------------------------------------------------------------------
	m_service (service), m_socket (service, ssl_context)
{
}

// ----------------------------------------------------------------------------------------------
boost::property_tree::ptree FacebookGraph::GetRequest (std::string user, std::string token, std::list<std::string> fields)
// ----------------------------------------------------------------------------------------------
{
	m_socket.set_verify_mode (boost::asio::ssl::verify_peer);
	m_socket.set_verify_callback (boost::bind(&VerifyCertificate, _1, _2));

	boost::asio::ip::tcp::resolver resolver (m_service);
	boost::asio::ip::tcp::resolver::query query("graph.facebook.com", "https");

	boost::asio::connect (m_socket.lowest_layer(), resolver.resolve (query));
	m_socket.handshake (boost::asio::ssl::stream_base::client);
	SendRequest (user, token, fields);
	return ParseResponse ();
}


///                                                  PRIVATE


// ----------------------------------------------------------------------------------------------
void FacebookGraph::SendRequest (std::string& user, std::string& token, std::list<std::string>& fields)
// ----------------------------------------------------------------------------------------------
{
	boost::asio::streambuf request;
	std::ostream request_stream(&request);

	// GET /user_id?fields=field1, field2...@access_token=token\r\n
	request_stream << "GET /" << user;
	if (!fields.empty())
	{
		request_stream <<"?fields=";
		request_stream << *fields.begin();
		for (std::list<std::string>::iterator i = ++fields.begin(); i != fields.end(); ++i)
			request_stream << ", " << *i; 
		request_stream << '&';
	}

	request_stream << "access_token=" << token << "\r\n";
	request_stream << "Host: graph.facebook.com\r\n";
	request_stream << "Connection: close\r\n";
	request_stream << "Accept: text/html\r\n\r\n";

	boost::asio::write (m_socket, request);
}

// --------------------------------------------------------------------------------------------
boost::property_tree::ptree FacebookGraph::ParseResponse ()
// --------------------------------------------------------------------------------------------
{
	boost::system::error_code error;
	boost::asio::streambuf response;
	std::istream response_stream (&response);

	ParseHTTPHeader (response, response_stream, m_socket);

	// Read until EOF
	response_stream.clear ();
    while (boost::asio::read (m_socket, response, boost::asio::transfer_at_least(1), error));
    if (error != boost::asio::error::eof)
		throw boost::system::system_error (error);

	// Parse body
	boost::property_tree::ptree pt;
	boost::property_tree::json_parser::read_json (response_stream, pt);

	return pt;
}

// --------------------------------------------------------------------------------------------
void ParseHTTPHeader (boost::asio::streambuf& response, std::istream& response_stream, ssl_socket& socket)
// --------------------------------------------------------------------------------------------
{
	// read first line - header status
	boost::asio::read_until (socket, response, "\r\n");

	unsigned int status_code;
    std::string http_version;
	std::string status_message;

    response_stream >> http_version;
    response_stream >> status_code;
    std::getline (response_stream, status_message);

    if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		throw std::exception ("Invalid facebook response");

    if (status_code != 200)
	{
		std::string err ("Facebook response returned with status code ");
		err.append (boost::lexical_cast<std::string> (status_code));
		throw std::exception (err.c_str());
	}

    // Read and process the response headers, which are terminated by a blank line.
	std::string header;
    boost::asio::read_until (socket, response, "\r\n\r\n");
    while (std::getline (response_stream, header) && header != "\r");
}

// --------------------------------------------------------------------------------------------
bool VerifCertificate(bool preverified, boost::asio::ssl::verify_context& ctx)
// --------------------------------------------------------------------------------------------
{
	//char subject_name[256];
	//X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
	//X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);

	return true;
}


}
}