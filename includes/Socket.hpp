#pragma once
#include "./common.hpp"
#include "./Client.hpp"

class Socket
{
	private:
		int _bodySize;
		std::map<int, struct epoll_event> _configEvent;
		std::vector<struct epoll_event> _eventList;

	public:

		Socket();
		~Socket();

		int getEventListSize();

		void configEvent(int fd);
		void addEvent(int fd, int epfd);
		void deleteEvent(int fd, int epfd);
		void checkEvents(int epfd);
		void processEvents(Client &client);
};
