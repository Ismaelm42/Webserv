#pragma once
#include "./common.hpp"
#include "./Client.hpp"

class Socket
{
	private:
		int _epfd;
		int _bodySize;
		std::map<int, struct epoll_event> _configEvent;
		std::vector<struct epoll_event> _eventList;

	public:

		Socket();
		~Socket();

		void initConfigValues(int epfd, int bodySize);
		int getEventListSize();

		void configEvent(int fd);
		void addEvent(int fd);
		void deleteEvent(int fd);
		void checkEvents();
		void processEvents(std::map<int, Client> &client);
};
