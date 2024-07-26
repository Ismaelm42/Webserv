#include "../includes/lib.hpp"

Socket::Socket()
{
	_epfd = epoll_create(1);
	if (_epfd < 0)
		throw std::runtime_error("Error: epoll_create: " + std::string(strerror(errno)));
}

void Socket::configEvent(int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN | EPOLLOUT;
	event.data.fd = fd;
	_configEvent.push_back(event);
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
			char buffer[1024] = {0};
			read(_eventList[i].data.fd, buffer, 1024);
			std::cout << "Received request:\n" << buffer << std::endl;

			_eventList.erase(_eventList.begin() + i);
		}
	}
}


void Socket::handleWrite()
{
	for(long unsigned i = 0; i < _eventList.size(); i++)
	{
		if (_eventList[i].events & EPOLLOUT)
		{
			std::string response = "Hello, world!\n";
			send(_eventList[i].data.fd, response.c_str(), response.size(), 0);
			_eventList.erase(_eventList.begin() + i);
		}
	}
}

Socket::~Socket()
{
	
}
