#include "../includes/lib.hpp"

Client::Client(std::string ip, int port, int fd, Server_config *config, struct Epoll_events *events)
:_fd(fd), _port(port), _status(0), _ip(ip), _config(config), _events(events)
{
	_request = new Request(this, _config);
	_response = new Response(this, _request, _config);
}

Client::~Client()
{
		delete _request;
	// 	delete _response;
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
	// std::cout << Purple <																																															< "Raw Request" << Reset << std::endl << buffer << std::endl;
	_request->fillRequest(buffer, bytesRead);
	_response->buildResponse();
	// _request.printParsed();
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
	// if (_request->getPath() == "/cgi/sum.sh")
	// {
	// 	Cgi cgi(_request);
	// 	cgi.executeCgi();
	// 	_request->reset();
	// 	_status = 0;
	// }
	// else
	// {
		int bytesSent;
		// std::stringstream responseStream;



		// responseStream << "HTTP/1.1 200 OK\r\n";
		// responseStream << "Content-Type: text/plain\r\n";
		// responseStream << "Content-Length: " << 29 << "\r\n";
		// responseStream << "\r\n";
		// responseStream << "Hello, world! from socket " << _fd << "\r\n";

		// std::string response = responseStream.str();
		_request->reset();
		std::string res=_response->getResString(); 
		bytesSent = send(_fd, res.c_str(), res.size(), 0);
		if (bytesSent < 0)
			throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
		_response->reset();
		_status = 0;
	// }
	return 0;
}
