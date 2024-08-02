#include "../includes/lib.hpp"

Socket::Socket()
{
	_bodySize = 1024;
}

int Socket::getEventListSize()
{
	return _eventList.size();
}

void Socket::configEvent(int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = fd;
	_configEvent[fd] = event;
	_eventList.resize(100); //asignar en checkEvents. Solucionar este problema.
}

void Socket::addEvent(int fd, int epfd)
{
	if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &_configEvent[fd]) == -1)
		throw std::runtime_error("Error: epoll_ctl_add: " + std::string(strerror(errno)));
}

void Socket::deleteEvent(int fd, int epfd)
{
	if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("Error: epoll_ctl_delete: " + std::string(strerror(errno)));
	_configEvent.erase(fd);
}

void Socket::checkEvents(int epfd)
{
	// _eventList.resize(_configEvent.size()); // El tamaño debería ser uno base y si se supera adaptarse de forma dinámica.
	if (epoll_wait(epfd, _eventList.data(), _eventList.size(), -1) == -1)
		throw std::runtime_error("Error: epoll_wait: " + std::string(strerror(errno)));
}

void Socket::handleRead()
{
	for(long unsigned i = 0; i < _eventList.size(); i++)
	{
		if (_eventList[i].events & EPOLLIN)
		{
			std::string message;
			char buffer[_bodySize];
			int bytesRead;
			int fd;

			fd = _eventList[i].data.fd;
			std::memset(buffer, 0, _bodySize);
			bytesRead = read(fd, buffer, _bodySize);
			if (bytesRead == 0 || bytesRead < 0)
			{
				close(fd);
				throw std::runtime_error("Error: read: " + std::string(strerror(errno)));
			}
			message = buffer;
			std::cout << "Received message:\n" << message << std::endl;
			std::cout << fd << std::endl;
			_request.insert(std::make_pair(fd, message));
		}
	}
}

void Socket::handleWrite()
{
	for (long unsigned i = 0; i < _eventList.size(); i++)
	{
		if (_eventList[i].events & EPOLLOUT && _request.find(_eventList[i].data.fd) != _request.end())
		{
			int bytesSent;
			std::stringstream responseStream;

			responseStream << "HTTP/1.1 200 OK\r\n";
            responseStream << "Content-Type: text/plain\r\n";
			responseStream << "Content-Length: " << 29 << "\r\n";
            responseStream << "\r\n";
			responseStream << "Hello, world! from socket " << _eventList[i].data.fd << "\r\n";
            std::string response = responseStream.str();

			bytesSent = send(_eventList[i].data.fd, response.c_str(), response.size(), 0);
			if (bytesSent < 0)
				throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
			if (static_cast<size_t>(bytesSent) < response.size())
			{
				//Si no se ha mandado todos los bytes de Response, debería entonces truncar response para quedarnos con los bytes que quedan que mandarle para el próximo loop.
				//El evento no debe por lo tanto eliminarse.
				continue;
			}
			_request.erase(_eventList[i].data.fd);
		}
	}
}

Socket::~Socket()
{
	
}
