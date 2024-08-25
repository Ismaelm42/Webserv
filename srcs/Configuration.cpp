#include "../includes/lib.hpp"

Configuration::Configuration(int argc, char **argv)
{
	struct stat stat_buffer;
	
	if (argc > 2)
		throw std::runtime_error("Error: Invalid number of arguments. You must specify exactly one configuration file.");
	argc == 1 ? _fileName = "./default.conf" : _fileName = argv[1];
	if (stat(_fileName.c_str(), &stat_buffer) < 0)
        throw std::runtime_error("Error: " + _fileName + ": " + std::string(strerror(errno)));
	if (!S_ISREG(stat_buffer.st_mode))
		throw std::runtime_error("Error: " + _fileName + ": is a directory");
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

void Configuration::splitWords(std::string& line)
{
	std::istringstream stream(line);
	std::string word;
	size_t cPos;

	while (stream >> word)
	{
		cPos = word.find('#');
		if (cPos != std::string::npos)
		{
			word.erase(cPos);
			if (!word.empty())
				_tokens.push_back(word);
			break ;
		}
        _tokens.push_back(word);
	}
}

void Configuration::initServerBlock()
{
	if (_tokens.size() > 2)
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
		_its = _serversConfig.end() - 1;
		_tokens.clear();
	}
}

void Configuration::handleLocations()
{
	Server_config *it = *_its;
	struct stat stat_buffer;
	std::string str;
	_itt = _tokens.begin();

	if (_inLocationBlock == false)
	{
		if (_tokens.end() - _tokens.begin() != 3)
			throw std::runtime_error(logError("Error: invalid number of arguments in \"location\" directive"));
		str = *(_tokens.begin() + 1);
		if (stat(str.c_str(), &stat_buffer) < 0)
	        throw std::runtime_error(logError("Error: \"" + str + "\": " + std::string(strerror(errno))));
		if (!S_ISDIR(stat_buffer.st_mode))
			throw std::runtime_error(logError("Error: \"" + str + "\": is not a directory"));
		if (!(stat_buffer.st_mode & S_IRUSR))
			throw std::runtime_error(logError("Error: \"" + str + "\": permission denied"));
		if (*(_tokens.begin() + 2) != "{")
			throw std::runtime_error(logError("Error: unexpected \"" + *(_tokens.begin() + 2) + "\". open curly brace missing in \"location\" directive"));
		Location_config config;
		it->locations.push_back(config);
		it->locations.back().location = str;
		_inLocationBlock = true;
	}
	else if (*_itt == "allow_methods")
	{
		_itt++;
		for (; _itt != _tokens.end(); _itt++)
		{
			str = *_itt;
			if (_itt == _tokens.end() - 1)
			{
				if (str.find(";") != str.size() - 1)
					throw std::runtime_error(logError("Error: syntax error in \"allow_methods\" directive"));
				str.erase(str.size() - 1);
			}
			if (str != "GET" && str != "POST" && str != "DELETE")
				throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"allow_methods\" directive"));
			it->locations.back().methods.insert(str);
		}
	}
	else if (*_itt == "autoindex")
	{
		if (_tokens.end() - _tokens.begin() != 2)
			throw std::runtime_error(logError("Error: invalid number of arguments in \"autoindex\" directive"));
		str = *(_tokens.begin() + 1);
		if (str.find(";") != str.size() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"autoindex\" directive"));
		str.erase(str.size() - 1);
		if (str != "on" && str != "off")
			throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"autoindex\" directive"));
		if (str == "on")
			it->locations.back().autoindex = true;
	}
	else if (*_itt == "return" && !it->locations.back().redir.first)
	{
		if (_tokens.end() - _tokens.begin() != 2 && _tokens.end() - _tokens.begin() != 3)
			throw std::runtime_error(logError("Error: invalid number of arguments in \"return\" directive"));
		str = *(_itt + 1);
		if (_tokens.end() - _tokens.begin() == 2)
		{
			if (str.find(";") != str.size() - 1)
				throw std::runtime_error(logError("Error: syntax error in \"return\" directive1"));
			str.erase(str.size() - 1);
		}
		if (str.size() != 3)
			throw std::runtime_error(logError("Error: error code must be exactly 3 digits in \"return\" directive"));
		if (!std::isdigit(str[0]) || !std::isdigit(str[1]) || !std::isdigit(str[2]))
			throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"return\" directive"));
		int nbr = std::atoi(str.c_str());
		if (nbr < 100 || nbr > 599)
			throw std::runtime_error(logError("Error: error code is out of range: \"" + str + "\" in \"return\" directive"));
		it->locations.back().redir.first = nbr;
		if (_tokens.end() - _tokens.begin() == 3)
		{
			str = *(_itt + 2);
			if (str.rfind(";") != str.size() - 1)
				throw std::runtime_error(logError("Error: syntax error in \"return\" directive2"));
			str.erase(str.size() - 1);
			it->locations.back().redir.second = str;
		}
	}
	else if (*_itt == "}" && _itt + 1 == _tokens.end())
		_inLocationBlock = false;
	else
		throw std::runtime_error(logError("Error: unexpected \"" + *_itt + "\" in \"location\" directive"));
	_tokens.clear();
}

