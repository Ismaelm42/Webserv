#pragma once
#include "./common.hpp"
#include "./Server.hpp"

class Webserv
{
	private:
		int _bodySize;								// Tamaño max. de caracteres
		std::vector<int> _ports;					// Puertos
		std::vector<Server*> _servers;				// Servers			
		struct Epoll_events *_events;				// Estructura Epoll_events (common.hpp)

	public:
		Webserv();
		~Webserv();
		void initializeServers();					// Crea e inicializa los servers y la estructura Epoll
		void run();									// Run
};
