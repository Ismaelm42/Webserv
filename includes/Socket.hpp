#pragma once
#include "./common.hpp"

class Socket
{
	private:
		int _epfd;
	public:
		std::vector<struct epoll_event> _configEvent;
		std::vector<struct epoll_event> _eventList;
		std::string request;
		Socket();
		void configEvent(int fd);
		void addEvent(int fd);
		void checkEvents();
		void handleRead();
		void handleWrite();

		~Socket();
};
