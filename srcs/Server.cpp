#include "../includes/lib.hpp"

Server::Server(std::string host, int port, struct Epoll_events *events, Server_config *config)
:_port(port), _host(host), _config(config), _events(events)
{
	int optval = 1;

    _socket = ::socket(AF_INET, SOCK_STREAM, 0);
   	if (_socket < 0)
        throw std::runtime_error("Error: socket: " + std::string(strerror(errno)));

    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
        throw std::runtime_error("Error: setsockopt: " + std::string(strerror(errno)));
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) != 0)
        throw std::runtime_error("Error: setsockopt: " + std::string(strerror(errno)));
    if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Error: fcntl: " + std::string(strerror(errno)));
	
    _sockaddr.sin_addr.s_addr = inet_addr(_host.c_str());
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(_port);

    if (bind(_socket, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) < 0)
        throw std::runtime_error("Error: bind: " + std::string(strerror(errno)));

    if (listen(_socket, 50) < 0)
        throw std::runtime_error("Error: listen: " + std::string(strerror(errno)));
    std::cout << Log << Green << "Listenning on " << _host << ":" << _port << Reset << std::endl;
}

Server::~Server()
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		delete it->second;
	_clients.clear();
}

int Server::acceptConnections()
{
	int _sockaddrlen = sizeof(_sockaddr);
	int client = accept(_socket, (struct sockaddr *)&_sockaddr, (socklen_t *)&_sockaddrlen);
	if (client < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
		throw std::runtime_error("Error: accept: " + std::string(strerror(errno)));
    if (client > 0 && fcntl(client, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error: fcntl: " + std::string(strerror(errno)));
	return client;
}

void Server::addEvent(int fd)
{
	if (fd > 0)
	{
		struct epoll_event newEvent;
		newEvent.events = EPOLLIN | EPOLLOUT;
		newEvent.data.fd = fd;
		_events->added[fd] = newEvent;
		if (epoll_ctl(_events->epfd, EPOLL_CTL_ADD, fd, &_events->added[fd]) == -1)
			throw std::runtime_error("Error: epoll_ctl_add: " + std::string(strerror(errno)));
	}
}

void Server::deleteEvent(int fd)
{
	if (epoll_ctl(_events->epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw std::runtime_error("Error: epoll_ctl_delete: " + std::string(strerror(errno)));
    _events->added.erase(fd);
}

void Server::addClient(int fd)
{
	if (fd > 0 && _clients.find(fd) == _clients.end())
	{
		std::cout << Log << Green << "Client associated with socket " << fd << " has been created" << Reset << std::endl;
    	Client *client = new Client(_config, fd, _port, _host);
    	_clients[fd] = client;
	}
}

void Server::deleteClient(int fd)
{
	delete _clients[fd];
	_clients.erase(fd);
	close(fd);
	std::cout << Log << Red << "Client associated with socket " << fd << " has been removed." << Reset << std::endl;   
}

void Server::handleRequest(int fd)
{
	if (_clients.find(fd) != _clients.end())
	{
		if (!_clients[fd]->_isReady && _clients[fd]->getRequest() < 0)
		{
			deleteEvent(fd);
			deleteClient(fd);
		}
	}
}

void Server::handleResponse(int fd)
{
	if (_clients.find(fd) != _clients.end())
	{
		if (_clients[fd]->_isReady && _clients[fd]->sendResponse() < 0)
		{
			deleteEvent(fd);
			deleteClient(fd);
		}
	}
}

void Server::handleEvents()
{
	if (_events->added.empty())
		return;
	if (_events->added.size() > _events->log.size())
		_events->log.resize(_events->log.size() * 2);
	_numEvents = epoll_wait(_events->epfd, _events->log.data(), _events->log.size(), -1);
    if (_numEvents == -1)
		throw std::runtime_error("Error: epoll_wait: " + std::string(strerror(errno)));
	for (int i = 0; i < _numEvents; i++)
	{
		if (_events->log[i].events & EPOLLERR || _events->log[i].events & EPOLLHUP)
		{
			std::cout << Log << Red << "Connection unexpectedly reset by peer" << Reset << std::endl;
			deleteEvent(_events->log[i].data.fd);
			deleteClient(_events->log[i].data.fd);
		}
		else
		{
			if (_events->log[i].events & EPOLLIN)
				handleRequest(_events->log[i].data.fd);
			if (_events->log[i].events & EPOLLOUT)
				handleResponse(_events->log[i].data.fd);
		}
	}
}
