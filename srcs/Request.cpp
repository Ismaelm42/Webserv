#include "../includes/lib.hpp"

Request::Request()
{

}

Request::~Request()
{

}

std::string Request::getRequest()
{
	return _request;
}

/**
 * @brief añade una petición al cliente.
 * 
 * @param fd file descriptor
 * 
 * Reserva un buffer de 10000 bytes y lee el mensaje del cliente asociado a fd.
 * Si el mensaje es 0 o menor que 0 cierra el file descriptor y lanza una excepción.
 * Si no, añade el mensaje a la petición y lo muestra por pantalla.
 *  ////                    Es necesario añadir verificaciones de errores en la lectura, incluir estructura 
 *  ////                    para los headers y el cuerpo de la petición.
 * 
 *  ////                   ver si el buffer se puede hacer de forma que dependa del tamaño del mensaje.
 *  ////                   en el archivo de confirguración se debe determinar el tamaño del body
 */

void Request::addRequest(int fd)
{
	char buffer[10000];
	int bytesRead;

	std::memset(buffer, 0, 10000);
	bytesRead = read(fd, buffer, 10000);
	std::cout << High_Green << "READ FD = " << fd << Reset << std::endl;
	if (bytesRead == 0 || bytesRead < 0)
		throw std::runtime_error("Error: read: " + std::string(strerror(errno)));
	_request = buffer;
	std::cout << "Received message:\n" << _request << std::endl;
	std::cout << fd << std::endl;
}

/**
 * @brief "Destructor de request"
 * 
 * Borra la petición, ojo no la respuesta
 * 
 */
void Request::eraseRequest()
{
	_request.clear();
}
