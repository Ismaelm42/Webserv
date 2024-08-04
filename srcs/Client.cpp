#include "../includes/lib.hpp"

Client::Client()
{
 //std::cerr << "Client constructor" << std::endl;
}

Client::~Client()
{

}

/**
 * @brief Add client to the map
 * 
 * @param fd file descriptor
 * instancia un request para el primer elemento del par dentro del mapa cuyo 
 * primer elemento es _data[fd] y un response para el segundo asociando cada 
 * uno de ellos con el file descriptor.	
 */

void Client::addClient(int fd)
{
	_data[fd].first = Request();
	_data[fd].second = Response();
}

/**
 * @brief borra el cliente del mapa.
 * 
 * @param fd file descriptor
 * 
 */

void Client::deleteClient(int fd)
{
	_data.erase(fd);
}

/**
 * @brief añade una petición al cliente.
 * Llamando a la porpia función addRequest de la clase Request.
 * @param fd file descriptor
 *
 */
void Client::addRequest(int fd)
{
	_data[fd].first.addRequest(fd);
}

/*
 * @brief envía la respuesta al cliente asociado a fd conviritendola antes en un string.	
 *	ver si hay que incluir la codificación en este punto o en la propia clase response.
 * @param fd file descriptor
 * Antes de enviar la respuesta comprobamos si la petición no está vacía y si no lo está 
 * añadimos la respuesta al cliente y borramos la petición (tanto request como response).
 * al cliente
 */
void Client::sendResponse(int fd)
{
	std::string request = _data[fd].first.getRequest();

	if (!request.empty())
	{
		_data[fd].second.addResponse(fd, request);
		_data[fd].first.eraseRequest();
		_data[fd].second.eraseResponse();
	}
}

/**
 * @brief cuenta la cantidad de clientes.
 * 
 * @return int cantidad de clientes (fds en el mapa)
 */
int Client::clientCounter()
{
	return _data.size();
}
