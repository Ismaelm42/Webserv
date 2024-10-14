#include "../includes/lib.hpp"

Configuration::Configuration(int argc, char **argv)
{
	if (argc > 2)
		throw std::runtime_error("Error: Invalid number of arguments. You must specify exactly one configuration file.");
	argc == 1 ? _fileName = "./default.conf" : _fileName = argv[1];
	struct stat stat_buffer;
	if (stat(_fileName.c_str(), &stat_buffer) < 0)
	       throw std::runtime_error("Error: \"" + _fileName + "\": " + std::string(strerror(errno)));
	if (S_ISDIR(stat_buffer.st_mode))
		throw std::runtime_error("Error: \"" + _fileName + "\": is a directory");
	if (!(stat_buffer.st_mode & S_IRUSR))
		throw std::runtime_error("Error: \"" + _fileName + "\": permission denied");
	_file.open(_fileName.c_str());
	if (!_file.is_open())
		throw std::runtime_error("Error: " + _fileName + ": " + std::string(strerror(errno)));
	_inServerBlock = false;
	_inLocationBlock = false;
	_loopCounter = 0;
}

Configuration::~Configuration()
{
	for (std::vector<Server_config*>::iterator it = _serversConfig.begin(); it != _serversConfig.end(); it++)
		delete *it;
	if (_file.is_open())
		_file.close();
}

std::string Configuration::logError(std::string message)
{
	std::ostringstream ss;
	ss << message << " in " << _fileName << ":" << _loopCounter;
	std::string error_message = ss.str();
	return error_message;
}

void Configuration::checkFileOrDirectory(std::string &path, const std::string type)
{
	struct stat stat_buffer;

	if (stat(path.c_str(), &stat_buffer) < 0)
	    throw std::runtime_error(logError("Error: \"" + path + "\": " + std::string(strerror(errno))));
	if (type == "dir" && !S_ISDIR(stat_buffer.st_mode))
		throw std::runtime_error(logError("Error: \"" + path + "\": is not a directory"));
	if (type == "file" && S_ISDIR(stat_buffer.st_mode))
		throw std::runtime_error(logError("Error: \"" + path + "\": is a directory"));
	if (!(stat_buffer.st_mode & S_IRUSR))
		throw std::runtime_error(logError("Error: \"" + path + "\": permission denied"));
}

void Configuration::createTokens(std::string& line)
{
	std::istringstream stream(line);
	std::string token;
	size_t cPos;

	while (stream >> token)
	{
		cPos = token.find('#');
		if (cPos != std::string::npos)
		{
			token.erase(cPos);
			if (!token.empty())
				_tokens.push_back(token);
			break ;
		}
        _tokens.push_back(token);
	}
}

void Configuration::initServerBlock()
{
	if (_tokens.size() != 2)
		throw std::runtime_error(logError("Error: syntax error in \"server\" directive"));
	if (_tokens.size() == 2)
	{
		if (_tokens[0] != "server")
			throw std::runtime_error(logError("Error: unexpected \"" + _tokens[0] + "\""));
		if (_tokens[1] != "{")
			throw std::runtime_error(logError("Error: unexpected \"" + _tokens[1] + "\""));
		_inServerBlock = true;
		Server_config* config = new Server_config;
		_serversConfig.push_back(config);
		_itServer = _serversConfig.end() - 1;
		_itConfig = *_itServer;
	}
}

void Configuration::setAllowedMethods()
{
	for (; _itToken != _tokens.end(); _itToken++)
	{
		std::string str = *_itToken;
		if (_itToken == _tokens.end() - 1)
		{
			if (str.find(";") != str.size() - 1)
				throw std::runtime_error(logError("Error: syntax error in \"allow_methods\" directive"));
			str.erase(str.size() - 1);
		}
		if (str != "GET" && str != "POST" && str != "DELETE")
			throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"allow_methods\" directive"));
		_itConfig->locations.back().methods.insert(str);
	}
}

