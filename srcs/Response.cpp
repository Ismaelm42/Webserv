#include "../includes/lib.hpp"

Response::Response()
{

}

Response::~Response()
{

}


void Response::addResponse(int fd, std::string request)
{
	int bytesSent;
	std::stringstream responseStream;
	request = request + request; //línea para quitarme problemas en compilación

	responseStream << "HTTP/1.1 200 OK\r\n";
	responseStream << "Content-Type: text/plain\r\n";
	responseStream << "Content-Length: " << 29 << "\r\n";
	responseStream << "\r\n";
	responseStream << "Hello, world! from socket " << fd << "\r\n";

	std::string response = responseStream.str();
	bytesSent = send(fd, response.c_str(), response.size(), 0);
	if (bytesSent < 0)
		throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
	// if (static_cast<size_t>(bytesSent) < response.size())
	// {
	// 	Si no se ha mandado todos los bytes de Response, debería entonces truncar response para quedarnos con los bytes que quedan que mandarle para el próximo loop.
	// 	El evento no debe por lo tanto eliminarse.
	// }
}

void Response::eraseResponse()
{

}
