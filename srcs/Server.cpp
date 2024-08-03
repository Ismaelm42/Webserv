#include "../includes/lib.hpp"

Server::Server(int port)
{

    _port = port;
    _optval = 1;
    _addressLen = sizeof(_address);
    _serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
   	if (_serverFd < 0)
        throw std::runtime_error("Error: socket: " + std::string(strerror(errno)));
	_epfd = epoll_create(1);
	if (_epfd < 0)
		throw std::runtime_error("Error: epoll_create: " + std::string(strerror(errno)));
}

Server::~Server()
{
    close(_serverFd);
}

void Server::configServer()
{
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &_optval, sizeof(_optval)) != 0)
        throw std::runtime_error("Error: setsockopt: " + std::string(strerror(errno)));
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEPORT, &_optval, sizeof(_optval)) != 0)
        throw std::runtime_error("Error: setsockopt: " + std::string(strerror(errno)));
    if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Error: fcntl: " + std::string(strerror(errno)));
	
    _address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
}

void Server::bindServer()
{
    if (bind(_serverFd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
        throw std::runtime_error("Error: bind: " + std::string(strerror(errno)));
}

void Server::listenning()
{
    if (listen(_serverFd, 1) < 0)
        throw std::runtime_error("Error: listen: " + std::string(strerror(errno)));
    std::cout << "Server started listenning on port " << _port << std::endl;
}

void Server::acceptConnections()
{
    while (true)
	{
	    _socketFd = accept(_serverFd, (struct sockaddr *)&_address, (socklen_t *)&_addressLen);
        if (_socketFd < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
            throw std::runtime_error("Error: accept: " + std::string(strerror(errno)));
        if (_socketFd >= 0)
        {
            std::cout << Red << "Connection accepted with socket fd " << _socketFd << Reset << std::endl;
            _client.addClient(_socketFd);
            _socket.configEvent(_socketFd);
            _socket.addEvent(_socketFd, _epfd);
        }
        if (_client.clientCounter())
        {
            _socket.checkEvents(_epfd);
            _socket.processEvents(_client);
        }
    }
}