void Configuration::setAutoindex()
{
	if (_tokens.end() - _tokens.begin() != 2)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"autoindex\" directive"));
	std::string str = *_itToken;
	if (str.find(";") != str.size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"autoindex\" directive"));
	str.erase(str.size() - 1);
	if (str != "on" && str != "off")
		throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"autoindex\" directive"));
	if (str == "on")
		_itConfig->locations.back().autoindex = true;
}

void Configuration::setRedir()
{
	if (_tokens.end() - _tokens.begin() != 2 && _tokens.end() - _tokens.begin() != 3)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"return\" directive"));
	std::string str = *_itToken;
	if (_tokens.end() - _tokens.begin() == 2)
	{
		if (str.find(";") != str.size() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"return\" directive"));
		str.erase(str.size() - 1);
	}
	if (str.size() != 3)
		throw std::runtime_error(logError("Error: error code must be exactly 3 digits in \"return\" directive"));
	if (!std::isdigit(str[0]) || !std::isdigit(str[1]) || !std::isdigit(str[2]))
		throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"return\" directive"));
	int nbr = std::atoi(str.c_str());
	if (nbr < 100 || nbr > 599)
		throw std::runtime_error(logError("Error: error code is out of range: \"" + str + "\" in \"return\" directive"));
	_itConfig->locations.back().redir.first = nbr;
	if (_tokens.end() - _tokens.begin() == 3)
	{
		str = *(_itToken + 1);
		if (str.rfind(";") != str.size() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"return\" directive"));
		str.erase(str.size() - 1);
		_itConfig->locations.back().redir.second = str;
	}
}

void Configuration::setCgi()
{
	if (_tokens.end() - _tokens.begin() != 3)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"cgi\" directive"));
	std::string first = *_itToken;
	if (first.find(";") != std::string::npos || first.find(".") != 0)
		throw std::runtime_error(logError("Error: syntax error in \"cgi\" directive"));
	std::string second = *(_itToken + 1);
	if (second.rfind(";") != second.size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"cgi\" directive"));
	second.erase(second.size() - 1);
	_itConfig->locations.back().cgi.push_back(std::make_pair(first, second));
}

void Configuration::setAuthBasic()
{
	std::string str = *(_itToken);
	if (str == "off;")
		_itConfig->locations.back().auth_basic = "off";
	else
	{
		str.append(" ");
		str.append(*(_itToken + 1));
		if (str.find(";") != str.size() - 1)
		 	throw std::runtime_error(logError("Error: syntax error in \"auth_basic\" directive"));
		str.erase(str.size() - 1);
		if (str[str.size() - 1] == '\"')
			str.erase(str.size() - 1);
		if (str[0] == '\"')
			str.erase(0, 1);
		_itConfig->locations.back().auth_basic = str;
	}
}

void Configuration::setAuthBasicUserFile()
{
	std::string str = *(_itToken);
	if (str.find(";") != str.size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"auth_basic_user_file\" directive"));
	str.erase(str.size() - 1);
	checkFileOrDirectory(str, "file");
	_itConfig->locations.back().auth_basic_user_file = str;
}

