#include "../includes/lib.hpp"

Webserv::Webserv(int argc, char **argv)
{
	_config = new Configuration(argc, argv);
	_events = new Epoll_events;
	_events->epfd = epoll_create(1);
}

Webserv::~Webserv()
{
	delete _config;
	delete _events;
	for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		delete *it;
}

void Webserv::initializeServers()
{
	_config->parsing();
	if (_events->epfd < 0)
		throw std::runtime_error("Error: epoll_create: " + std::string(strerror(errno)));
	_events->log.resize(200);
	std::vector<std::pair<std::string, int> >::iterator it;
	for (_config->_itServer = _config->_serversConfig.begin(); _config->_itServer != _config->_serversConfig.end(); _config->_itServer++)
	{
		_config->_itConfig = *_config->_itServer;
		for (it = _config->_itConfig->host_port.begin(); it != _config->_itConfig->host_port.end(); it++)
		{
			Server *server = new Server(it->first, it->second, _events, _config->_itConfig);
			_servers.push_back(server);
		}
	}
}

void Webserv::run()
{
	initializeServers();
	
	std::vector<Server*>::iterator it = _servers.begin();
	Server *server;
	int fd;

	while (!globalSigint)
	{
		server = *it;
		fd = server->acceptConnections();
		server->addEvent(fd);
		server->addClient(fd);
		server->handleEvents();
		it = (++it == _servers.end()) ? _servers.begin() : it;
	}
}
