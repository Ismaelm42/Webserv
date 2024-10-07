#pragma once
#include "./common.hpp"
#include "./Client.hpp"
#include "./Request.hpp"

class Server
{
	private:
		int _port;
		int _socket;
		int _numEvents;
		std::string _host;
		Server_config *_config;
		struct sockaddr_in _sockaddr;
		struct Epoll_events *_events;
		std::map<int, Client*> _clients;
	public:		
		Server(std::string host, int port, struct Epoll_events *events, Server_config *config);
		~Server();
		int acceptConnections();
		void addEvent(int fd);
		void deleteEvent(int fd);
		void addClient(int fd);
		void deleteClient(int fd);
		void handleRequest(int fd);
		void handleResponse(int fd);
		void handleEvents();
};