void Configuration::handleLocations()
{
	if (_itConfig->root == "")
		throw std::runtime_error(logError("Error: \"root\" directive is required to set \"location\" directive"));
	_itToken = _tokens.begin() + 1;
	if (_inLocationBlock == false)
	{
		if (_tokens.end() - _tokens.begin() != 3)
			throw std::runtime_error(logError("Error: invalid number of arguments in \"location\" directive"));
		*(_itToken) = _itConfig->root + *(_itToken);
		if (*(_itToken + 1) != "{")
			throw std::runtime_error(logError("Error: unexpected \"" + *(_itToken + 1) + "\". open curly brace missing in \"location\" directive"));
		Location_config config;
		_itConfig->locations.push_back(config);
		_itConfig->locations.back().location = *(_itToken);
		_inLocationBlock = true;
	}
	else if (_tokens[0] == "root")
		setLocationRootDirectory();
	else if (_tokens[0] == "allow_methods")
		setAllowedMethods();
	else if (_tokens[0] == "autoindex")
		setAutoindex();
	else if (_tokens[0] == "client_max_body_size")
		setMaxBodySize(_itConfig->locations.back().body_size);
	else if (_tokens[0] == "return" && !_itConfig->locations.back().redir.first)
		setRedir();
	else if (_tokens[0] == "index")
		setIndexFiles(_itConfig->locations.back().location, _itConfig->locations.back().index);
	else if (_tokens[0] == "cgi")
		setCgi();
	else if (_tokens[0] == "auth_basic" && _tokens[0] != "auth_basic_")
		setAuthBasic();
	else if (_tokens[0] == "auth_basic_user_file")
		setAuthBasicUserFile();
	else if (_tokens[0] == "}" && (_itToken) == _tokens.end())
	{
		_inLocationBlock = false;
		if (_itConfig->locations.back().body_size == 0)
			_itConfig->locations.back().body_size = _itConfig->body_size;
	}
	else
		throw std::runtime_error(logError("Error: unexpected \"" + _tokens[0] + "\" in \"location\" directive"));
}

void Configuration::setListen()
{
	size_t cPos = _itToken->find(":");
	size_t c2Pos = _itToken->find(";");
	if (_itToken == _tokens.end() || c2Pos == 0 || _itToken != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"listen\" directive"));
	if (c2Pos != _itToken->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"listen\" directive"));
	std::string host = (cPos == std::string::npos) ? "0.0.0.0" : _itToken->substr(0, cPos);
	std::string port = (cPos == std::string::npos) ? *_itToken : _itToken->substr(cPos + 1, 6);
	struct sockaddr_in host_check;
	if (inet_pton(AF_INET, host.c_str(), &(host_check.sin_addr)) != 1)
		throw std::runtime_error(logError("Error: ip adress is not valid \"" + host + "\" in \"listen\" directive"));
	port.erase(port.size() - 1);
	for (int i = 0; port[i] != 0; i++)
		if (port[i] < '0' || port[i] > '9')
			throw std::runtime_error(logError("Error: unexpected \"" + port + "\""));
	int port_check = std::atoi(port.c_str());
	if (port_check < 0 || port_check > 65536)
		throw std::runtime_error(logError("Error: port number is out of range: \"" + port + "\" in \"listen\" directive"));
	std::pair<std::string, int> pair = std::make_pair(host, std::atoi(port.c_str()));
	_itConfig->host_port.push_back(pair);
}

void Configuration::setServerName()
{
	if (_itToken == _tokens.end())
		throw std::runtime_error(logError("Error: invalid number of arguments in \"server_name\" directive"));
	for (; _itToken != _tokens.end(); _itToken++)
	{
		size_t cPos = _itToken->find(";");
		if (cPos != std::string::npos && (_itToken + 1 != _tokens.end() || cPos != _itToken->size() - 1 || cPos == 0))
			throw std::runtime_error(logError("Error: syntax error in \"server_name\" directive"));
		if (cPos == std::string::npos && _itToken == _tokens.end() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"server_name\" directive"));
		if (cPos != std::string::npos)
		{
			_itToken->erase(_itToken->size() - 1);
			_itConfig->server_names.push_back(*_itToken);
		}
		else
			_itConfig->server_names.push_back(*_itToken);
	}
}

void Configuration::setRootDirectory()
{
	size_t cPos = _itToken->find(";");
	if (_itToken == _tokens.end() || cPos == 0)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"root\" directive"));
	if (_itToken != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: too many arguments in \"root\" directive"));
	if (cPos != _itToken->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"root\" directive"));
	_itToken->erase(_itToken->size() - 1);
	while (_itToken->rfind("/") == _itToken->size() - 1)
		_itToken->erase(_itToken->size() - 1);
	_itConfig->root = *_itToken;
}

