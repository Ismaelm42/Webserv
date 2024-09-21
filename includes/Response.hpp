#pragma once
#include "./common.hpp"
#include "./MimeType.hpp"

class Client;  // Declaración adelantada de Client
class Request;

/*
    Crear y alamacenar la respuesta. almacena el valor para ser enviado al cliente.
	con la funcion getRes() se obtiene la respuesta.
*/
class Response
{
    public:
		Response(Client *client, Server_config *config, Request *_request, struct Epoll_events *_events);
        //Response();														// constructor		
	//	Response(Client *c);												// constructor con cliente
		//Response(Request&);												// constructor con request	
      	Response(const Response& other);
		Response &operator=(const Response& other);
	  
	    ~Response();   		// destructor								
		bool 					isErrorCode();
		void 					buildResponse();
		int 					buildBody();
		int 					getTarget();
		int 					isValidTargetFile();
		int 					isNotValidMethod();
		int 					getCode() const;
		int 					setCode(int code);
		int 					setReturnCode(int code);
		void 					setClient(Client* client);
		void 					setStatusline();
		int 					getFile();
		std::string 			getResString();
		void    				setHeaders();											// setea los headers de la respuesta
		void 					reset();												// resetea la respuesta
		std::string 			_response_str;											// contenido de la respuesta en string
		Client*					_client;												// cliente
		Server_config* 			_config;												// request
		Request*				_request;
		struct Epoll_events 	*_events;
		int 					cgiFlag;

	private:
		std::string 		_responseString;										// respuesta
		int 				_code;													// codigo de respuesta
		int 				_auto_index_flag;										// flag para auto index
		std::string 		_target;												// target a devolver en la respuesta
		std::string 		_response_body_str;
        std::string 		mime;												// objeto de la clase Mime para determinar el tipo de resoponse a enviar
		Location_config 	*_location_config;
		std::string			getMatch(std::string path, std::vector<Location_config> locations);
		int 				setIndex();
		int 				launchCgi();
		std::string 		_location;											// location	to store the redirections


};
