#pragma once
#include "./common.hpp"
#include "./Request.hpp"
#include "./Response.hpp"

class Client
{
	private:
		int _fd;
		Request _request;
		Response _response;

	public:
		Client();							//Necesario para la primera instancia al crear Server.
		Client(int fd);						//Constructor que instancia el file descriptor.
		~Client();
		void handleRequest();				//Función que delega las peticiones a la clase Request.
		void handleResponse();				//Función que delega las respuestas a la clase Response.
};
