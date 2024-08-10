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
		Server(int port, struct Epoll_events *events);					// Se crea y configura el socket del server
		~Server();
		int acceptConnections();										// Acepta las conexiones
		void addClient(int fd);											// Añade un nuevo cliente
		void deleteClient(int fd);										// Elimina un cliente
		void recordEvent(int fd);										// Añade un nuevo evento a la estructura Epoll (_events->added)
		void deleteEvent(int fd);										// Elimina un evento de la estructura Epoll (_events->added / _events_log)
		void handleRequest(int fd);										// Gestiona la request del cliente				
		void handleResponse(int fd);									// Gestiona la response del cliente
		bool hasClientsToProcess();										// Verifica si hay clientes en el container _clients
		void handleEvents();											// Procesa los eventos ocurridos y llama a las funciones de lectura o respuesta
};
