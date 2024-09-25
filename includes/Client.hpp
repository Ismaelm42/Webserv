#pragma once
#include "./common.hpp"
#include "./Cgi.hpp"
#include "./Request.hpp"
#include "./Response.hpp"

class Cgi;

class Client
{
	private:
		Cgi *_cgi;								// Puntero a una clase CGI
		bool _isCgi;							// Flag que indica si el cliente debe procesar un cgi
		int _cgiFd[2];							// Fds del CGI
		Request *_request;						// Puntero a estructura request
		Response *_response;					// Puntero a estructura response
		Server_config *_config;					// Puntero a la estructura de config
		struct Epoll_events *_events;			// Puntero a estructura epoll

	public:
		int _fd;								// Fd del cliente
		int _port;								// Puerto
		bool _isReady;							// Flag que indica si el cliente ha recibido una request y está listo para responder
		std::string _host;						// Host

		Client(Server_config *config, struct Epoll_events *events, int fd, int port, std::string host);
		~Client();
		int getRequest();
		int sendResponse();
		void initCgi();
		void resetCgi();
};
