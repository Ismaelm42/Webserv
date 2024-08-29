#pragma once
#include "./common.hpp"
#include "./Configuration.hpp"
#include "./Server.hpp"

class Webserv
{
	private:
		Configuration *_config;						// Clase Configuration
		std::vector<Server*> _servers;				// Servers			
		struct Epoll_events *_events;				// Estructura Epoll_events (common.hpp)

	public:
		Webserv(int argc, char **argv);
		~Webserv();
		void initializeServers();					// Crea e inicializa los servers y la estructura Epoll
		void run();									// Run
};
