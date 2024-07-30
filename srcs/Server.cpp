#include "../includes/lib.hpp"

Server::Server(int port)
{
    this->_port = port;
    _serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
   	if (_serverFd < 0)
        throw std::runtime_error("Error: socket: " + std::string(strerror(errno)));
}

void Server::configServer()
{
    int optval = 1;

    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
        throw std::runtime_error("Error: setsockopt: " + std::string(strerror(errno)));
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) != 0)
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
    Socket socket;
    int addressLen = sizeof(_address);
    int fd;

    while (true)
	{
	    fd = accept(_serverFd, (struct sockaddr *)&_address, (socklen_t *)&addressLen);
        if (fd < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
            throw std::runtime_error("Error: accept: " + std::string(strerror(errno)));
        if (fd >= 0)
        {
            socket.configEvent(fd);
            socket.addEvent(fd);
        }
        if (socket.getEventListSize() > 0)
        {
            socket.checkEvents();
            socket.handleRead();
            socket.handleWrite();
        }
    }
}

Server::~Server()
{
    close(_serverFd);
}