void Configuration::setIndexFiles(std::string path, std::vector<std::string> &container)
{
	if (_itConfig->root == "")
		throw std::runtime_error(logError("Error: \"root\" directive is required to set \"index\" directive"));
	if (_itToken == _tokens.end())
		throw std::runtime_error(logError("Error: invalid number of arguments in \"index\" directive"));
	for (; _itToken != _tokens.end(); _itToken++)
	{
		size_t cPos = _itToken->find(";");
		if (cPos != std::string::npos && (_itToken + 1 != _tokens.end() || cPos != _itToken->size() - 1 || cPos == 0))
			throw std::runtime_error(logError("Error: syntax error in \"index\" directive"));
		if (cPos == std::string::npos && _itToken == _tokens.end() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"index\" directive"));
		if (cPos != std::string::npos)
			_itToken->erase(_itToken->size() - 1);
		if (_itToken->find('/') != 0 && path[path.size() - 1] != '/')
			*_itToken = "/" + *_itToken;
		*_itToken = path + *_itToken;
		container.push_back(*_itToken);
	}
}

void Configuration::setLocationRootDirectory()
{
	size_t cPos = _itToken->find(";");
	if (_itToken == _tokens.end() || cPos == 0)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"root\" directive"));
	if (_itToken != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: too many arguments in \"root\" directive"));
	if (cPos != _itToken->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"root\" directive"));
	_itToken->erase(_itToken->size() - 1);
	while (_itToken->rfind("/") == _itToken->size() - 1)
		_itToken->erase(_itToken->size() - 1);
	_itConfig->locations.back().root = *_itToken;
	std::cout << Red << "root: " << _itConfig->locations.back().root << Reset << std::endl;
}

void Configuration::setErrorPages()
{
	if (_itConfig->root == "")
		throw std::runtime_error(logError("Error: \"root\" directive is required to set \"error_page\" directive"));
	if (_itToken == _tokens.end() || _tokens.begin() + 2 == _tokens.end())
		throw std::runtime_error(logError("Error: invalid number of arguments in \"error_page\" directive"));
	std::string file = *(_tokens.end() - 1);
	size_t cPos = file.find(";");
	if (cPos != file.size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"error_page\" directive"));
	file.erase(file.size() - 1);
	if (file.find('/') != 0)
		file = "/" + file;
	file = _itConfig->root + file;
	for (; _itToken != _tokens.end() - 1; _itToken++)
	{
		std::string code = *_itToken;
		if (code.size() != 3)
			throw std::runtime_error(logError("Error: error code must be exactly 3 digits in \"error_page\" directive"));
		if (!std::isdigit(code[0]) || !std::isdigit(code[1]) || (!std::isdigit(code[2]) && code[2] != 'x'))
			throw std::runtime_error(logError("Error: unexpected \"" + code + "\" in \"error_page\" directive"));
		int nbr = std::atoi(code.c_str());
		if ((code[2] == 'x' && (nbr < 9 || nbr > 59)) || (code[2] != 'x' && (nbr < 100 || nbr > 599)))
			throw std::runtime_error(logError("Error: error code is out of range: \"" + code + "\" in \"error_page\" directive"));
		if (code[2] == 'x' && (_itToken != _tokens.end() - 2 || _itToken != _tokens.begin() + 1))
			throw std::runtime_error(logError("Error: syntax error in \"error_page\" directive"));
		if (code[2] == 'x')
			for (int i = 0; i < 10; i++)
				_itConfig->error_pages[(nbr * 10) + i] = file;
		else
			_itConfig->error_pages[nbr] = file;
	}
}

