#pragma once
#include "./common.hpp"
#include "./Request.hpp"
#include "./Cgi.hpp"

class Client
{
	public:	//public en vez de private
		int _fd;								// Fd del cliente
		int _port;
		int _status;							// Flag que indica si el cliente ha recibido una request
		Request _request;
		std::string _ip;
		std::string _response;					// Response
		Server_config *_config;					// Puntero a la estructura de configuración
	public:
		Client(std::string _ip, int port, int fd, Server_config *config);
		~Client();
		int getStatus();
		int getRequest();
		int sendResponse();
};


