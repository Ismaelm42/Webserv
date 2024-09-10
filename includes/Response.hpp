#pragma once
#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include "./common.hpp"
// #include "./Client.hpp" // no reconoce el tipo Client

class Client;  // Declaración adelantada de Client
class Request;

//class Client; // forward declaration si lo inlcuyo lo reconoce pero no funciona error clase incompleta

/*
    Crear y alamacenar la respuesta. almacena el valor para ser enviado al cliente.
	con la funcion getRes() se obtiene la respuesta.
*/
class Response
{
    public:
		Response(Client *client, Request *_request, Server_config *config);
        //Response();														// constructor		
	//	Response(Client *c);												// constructor con cliente
		//Response(Request&);												// constructor con request	
      	Response(const Response& other);
		Response &operator=(const Response& other);
	  
	    ~Response();   		// destructor								
		bool 			isErrorCode();
		void 			buildResponse();
		int 			getCode() const;
		void 			setCode(int code);
		void 			setClient(Client* client);
		void 			setStatusline();
		int 			getFile();
		std::string 	getResString();
		void    		setHeaders();											// setea los headers de la respuesta
		void 			reset();												// resetea la respuesta
		std::string 	_response_str;											// contenido de la respuesta en string
		Client*			_client;												// cliente
		Request*		_request;
		Server_config* 	_config;												// request

	private:
		std::string 	_responseString;										// respuesta
		int 			_code;													// codigo de respuesta
		std::string 	_target;												// target a devolver en la respuesta
		std::string 	_response_body_str;
};

#endif // RESPONSE_HPP