void Configuration::setMaxBodySize(size_t &bodySize)
{
	size_t cPos = _itToken->find(";");
	if (_itToken == _tokens.end() || cPos == 0)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"client_max_body_size\" directive"));
	if (_itToken != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: too many arguments in \"client_max_body_size\" directive"));
	if (cPos != _itToken->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"client_max_body_size\" directive"));
	_itToken->erase(_itToken->size() - 1);
	std::string body = *_itToken;
	unsigned i = 0;
	for (; body[i] != 0; i++)
		if (body[i] < '0' || body[i] > '9')
			break ;
	if ((i != body.size() && i != body.size() - 1) || (i == body.size() - 1 && (body[i] != 'K' && body[i] != 'M' && body[i] != 'G')))
		throw std::runtime_error(logError("Error: unexpected \"" + body + "\" in \"client_max_body_size\" directive"));
	bodySize = std::atoi(body.c_str());
	if (body[i] == 'K')
		bodySize *= 1024;
	if (body[i] == 'M')
		bodySize *= 1024 * 1024;
	if (body[i] == 'G')
		bodySize *= 1024 * 1024 * 1024;
	if (bodySize > 5368709120)
		throw std::runtime_error(logError("Error: size limited to 5GB in \"client_max_body_size\" directive"));
}

void Configuration::checkServerBlockErrors()
{
	if (_itConfig->host_port.empty())
		throw std::runtime_error("Error: no \"listen\" directive found in server block");
	if (_itConfig->server_names.empty())
		throw std::runtime_error("Error: no \"server_name\" directive found in server block");
	std::vector<Location_config>::iterator it = _itConfig->locations.begin();
	for (; it != _itConfig->locations.end(); it++)
		if (it->location == _itConfig->root + "/")
			break ;
	if (it == _itConfig->locations.end())
		throw std::runtime_error("Error: no location \"/\" directive found in server block");
	std::pair<std::set<std::pair<std::string, int> >::iterator, bool> dup_check;
    std::set<std::pair<std::string, int> > dup_cont;
	for (_itServer = _serversConfig.begin(); _itServer != _serversConfig.end(); _itServer++)
	{
		_itConfig = *_itServer;
		for (std::vector<std::pair<std::string, int> >::iterator it = _itConfig->host_port.begin(); it != _itConfig->host_port.end(); it++)
		{
			dup_check = dup_cont.insert(*it);
			if (dup_check.second == false)
			{
				std::ostringstream ss;
				ss << it->first << ":" << it->second;
				throw std::runtime_error("Error: invalid configuration: \"" + ss.str() + "\" is configured more than once or is duplicated.");
			}
		}
	}
}

void Configuration::endServerBlock()
{
	if (_itToken == _tokens.end())
		_inServerBlock = false;
	else
		throw std::runtime_error(logError("Error: syntax error after closing brace"));
	checkServerBlockErrors();
}

void Configuration::handleConfigLine()
{
	if (_tokens.empty())
		return ;
	else if (_inServerBlock == false)
		initServerBlock();
	else if (_inLocationBlock == true || _tokens[0] == "location")
		handleLocations();
	else
	{
		_itToken = _tokens.begin() + 1;
		if (_tokens[0] == "listen")
			setListen();
		else if (_tokens[0] == "server_name")
			setServerName();
		else if (_tokens[0] == "root")
			setRootDirectory();
		else if (_tokens[0] == "index")
			setIndexFiles(_itConfig->root, _itConfig->index);
		else if (_tokens[0] == "error_page")
			setErrorPages();
		else if (_tokens[0] == "client_max_body_size")
			setMaxBodySize(_itConfig->body_size);
		else if (_tokens[0] == "}")
			endServerBlock();
		else
			throw std::runtime_error(logError("Error: unexpected \"" + _tokens[0] + "\""));
	}
}

void Configuration::parsing()
{
	std::string line;

	while (getline(_file, line))
	{
		_loopCounter++;
		createTokens(line);
		handleConfigLine();
		_tokens.clear();
	}
}
