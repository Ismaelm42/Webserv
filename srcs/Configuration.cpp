#include "../includes/lib.hpp"

Configuration::Configuration(int argc, char **argv)
{
	if (argc > 2)
		throw std::runtime_error("Error: Invalid number of arguments. You must specify exactly one configuration file.");
	argc == 1 ? _fileName = "./default.conf" : _fileName = argv[1];
	checkFileOrDirectory(_fileName, "file");
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
		throw std::runtime_error(logError("Error: \"" + *_itToken + "\": is a directory"));
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
	_itToken++;
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
	std::string str = *(_tokens.begin() + 1);
	if (str.find(";") != str.size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"autoindex\" directive"));
	str.erase(str.size() - 1);
	if (str != "on" && str != "off")
		throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"autoindex\" directive"));
	if (str == "on")
		_itConfig->locations.back().autoindex = true;
}

void Configuration::setReturn()
{
	if (_tokens.end() - _tokens.begin() != 2 && _tokens.end() - _tokens.begin() != 3)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"return\" directive"));
	std::string str = *(_itToken + 1);
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
		str = *(_itToken + 2);
		if (str.rfind(";") != str.size() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"return\" directive"));
		str.erase(str.size() - 1);
		_itConfig->locations.back().redir.second = str;
	}
}

void Configuration::handleLocations()
{
	if (_itConfig->root == "")
		throw std::runtime_error(logError("Error: \"root\" directive is required to set \"location\" directive"));
	_itToken = _tokens.begin();
	if (_inLocationBlock == false)
	{
		if (_tokens.end() - _tokens.begin() != 3)
			throw std::runtime_error(logError("Error: invalid number of arguments in \"location\" directive"));
		*(_itToken + 1) = _itConfig->root + *(_itToken + 1);
		checkFileOrDirectory(*(_itToken + 1), "dir");
		if (*(_itToken + 2) != "{")
			throw std::runtime_error(logError("Error: unexpected \"" + *(_itToken + 2) + "\". open curly brace missing in \"location\" directive"));
		Location_config config;
		_itConfig->locations.push_back(config);
		_itConfig->locations.back().location = *(_itToken + 1);
		_inLocationBlock = true;
	}
	else if (*_itToken == "allow_methods")
		setAllowedMethods();
	else if (*_itToken == "autoindex")
		setAutoindex();
	else if (*_itToken == "return" && !_itConfig->locations.back().redir.first)
		setReturn();
	else if (*_itToken == "}" && (_itToken + 1) == _tokens.end())
		_inLocationBlock = false;
	else
		throw std::runtime_error(logError("Error: unexpected \"" + *(_itToken) + "\" in \"location\" directive"));
}

void Configuration::setListenPort()
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
	_itConfig->ip_port.push_back(pair);
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
	checkFileOrDirectory(*_itToken, "dir");
	_itConfig->root = *_itToken;
}

void Configuration::setIndexFiles()
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
		if (_itToken->find('/') != 0)
			*_itToken = "/" + *_itToken;
		*_itToken = _itConfig->root + *_itToken;
		checkFileOrDirectory(*_itToken, "file");
		_itConfig->index.push_back(*_itToken);
	}
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
	checkFileOrDirectory(file, "file");
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

void Configuration::setMaxBodySize()
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
	_itConfig->body_size = std::atoi(body.c_str());
	if (body[i] == 'K')
		_itConfig->body_size *= 1024;
	if (body[i] == 'M')
		_itConfig->body_size *= 1024 * 1024;
	if (body[i] == 'G')
		_itConfig->body_size *= 1024 * 1024 * 1024;
	if (_itConfig->body_size > 5368709120)
		throw std::runtime_error(logError("Error: size limited to 5GB in \"client_max_body_size\" directive"));
}

void Configuration::endServerBlock()
{
	if (_itToken == _tokens.end())
		_inServerBlock = false;
	else
		throw std::runtime_error(logError("Error: syntax error after closing brace"));
}

void Configuration::checkFormatError()
{

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
			setListenPort();
		else if (_tokens[0] == "server_name")
			setServerName();
		else if (_tokens[0] == "root")
			setRootDirectory();
		else if (_tokens[0] == "index")
			setIndexFiles();
		else if (_tokens[0] == "error_page")
			setErrorPages();
		else if (_tokens[0] == "client_max_body_size")
			setMaxBodySize();
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
	printServerConfig();
}




















//Comprobación de los datos de la configuración guardados en la estructura:
void Configuration::printServerConfig()
{
	int i = 0;
	for (_itServer = _serversConfig.begin(); _itServer != _serversConfig.end(); _itServer++)
	{
		Server_config *it = *_itServer;
		std::cout << Yellow << "[SERVER " << i + 1 << "]" << Reset << std::endl << std::endl;
		for (std::vector<std::pair<std::string, int> >::iterator itip = it->ip_port.begin(); itip != it->ip_port.end(); itip++)
		{
			std::cout << "Listen IP:\t\t" << itip->first << std::endl;
			std::cout << "Listen PORT:\t\t" << itip->second << std::endl;
		}
		std::cout << "Server names:\t\t";
		for (std::vector<std::string>::iterator its = it->server_names.begin(); its != it->server_names.end(); its++)
		{
			std::cout << *its;
			if (its != it->server_names.end() - 1)
				std::cout << "\t";
		}
		std::cout << std::endl;
		std::cout << "Root:\t\t\t" << it->root << std::endl;
		std::cout << "Index:\t\t\t";
		for (std::vector<std::string>::iterator its = it->index.begin(); its != it->index.end(); its++)
		{
			std::cout << *its;
			if (its != it->index.end() - 1)
				std::cout << "  ";
		}
		std::cout << std::endl;
		for (std::map<int, std::string>::iterator itm = it->error_pages.begin(); itm != it->error_pages.end(); itm++)
		{
			std::cout << "Error page:\t\t" << itm->first << "->" << itm->second << std::endl;
		}
		std::cout << "Client max body size:\t" << it->body_size << " bytes" << std::endl;
		for (std::vector<Location_config>::iterator itl = it->locations.begin(); itl != it->locations.end(); itl++)
		{
			std::cout << "\nLocation:\t\t" << itl->location << std::endl;
			std::cout << "\tmethods\t\t";
			for (std::set<std::string>::iterator it = itl->methods.begin(); it != itl->methods.end(); it++)
			{
				std::cout << *it << " ";
			}
			std::cout << std::endl;
			if (itl->autoindex)
				std::cout << "\tautoindex:\ton" << std::endl;
			else
				std::cout << "\tautoindex:\toff" << std::endl;
			if (itl->redir.first != 0)
				std::cout << "\tredir code:\t" << itl->redir.first << std::endl;
			if (itl->redir.second != "")
				std::cout << "\tredir URI:\t" << itl->redir.second << std::endl;
		}
		std::cout << std::endl << std::endl;
		i++;
	}
}
