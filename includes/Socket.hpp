#pragma once
#include "./common.hpp"

class Socket
{
	private:
		int _epfd;
		int _bodySize;
		std::vector<struct epoll_event> _configEvent;
		std::vector<struct epoll_event> _eventList;
		std::map<int, std::string> _request;

	public:

		Socket();
		~Socket();

		int getEventListSize();

		void configEvent(int fd);
		void addEvent(int fd);
		void checkEvents();
		void handleRead();
		void handleWrite();

};
