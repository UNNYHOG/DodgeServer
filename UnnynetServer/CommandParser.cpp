#include "CommandParser.h"
#include "UnnynetServer.h"

#include <iostream>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/lexical_cast.hpp>

namespace qi      = boost::spirit::qi;
namespace ascii   = boost::spirit::ascii;
namespace phoenix = boost::phoenix;

namespace unhg 
{
namespace unnynet
{

// ----------------------------------------------------------------------------------------------
template<typename T>
T GetParam(const std::vector<std::string>& params, size_t index)
// ----------------------------------------------------------------------------------------------
{
	if ((index >= params.size()) || (index < 0))
		throw std::exception ("Not enough arguments");

	try
	{
		return boost::lexical_cast<T>(params[index]);
	}
	catch (std::exception exp)
	{
		throw std::exception ("Bad argument format");
	}
}

// -----------------------------------------------------------------------------------------------
CommandParser::CommandParser (UnnynetServer* server) : m_server (server)
// -----------------------------------------------------------------------------------------------
{
}

// -----------------------------------------------------------------------------------------------
std::string CommandParser::Parse (const std::string& commandline)
// -----------------------------------------------------------------------------------------------
{
	std::vector<char> _command;
	std::vector<std::vector<char> > _params;

	if (!ParseCommand (commandline, _command, _params))
		return "Parsing command failed";

	std::string command (_command.begin(), _command.end());
	std::transform (command.begin(), command.end(), command.begin(), ::tolower);

	std::vector<std::string> params;
	for (size_t i = 0; i < _params.size(); ++i)
		params.push_back (std::string(_params[i].begin(), _params[i].end()));

	try
	{
		if ((command == "quit") || (command == "q") || (command == "exit"))
		{
			m_server -> Stop ();
			return m_server -> Status ();
		}
		else if (command == "status")
			return m_server -> Status ();
		else if (command == "showsessions")
			return m_server -> ShowSessions ();
		else if (command == "killsession")
			return (m_server -> KillSession (GetParam<size_t> (params, 0)) ? "Done" : "No session with specified id");
		else return "Unknown command";
	}
	catch (std::exception exp)
	{
		return exp.what();
	}
}

// -----------------------------------------------------------------------------------------------
bool CommandParser::ParseCommand (const std::string& command, std::vector<char>& proc_name, std::vector<std::vector<char> >& params)
// -----------------------------------------------------------------------------------------------
{
	std::string::const_iterator begin = command.begin();
	bool r = qi::phrase_parse(begin, command.end(),
		(
			qi::lexeme[+qi::alpha[phoenix::push_back(phoenix::ref(proc_name), qi::_1)]] 
				>> *((qi::lexeme[+qi::alnum])[phoenix::push_back(phoenix::ref(params), qi::_1)] % ',')
        ) ,
        ascii::space);

    if (begin != command.end())
        return false;

    return r;
}

}
}