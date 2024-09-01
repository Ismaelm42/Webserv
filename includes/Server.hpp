#pragma once
#include "./common.hpp"
#include "./Client.hpp"
#include "./Request.hpp"

class Server
{
	private:
		int _socket;													// File descriptor del Server
		int _port;														// Puerto del servidor
		std::string _ip;												// IP
		Server_config *_config;											// Puntero a la estructura de configuración del server
		struct sockaddr_in _sockaddr;									// _sockaddr del Server
		struct Epoll_events *_events; 									// Puntero a estructura Epoll
		std::map<int, Client*> _clients;								// fd + cliente
	public:		
		Server(std::string ip, int port, struct Epoll_events *events, Server_config *config);	// Se crea y configura el socket del server
		~Server();
		int acceptConnections();																// Acepta las conexiones
		void addEventandClient(int fd);															// Añade un nuevo evento a la estructura Epoll (_events->added) y el clliente asociado a ese fd
		void deleteEventandClient(int fd);														// Elimina un evento de la estructura Epoll (_events->added / _events_log) y el cliente asociado a ese fd
		void handleRequest(int fd);																// Gestiona la request del cliente				
		void handleResponse(int fd);															// Gestiona la response del cliente
		void handleEvents();																	// Procesa los eventos ocurridos y llama a las funciones de lectura o respuesta
};
