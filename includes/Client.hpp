#pragma once
#include "./common.hpp"
#include "./Request.hpp"
#include "./Response.hpp"
#include "./Cgi.hpp"

class Request;

class Client
{
	public:	//public en vez de private
		int _fd;								// Fd del cliente
		int _port;								// Puerto
		int _status;							// Flag que indica si el cliente ha recibido una request
		std::string _ip;						// Host
		Request *_request;						// Puntero a estructura request
		Response *_response;					// Puntero a estructura response
		Server_config *_config;					// Puntero a la estructura de configuración
		struct Epoll_events *_events;			// Puntero a estructura epoll
		
	public:
		Client(std::string _ip, int port, int fd, Server_config *config, struct Epoll_events *events);
		~Client();
		int getStatus();
		int getRequest();
		int sendResponse();
};

