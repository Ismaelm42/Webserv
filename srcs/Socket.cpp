#include "../includes/lib.hpp"

Socket::Socket()
{
	_epfd = epoll_create(1);
	_bodySize = 1024;
	if (_epfd < 0)
		throw std::runtime_error("Error: epoll_create: " + std::string(strerror(errno)));
}

int Socket::getEventListSize()
{
	return _eventList.size();
}

void Socket::configEvent(int fd)
{
	struct epoll_event _event;
	_event.events = EPOLLIN | EPOLLOUT;
	_event.data.fd = fd;
	_configEvent.push_back(_event);
	_eventList.resize(_eventList.size() + 1);
}

void Socket::addEvent(int fd)
{
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &_configEvent.back()) == -1)
		throw std::runtime_error("Error: epoll_ctl: " + std::string(strerror(errno)));
}

void Socket::checkEvents()
{
	if (epoll_wait(_epfd, _eventList.data(), _eventList.size(), -1) == -1)
		throw std::runtime_error("Error: epoll_wait: " + std::string(strerror(errno)));
}

void Socket::handleRead()
{
	for(long unsigned i = 0; i < _eventList.size(); i++)
	{
		if (_eventList[i].events & EPOLLIN)
		{
			char buffer[_bodySize];
			std::memset(buffer, 0, _bodySize);
			if (read(_eventList[i].data.fd, buffer, _bodySize) < 0)
				throw std::runtime_error("Error: read: " + std::string(strerror(errno)));
			std::string request(buffer);
			std::cout << "Received request:\n" << request << std::endl;
			_request.insert(std::make_pair(_eventList[i].data.fd, request));
		}
	}
}

void Socket::handleWrite()
{
	for(long unsigned i = 0; i < _eventList.size(); i++)
	{
		if (_eventList[i].events & EPOLLOUT && _request.find(_eventList[i].data.fd) != _request.end())
		{
			std::stringstream oss;
			oss << _eventList[i].data.fd;
			std::string response = "Hello, world! from " + oss.str() + "\n";
			if (send(_eventList[i].data.fd, response.c_str(), response.size(), 0) < 0)
				throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
			_request.erase(_eventList[i].data.fd);
		}
	}
}

Socket::~Socket()
{
	
}
