#pragma once
#include "./common.hpp"

class Socket
{
	private:
		int _bodySize;
		std::map<int, struct epoll_event> _configEvent;
		std::vector<struct epoll_event> _eventList;
		std::map<int, std::string> _request;

	public:

		Socket();
		~Socket();

		int getEventListSize();

		void configEvent(int fd);
		void addEvent(int fd, int epfd);
		void deleteEvent(int fd, int epfd);
		void checkEvents(int epfd);

		void handleRead();
		void handleWrite();

};
