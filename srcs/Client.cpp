#include "../includes/lib.hpp"
#include "../includes/Client.hpp"

Client::Client(std::string ip, int port, int fd, Server_config *config)
:_fd(fd), _port(port), _status(0), _ip(ip), _config(config)
{
	Request request;
	Response response;	
}

Client::~Client()
{
}

int Client::getStatus()
{
	return _status;
}
/*
	Gestiona la request y si la lectura del socket no es posible cierra la conexión con el socket.
	En desarrollo.
*/
int Client::getRequest()
{
	char buffer[10000];
	int bytesRead;

	std::memset(buffer, 0, 10000);
	bytesRead = read(_fd, buffer, 10000);
	if (bytesRead == 0 || bytesRead < 0)
	{
		std::cout << Red << "Connection closed on fd " << _fd << Reset << std::endl;
		return -1;
	}
	request.fillRequest(buffer, bytesRead);
	if (DEBUG)
		std::cout << "Request_errorCode: " << request.getErrorCode() << std::endl;
	_request = buffer;
	std::cout << Cyan << "Message received from fd " << _fd << "\taddress " << _ip << ":" << _port << Reset << std::endl;
	_status = 1;
	response.setClient(this);
	response.buildResponse();
	return 0;
}
	
void Client::setInResponse(Response* response) {
    response->setClient(this);  // Pasar el puntero 'this' al objeto Response
}

void Client::setInRequest(Request* request) {
    request->setClient(this);  // Pasar el puntero 'this' al objeto Response
}

/*
	Gestiona la response.
	En desarrollo.
*/
int Client::sendResponse()
{
	std::string res = response.getResString();	// Obtenemos la respuesta en formato std:string
	// std::cout << "Response: " << res << std::endl;
	int bytesSent;
	/*
	std::stringstream responseStream;
	responseStream <<  res << _fd << "\r\n";
	std::string response = responseStream.str();
	bytesSent = send(_fd, response.c_str(), response.size(), 0);*/
	bytesSent = send(_fd, res.c_str(), res.size(), 0);		// Enviamos la respuesta al cliente en formato c-string


	if (bytesSent < 0)
		throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
	_status = 0;
	request.reset();			// Reseteamos la request para la siguiente iteración
	return 0;
}
