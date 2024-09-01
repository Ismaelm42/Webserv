#include "../includes/lib.hpp"

Client::Client(std::string ip, int port, int fd, Server_config *config)
:_fd(fd), _port(port), _status(0), _ip(ip), _config(config)
{
	
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
	_request = buffer;
	std::cout << Cyan << "Message received from fd " << _fd << "\tadress " << _ip << ":" << _port << Reset << std::endl;
	_status = 1;
	return 0;
}

/*
	Gestiona la response.
	En desarrollo.
*/
int Client::sendResponse()
{
	int bytesSent;
	std::stringstream responseStream;

	responseStream << "HTTP/1.1 200 OK\r\n";
	responseStream << "Content-Type: text/plain\r\n";
	responseStream << "Content-Length: " << 29 << "\r\n";
	responseStream << "\r\n";
	responseStream << "Hello, world! from socket " << _fd << "\r\n";

	std::string response = responseStream.str();
	bytesSent = send(_fd, response.c_str(), response.size(), 0);
	if (bytesSent < 0)
		throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
	_status = 0;
	request.reset();			// Reseteamos la request para la siguiente iteración
	return 0;
}
