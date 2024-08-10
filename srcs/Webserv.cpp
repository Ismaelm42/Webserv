#include "../includes/lib.hpp"


Webserv::Webserv()
{
	_events = new Epoll_events;
	_events->epfd = epoll_create(1);
	if (_events->epfd < 0)
		throw std::runtime_error("Error: epoll_create: " + std::string(strerror(errno)));
	_ports.push_back(8080);
	_ports.push_back(4040);
	_ports.push_back(9090);
}

Webserv::~Webserv()
{
	delete _events;
	for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		delete *it;
}

void Webserv::initializeServers()
{
	for(unsigned i = 0; i < _ports.size(); i++)
	{
		Server *server = new Server(_ports[i], _events);
		_servers.push_back(server);
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
		if (fd >= 0)
		{
			server->addClient(fd);
			server->recordEvent(fd);
		}
		if (server->hasClientsToProcess())
			server->handleEvents();
		it = (++it == _servers.end()) ? _servers.begin() : it;
	}
}
