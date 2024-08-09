#pragma once
#include "./common.hpp"
#include "./Client.hpp"

class Server
{
	private:
		int _socket;													// File descriptor del Server
		int _port;														// Puerto del servidor
		int _sockaddrlen;												// Tamaño de la estructura _sockaddr
		struct sockaddr_in _sockaddr;									// _sockaddr del Server
		struct Epoll_events *_events; 									// Puntero a estructura Epoll
		std::map<int, Client*> _clients;								// fd + cliente
	public:		
		Server(int port, struct Epoll_events *events);
		int acceptConnections();
		void addClient(int fd);
		void deleteClient(int fd);
		void recordEvent(int fd);
		void deleteEvent(int fd);
		void handleRequest(int fd);										
		void handleResponse(int fd);
		bool hasClientsToProcess();
		void handleEvents();
};
