#pragma once
#include "./common.hpp"
#include "./Configuration.hpp"
#include "./Server.hpp"
// #include "./Request.hpp"

#define URI_MAX_LENGTH 8192		// nginx default 8k puede cambiarse con las directivas: http { client_header_buffer_size 16k; large_client_header_buffers 4 16k;}

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
