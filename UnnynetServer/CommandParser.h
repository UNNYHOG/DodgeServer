#ifndef _COMMAND_PARSER_H_
#define _COMMAND_PARSER_H_

#include <string>
#include <vector>
#include <boost/noncopyable.hpp>

namespace unhg 
{
namespace unnynet
{

class UnnynetServer;

class CommandParser : private boost::noncopyable
{
private:

	UnnynetServer* m_server;

public:

	CommandParser (UnnynetServer* server);
	std::string Parse (const std::string& commandline);

private:

	bool ParseCommand (const std::string& command, std::vector<char>& proc_name, std::vector<std::vector<char> >& params);

};

}
}

#endif