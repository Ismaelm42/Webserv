#include "../includes/lib.hpp"

Configuration::Configuration(int argc, char **argv)
{
	struct stat stat_buffer;
	
	if (argc > 2)
		throw std::runtime_error("Error: expected only one argument");
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
		throw std::runtime_error(logError("Error: syntax error"));
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

}
// Comprobar? Quedaría bien guapo
// 0.0.0.0 a 255.255.255.255 y de 0 a 65535
void Configuration::setListenPort()
{
	std::pair<std::string, int> pair;
	Server_config *it = *_its;

	_itt = _tokens.begin() + 1;
	size_t cPos = _itt->find(":");
	size_t c2Pos = _itt->find(";");

	if (_itt == _tokens.end() || c2Pos == 0)
		throw std::runtime_error(logError("Error: no arguments"));
	if (_itt != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: too many arguments"));
	if (c2Pos != _itt->size() - 1)
		throw std::runtime_error(logError("Error: listen syntax error"));
	if (cPos == std::string::npos)
		pair = std::make_pair("0.0.0.0", std::atoi(_itt->c_str()));
	else
		pair = std::make_pair(_itt->substr(0, cPos),std::atoi(_itt->substr(cPos + 1, 5).c_str()));
	it->ip_port.push_back(pair);
	_tokens.clear();
}

void Configuration::setServerName()
{
	size_t cPos;

	Server_config *it = *_its;
	_itt = _tokens.begin() + 1;

	if (_itt == _tokens.end())
		throw std::runtime_error(logError("Error: no arguments"));
	for (; _itt != _tokens.end(); _itt++)
	{
		cPos = _itt->find(";");
		if (cPos != std::string::npos && (_itt + 1 != _tokens.end() || cPos != _itt->size() - 1 || cPos == 0))
			throw std::runtime_error(logError("Error: server_name syntax error"));
		if (cPos == std::string::npos && _itt == _tokens.end() - 1)
			throw std::runtime_error(logError("Error: server_name syntax error"));
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

	if (_itt == _tokens.end())
		throw std::runtime_error(logError("Error: no arguments"));
	if (_itt != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: too many arguments"));
	if (cPos != _itt->size() - 1)
		throw std::runtime_error(logError("Error: listen syntax error"));
	_itt->erase(_itt->size() - 1);
	if (stat(_itt->c_str(), &stat_buffer) < 0)
        throw std::runtime_error(logError("Error: " + *_itt + ": " + std::string(strerror(errno))));
	if (!S_ISDIR(stat_buffer.st_mode))
		throw std::runtime_error(logError("Error: " + *_itt + ": is not a directory"));
    if (!(stat_buffer.st_mode & S_IRUSR))
		throw std::runtime_error(logError("Error: " + *_itt + ": permission denied"));
	it->root = *_itt;
	_tokens.clear();
}

void Configuration::setIndexFile()
{

}

void Configuration::setErrorPage()
{

}

void Configuration::setMaxBodySize()
{

}

void Configuration::endServerBlock()
{

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
	else if (_tokens[0] == "listen")
		setListenPort();
	else if (_tokens[0] == "server_name")
		setServerName();
	else if (_tokens[0] == "root")
		setRootDirectory();
	else if (_tokens[0] == "index")
		setIndexFile();
	else if (_tokens[0] == "error_page")
		setErrorPage();
	else if (_tokens[0] == "client_max_body_size")
		setMaxBodySize();
	else if (_tokens[0] == "}")
		endServerBlock();
	else
		checkFormatError();
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
		std::cout << Yellow << "[SERVER " << i << "]" << Reset << std::endl;
		for (std::vector<std::pair<std::string, int> >::iterator itip = it->ip_port.begin(); itip != it->ip_port.end(); itip++)
		{
			std::cout << "Listen IP: " << itip->first << std::endl;
			std::cout << "Listen PORT: " << itip->second << std::endl << std::endl;
		}
		std::cout << "Server names: ";
		for (std::vector<std::string>::iterator its = it->server_names.begin(); its != it->server_names.end(); its++)
		{
			std::cout << *its;
			if (its != it->server_names.end() - 1)
				std::cout << " - ";
		}
		std::cout << std::endl;
		std::cout << "Root: " << it->root << std::endl;
		std::cout << "Index: ";
		for (std::vector<std::string>::iterator its = it->index.begin(); its != it->index.end(); its++)
		{
			std::cout << *its;
			if (its != it->index.end() - 1)
				std::cout << " - ";
		}
		std::cout << std::endl;
		for (std::map<int, std::string>::iterator itm = it->error_pages.begin(); itm != it->error_pages.end(); itm++)
		{
			std::cout << "Error page:" << itm->first << "\t\t" << itm->second << std::endl;
		}
		std::cout << "Client max body size: " << it->body_size << std::endl;
		std::cout << std::endl << std::endl;
		i++;
	}
}
