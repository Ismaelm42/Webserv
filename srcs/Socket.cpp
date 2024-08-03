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
	_eventList.resize(_configEvent.size());
	if (epoll_wait(epfd, _eventList.data(), _eventList.size(), -1) == -1)
		throw std::runtime_error("Error: epoll_wait: " + std::string(strerror(errno)));
}

void Socket::processEvents(Client &client)
{
	for (long unsigned i = 0; i < _eventList.size(); i++)
	{
		if (_eventList[i].events & EPOLLIN)
			client.addRequest(_eventList[i].data.fd);
		if (_eventList[i].events & EPOLLOUT)
			client.sendResponse(_eventList[i].data.fd);
	}
}

Socket::~Socket()
{
	
}
