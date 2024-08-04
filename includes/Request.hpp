#pragma once
#include "./common.hpp"

/**
 * @brief Request class
 * viene de Client en un mapa para manejar las peticiones de los clientes como primer elemento in int (_data[fd]) 
 * el request como primer elemento del par del seguno elemento del mapa y el segundo ele mento del par sería el 
 * response asociado a la petición.
*/
class Request
{
	private:
		std::string _request;

	public:
		Request();
		~Request();

		std::string getRequest();   // getter
		void addRequest(int fd);    // setter
		void eraseRequest();        // "Destructor"

};
