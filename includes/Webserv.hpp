#pragma once
#include "./common.hpp"
#include "./Configuration.hpp"
#include "./Server.hpp"

class Webserv
{
	private:
		Configuration *_config;
		std::vector<Server*> _servers;
		struct Epoll_events *_events;

	public:
		Webserv(int argc, char **argv);
		~Webserv();
		void initializeServers();
		void run();
};
