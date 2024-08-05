#include "../includes/lib.hpp"

Socket::Socket()
{
}

void Socket::initConfigValues(int epfd, int bodySize)
{
	_epfd = epfd;
	_bodySize = bodySize;
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

void Socket::addEvent(int fd)
{
	if (epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &_configEvent[fd]) == -1)
		throw std::runtime_error("Error: epoll_ctl_add: " + std::string(strerror(errno)));
}

void Socket::deleteEvent(int fd)
{
	if (epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
        throw std::runtime_error("Error: epoll_ctl_delete: " + std::string(strerror(errno)));
	_configEvent.erase(fd);
	close(fd);
}

void Socket::checkEvents()
{
	_eventList.resize(_configEvent.size() + 1);
	if (epoll_wait(_epfd, _eventList.data(), _eventList.size(), -1) == -1)
	{
		throw std::runtime_error("Error: epoll_wait: " + std::string(strerror(errno)));

	}
}

/*
Función que procesa los eventos. Se utiliza un try catch para procesar los errores en caso de lectura y
poder eliminar el evento de la estructura socket.
*/

void Socket::processEvents(std::map<int, Client> &client)
{
	for (long unsigned i = 0; i < _eventList.size(); i++)
	{
		try
		{
			if (_eventList[i].events & EPOLLERR || _eventList[i].events & EPOLLHUP)
				throw std::runtime_error("Error: Epoller | Epollhup: " + std::string(strerror(errno)));
			if (_eventList[i].events & EPOLLIN)
				client[_eventList[i].data.fd].handleRequest();
			if (_eventList[i].events & EPOLLOUT)
				client[_eventList[i].data.fd].handleResponse();
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
			int fd = _eventList[i].data.fd;
			deleteEvent(fd);
		}
	}
}

Socket::~Socket()
{
	
}
