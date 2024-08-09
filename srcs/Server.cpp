#include "../includes/lib.hpp"

Server::Server(int port, struct Epoll_events *events)
:_port(port), _events(events)
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
	
    _sockaddr.sin_addr.s_addr = INADDR_ANY;
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(_port);

    if (bind(_socket, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) < 0)
        throw std::runtime_error("Error: bind: " + std::string(strerror(errno)));

    if (listen(_socket, 1) < 0)
        throw std::runtime_error("Error: listen: " + std::string(strerror(errno)));
    std::cout << Yellow << "Server started listenning on port " << _port << Reset << std::endl;
}

int Server::acceptConnections()
{
	int client;

	_sockaddrlen = sizeof(_sockaddr);
	client = accept(_socket, (struct sockaddr *)&_sockaddr, (socklen_t *)&_sockaddrlen);
	if (client < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
		throw std::runtime_error("Error: accept: " + std::string(strerror(errno)));
	return client;
}

void Server::addClient(int fd)
{
    Client *client = new Client(fd);
    _clients[fd] = client;
}

void Server::deleteClient(int fd)
{
    _clients.erase(fd);
}

void Server::recordEvent(int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = fd;
	_events->added[fd] = event;
	if (epoll_ctl(_events->epfd, EPOLL_CTL_ADD, fd, &_events->added[fd]) == -1)
		throw std::runtime_error("Error: epoll_ctl_add: " + std::string(strerror(errno)));
}

void Server::deleteEvent(int fd)
{
	if (epoll_ctl(_events->epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw std::runtime_error("Error: epoll_ctl_delete: " + std::string(strerror(errno)));
    _events->added.erase(fd);
    std::vector<struct epoll_event>::iterator it;
    for (it = _events->log.begin(); it != _events->log.end(); it++)
    {
        if (it->data.fd == fd)
        {
            it = _events->log.erase(it);
            break ;
        }
    }
	close(fd);    
}

void Server::handleRequest(int fd)
{
	if (_clients.find(fd) != _clients.end())
	{
		if (_clients[fd]->getRequest() < 0)
		{
			deleteEvent(fd);
			deleteClient(fd);
		}
	}
}
void Server::handleResponse(int fd)
{
	if (_clients.find(fd) != _clients.end() && _clients[fd]->getStatus())
		_clients[fd]->sendResponse();
}

bool Server::hasClientsToProcess()
{
	if (_clients.empty())
		return false;
	return true;
}

void Server::handleEvents()
{
	_events->log.resize(_events->added.size());
    if (epoll_wait(_events->epfd, _events->log.data(), _events->log.size(), -1) == -1)
		throw std::runtime_error("Error: epoll_wait: " + std::string(strerror(errno)));
	for (long unsigned i = 0; i < _events->log.size(); i++)
	{
		if (_events->log[i].events & EPOLLERR || _events->log[i].events & EPOLLHUP)
		{
			std::cout << "PORT = " << _port << std::endl;
			std::cout << "FD = " << _events->log[i].data.fd << std::endl;
			std::cout << "FDs EN _EVENTS_LOG:" << std::endl;
			for (std::vector<struct epoll_event>::iterator it = _events->log.begin(); it != _events->log.end(); it++)
				std::cout << it->data.fd << std::endl;
		
			std::cout << "FDs EN _EVENTS_ADDED:" << std::endl;
			for (std::map<int, struct epoll_event>::iterator it = _events->added.begin(); it != _events->added.end(); it++)
				std::cout << it->second.data.fd << std::endl;
			throw std::runtime_error("Error: Epoller | Epollhup: " + std::string(strerror(errno)));
		}
		if (_events->log[i].events & EPOLLIN)
			handleRequest(_events->log[i].data.fd);
		if (_events->log[i].events & EPOLLOUT)
			handleResponse(_events->log[i].data.fd);
	}
}