void Configuration::setListenPort()
{
	std::pair<std::string, int> pair;
	struct sockaddr_in host_check;
	Server_config *it = *_its;
	std::string host;
	std::string port;
	int port_check;

	_itt = _tokens.begin() + 1;
	size_t cPos = _itt->find(":");
	size_t c2Pos = _itt->find(";");

	if (_itt == _tokens.end() || c2Pos == 0 || _itt != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"listen\" directive"));
	if (c2Pos != _itt->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"listen\" directive"));
	if (cPos == std::string::npos)
	{
		host = "0.0.0.0";
		port = *_itt;
	}
	else
	{
		host = _itt->substr(0, cPos);
		port = _itt->substr(cPos + 1, 6);
	}
	if (inet_pton(AF_INET, host.c_str(), &(host_check.sin_addr)) != 1)
		throw std::runtime_error(logError("Error: ip adress is not valid \"" + host + "\" in \"listen\" directive"));
	port.erase(port.size() - 1);
	for (int i = 0; port[i] != 0; i++)
		if (port[i] < '0' || port[i] > '9')
			throw std::runtime_error(logError("Error: unexpected \"" + port + "\""));
	port_check = std::atoi(port.c_str());
	if (port_check < 0 || port_check > 65536)
		throw std::runtime_error(logError("Error: port number is out of range: \"" + port + "\" in \"listen\" directive"));
	pair = std::make_pair(host, std::atoi(port.c_str()));
	it->ip_port.push_back(pair);
	_tokens.clear();
}

void Configuration::setServerName()
{
	size_t cPos;

	Server_config *it = *_its;
	_itt = _tokens.begin() + 1;

	if (_itt == _tokens.end())
		throw std::runtime_error(logError("Error: invalid number of arguments in \"server_name\" directive"));
	for (; _itt != _tokens.end(); _itt++)
	{
		cPos = _itt->find(";");
		if (cPos != std::string::npos && (_itt + 1 != _tokens.end() || cPos != _itt->size() - 1 || cPos == 0))
			throw std::runtime_error(logError("Error: syntax error in \"server_name\" directive"));
		if (cPos == std::string::npos && _itt == _tokens.end() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"server_name\" directive"));
		if (cPos != std::string::npos)
		{
			_itt->erase(_itt->size() - 1);
			it->server_names.push_back(*_itt);
		}
		else
			it->server_names.push_back(*_itt);
	}
	_tokens.clear();
}

void Configuration::setRootDirectory()
{
	struct stat stat_buffer;

	Server_config *it = *_its;
	_itt = _tokens.begin() + 1;
	size_t cPos = _itt->find(";");

	if (_itt == _tokens.end() || cPos == 0)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"root\" directive"));
	if (_itt != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: too many arguments in \"root\" directive"));
	if (cPos != _itt->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"root\" directive"));
	_itt->erase(_itt->size() - 1);
	if (stat(_itt->c_str(), &stat_buffer) < 0)
        throw std::runtime_error(logError("Error: \"" + *_itt + "\": " + std::string(strerror(errno))));
	if (!S_ISDIR(stat_buffer.st_mode))
		throw std::runtime_error(logError("Error: \"" + *_itt + "\": is not a directory"));
    if (!(stat_buffer.st_mode & S_IRUSR))
		throw std::runtime_error(logError("Error: \"" + *_itt + "\": permission denied"));
	it->root = *_itt;
	_tokens.clear();
}

void Configuration::setIndexFiles()
{
	struct stat stat_buffer;
	size_t cPos;

	Server_config *it = *_its;
	_itt = _tokens.begin() + 1;

	if (_itt == _tokens.end())
		throw std::runtime_error(logError("Error: invalid number of arguments in \"index\" directive"));
	for (; _itt != _tokens.end(); _itt++)
	{
		cPos = _itt->find(";");
		if (cPos != std::string::npos && (_itt + 1 != _tokens.end() || cPos != _itt->size() - 1 || cPos == 0))
			throw std::runtime_error(logError("Error: syntax error in \"index\" directive"));
		if (cPos == std::string::npos && _itt == _tokens.end() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"index\" directive"));
		if (cPos != std::string::npos)
		{
			_itt->erase(_itt->size() - 1);
			if (stat(_itt->c_str(), &stat_buffer) < 0)
        		throw std::runtime_error(logError("Error: \"" + *_itt + "\": " + std::string(strerror(errno))));
			if (S_ISDIR(stat_buffer.st_mode))
				throw std::runtime_error(logError("Error: \"" + *_itt + "\": is a directory"));
    		if (!(stat_buffer.st_mode & S_IRUSR))
				throw std::runtime_error(logError("Error: \"" + *_itt + "\": permission denied"));
			it->index.push_back(*_itt);
		}
		else
			it->index.push_back(*_itt);
	}
	_tokens.clear();
}

