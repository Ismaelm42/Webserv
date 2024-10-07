#pragma once

#ifndef SERVER_HPP
#define SERVER_HPP

#include "./common.hpp"
#include "./Client.hpp"
#include "./Request.hpp"

class Server
{
	private:
		int _port;														// Puerto del servidor
		int _socket;													// File descriptor del Server
		int _numEvents;													// Número de eventos ocurridos después de la llamada epoll_wait
		std::string _host;												// Host
		Server_config *_config;											// Puntero a la estructura de configuración del server
		struct sockaddr_in _sockaddr;									// _sockaddr del Server
		struct Epoll_events *_events; 									// Puntero a estructura Epoll
		std::map<int, Client*> _clients;								// fd + cliente
	public:		
		Server(std::string host, int port, struct Epoll_events *events, Server_config *config);		// Se crea y configura el socket del server
		~Server();
		int acceptConnections();														// Acepta las conexiones
		void addEvent(int fd);
		void deleteEvent(int fd);
		void addClient(int fd);															// Añade un nuevo evento a la estructura Epoll (_events->added) y el clliente asociado a ese fd
		void deleteClient(int fd);														// Elimina un evento de la estructura Epoll (_events->added / _events_log) y el cliente asociado a ese fd
		void handleRequest(int fd);														// Gestiona la request del cliente				
		void handleResponse(int fd);													// Gestiona la response del cliente
		void handleEvents();															// Procesa los eventos ocurridos y llama a las funciones de lectura o respuesta
};

#endif // SERVER_HPP
