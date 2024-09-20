#include "../includes/lib.hpp"

Client::Client(Server_config *config, struct Epoll_events *events, int fd, int port, std::string host)
:_config(config), _events(events), _fd(fd), _port(port), _status(false), _host(host)
{
	_request = new Request(this, _config, _events);
	_response = new Response(this, _config, _request, _events);
	_cgi = NULL;
}

Client::~Client()
{
	delete _request;
	delete _response;
	if (_cgi != NULL)
		delete _cgi;
}

/*
	Gestiona la request y si la lectura del socket no es posible cierra la conexión con el socket.
	En desarrollo.
*/
int Client::getRequest()
{
	char buffer[10000];
	std::memset(buffer, 0, 10000);
	int bytesRead = read(_fd, buffer, 10000);
	if (bytesRead == 0 || bytesRead < 0)
	{
		std::cout << Red << "Connection closed on fd " << _fd << Reset << std::endl;
		return -1;
	}
	_request->fillRequest(buffer, bytesRead);
	_response->buildResponse();
	// _request->printParsed();
	std::cout << Cyan << "Message received from fd " << _fd << "\taddress " << _host << ":" << _port << Reset << std::endl;
	if (_request->isParsed())
		_status = true;
	return 0;
}

/*
	Gestiona la response.
	Necesario conocer la location y saber si tiene cgi o no para poder lanzar los scripts
	La condición _request->getPath() == "/cgi/sum.sh" es solo para salir del paso por ahora.
	Realizar las pruebas con /cgi/sum.sh?num1=5&num2=5
	En desarrollo.
*/
int Client::sendResponse()
{
	if (_request->getPath() == "/cgi/sum.sh" && _cgi == NULL)
	{
		_cgi = new Cgi(_request, _events);
		_cgi->executeCgi(_cgiFd);
		_events->track_fdin[_cgiFd.first] = _fd;
		_events->track_fdout[_cgiFd.second] = _fd;
		_request->reset();
		_status = false;
	}
	
	else
	{
		int bytesSent;
		_request->reset();
		std::string res = _response->getResString(); 
		bytesSent = send(_fd, res.c_str(), res.size(), 0);
		if (bytesSent < 0)
			throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
		_response->reset();
		_status = false;
	}
	return 0;
}