void Configuration::setErrorPages()
{
	Server_config *it = *_its;
	_itt = _tokens.begin() + 1;
	struct stat stat_buffer;
	std::string file;
	std::string code;
	size_t cPos;

	if (_itt == _tokens.end() || _tokens.begin() + 2 == _tokens.end())
		throw std::runtime_error(logError("Error: invalid number of arguments in \"error_page\" directive"));
	file = *(_tokens.end() - 1);
	cPos = file.find(";");
	if (cPos != file.size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"error_page\" directive"));
	file.erase(file.size() - 1);
	if (stat(file.c_str(), &stat_buffer) < 0)
    	throw std::runtime_error(logError("Error: \"" + file + "\": " + std::string(strerror(errno))));
	if (S_ISDIR(stat_buffer.st_mode))
		throw std::runtime_error(logError("Error: \"" + file + "\": is a directory"));
    if (!(stat_buffer.st_mode & S_IRUSR))
		throw std::runtime_error(logError("Error: \"" + file + "\": permission denied"));
	for (; _itt != _tokens.end() - 1; _itt++)
	{
		code = *_itt;
		if (code.size() != 3)
			throw std::runtime_error(logError("Error: error code must be exactly 3 digits in \"error_page\" directive"));
		if (!std::isdigit(code[0]) || !std::isdigit(code[1]) || (!std::isdigit(code[2]) && code[2] != 'x'))
			throw std::runtime_error(logError("Error: unexpected \"" + code + "\" in \"error_page\" directive"));
		int nbr = std::atoi(code.c_str());
		if ((code[2] == 'x' && (nbr < 9 || nbr > 59)) || (code[2] != 'x' && (nbr < 100 || nbr > 599)))
			throw std::runtime_error(logError("Error: error code is out of range: \"" + code + "\" in \"error_page\" directive"));
		if (code[2] == 'x' && (_itt != _tokens.end() - 2 || _itt != _tokens.begin() + 1))
			throw std::runtime_error(logError("Error: syntax error in \"error_page\" directive"));
		if (code[2] == 'x')
			for (int i = 0; i < 10; i++)
				it->error_pages[(nbr * 10) + i] = file;
		else
			it->error_pages[nbr] = file;
	}
	_tokens.clear();
}

void Configuration::setMaxBodySize()
{
	std::string body;
	unsigned i;

	Server_config *it = *_its;
	_itt = _tokens.begin() + 1;
	size_t cPos = _itt->find(";");

	if (_itt == _tokens.end() || cPos == 0)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"client_max_body_size\" directive"));
	if (_itt != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: too many arguments in \"client_max_body_size\" directive"));
	if (cPos != _itt->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"client_max_body_size\" directive"));
	_itt->erase(_itt->size() - 1);
	body = *_itt;
	for (i = 0; body[i] != 0; i++)
		if (body[i] < '0' || body[i] > '9')
			break ;
	if ((i != body.size() && i != body.size() - 1) || (i == body.size() - 1 && (body[i] != 'K' && body[i] != 'M' && body[i] != 'G')))
		throw std::runtime_error(logError("Error: unexpected \"" + body + "\" in \"client_max_body_size\" directive"));
	it->body_size = std::atoi(body.c_str());
	if (body[i] == 'K')
		it->body_size *= 1024;
	if (body[i] == 'M')
		it->body_size *= 1024 * 1024;
	if (body[i] == 'G')
		it->body_size *= 1024 * 1024 * 1024;
	if (it->body_size > 5368709120)
		throw std::runtime_error(logError("Error: size limited to 5GB in \"client_max_body_size\" directive"));
	_tokens.clear();
}

void Configuration::endServerBlock()
{
	_tokens.clear();

}

void Configuration::checkFormatError()
{
	_tokens.clear();

}

void Configuration::handleConfigLine()
{
	if (_tokens.empty())
		return ;
	else if (_inServerBlock == false)
		initServerBlock();
	else if (_inLocationBlock == true || _tokens[0] == "location")
		handleLocations();
	else if (_tokens[0] == "listen")
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
	// else
	// 	throw std::runtime_error(logError("Error: unexpected \"" + _tokens[0] + "\""));
}

void Configuration::parsing()
{
	std::string line;

	while (getline(_file, line))
	{
		_loopCounter++;
		splitWords(line);
		handleConfigLine();
	}
	printServerConfig();
}




















//Comprobación de los datos de la configuración guardados en la estructura:
void Configuration::printServerConfig()
{
	int i = 0;
	for (_its = _serversConfig.begin(); _its != _serversConfig.end(); _its++)
	{
		Server_config *it = *_its;
		std::cout << Yellow << "[SERVER " << i << "]" << Reset << std::endl << std::endl;
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
			std::cout << "Location:\t\t" << itl->location << std::endl;
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
			std::cout << "\tredir code:\t" << itl->redir.first << std::endl;
			if (itl->redir.second != "")
				std::cout << "\tredir URI:\t" << itl->redir.second << std::endl;
		}
		std::cout << std::endl << std::endl;
		i++;
	}
}
