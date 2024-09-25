#include "../includes/lib.hpp"

Client::Client(Server_config *config, struct Epoll_events *events, int fd, int port, std::string host)
:_isCgi(false), _config(config), _events(events), _fd(fd), _port(port), _isReady(false), _host(host)
{
	_request = new Request(this, _config, _events);
	_response = new Response(this, _config, _request, _events);
	_cgi = NULL;
	_cgiFd[0] = 0;
	_cgiFd[1] = 0;
}

Client::~Client()
{
	delete _request;
	delete _response;
	if (_cgi != NULL)
		resetCgi();
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

	if (!_isCgi)
	{
		/*
			IMPORTANTE: Ismael:
			¿Te suena porque cuando incluyo localhost:8081/redir sin la barra al final 
			la lectura me da: GET /.root/ HTTP/1.1 y cuando incluyo la barra al final
			puedo realizar la redirección de forma correcta?
		*/
		bytesRead = read(_fd, buffer, 10000);
		if (bytesRead == 0 || bytesRead < 0)
		{
			std::cout << Red << "Connection closed on fd " << _fd << Reset << std::endl;
			return -1;
		}
		std::cout << Cyan << "Message received from fd " << _fd << "\taddress " << _host << ":" << _port << Reset << std::endl;
		_request->fillRequest(buffer, bytesRead);
		
		if (_request->isParsed())
			_isReady = true;
		std::cout << Purple << "Request no CGI client fd " << _fd << Reset << std::endl;
	}
	else
	{
		bytesRead = read(_cgiFd[0], buffer, 10000);
		if (bytesRead < 0)
			throw std::runtime_error("Error: read: " + std::string(strerror(errno)));
		_request->fillCgi(buffer);
		_isReady = true;
		std::cout << Purple << "Request CGI client fd " << _fd << Reset << std::endl;
		std::cout << High_Blue << buffer << Reset << std::endl;
	}
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
	std::string res;
	int bytesSent;

	if (!_isCgi)
	{
		_response->buildResponse();
		if (_isCgi)
		{
			_request->reset();
			_response->reset(false);
			_isReady = false;
			return 0;
		}
		else
		{
			res = _response->getResString(); 
			bytesSent = send(_fd, res.c_str(), res.size(), 0);
			if (bytesSent < 0)
			{
				std::cout << Red << "Connection closed on fd " << _fd << Reset << std::endl;
				return -1;
			}
			_request->reset();
			_response->reset(false);
			_isReady = false;
		}
	}
	else
	{
		res = _request->getcgiString();
		std::cout << Purple << "Response CGI client fd " << _fd << Reset << std::endl;
		bytesSent = send(_fd, res.c_str(), res.size(), 0);
		// bytesSent = write(_cgiFd[1], res.c_str(), res.size());
		if (bytesSent < 0)
			throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
		_isCgi = false;
		_request->reset();
		_response->reset(true);
		_isReady = false;
	}
	// Ismael:
	// Esto me gustaría verlo juntos, Hay que chequear las locations y ver si se permiten cgi antes de lanzarlo
	// y una vez que se lanza creo que los formularios le envían directamente los datos del formulario al archivo especificado en action 
	// y el archivo se encarga de procesar los datos y devolver la respuesta o establecer los códigos de error.	
	return 0;
}

void Client::initCgi()
{
	std::cout << Yellow << "InitCGI" << Reset << std::endl; 
	_cgi = new Cgi(_fd ,_request, _events);
	_cgi->executeCgi(_cgiFd);
	_isCgi = true;
}

void Client::resetCgi()
{
	std::cout << Yellow << "ResetCGI" << Reset << std::endl;
	_isCgi = false;
	_events->cgi_in.erase(_cgiFd[0]);
	_events->cgi_in.erase(_cgiFd[1]);
	deleteEvent(_cgiFd[0], _events);
	deleteEvent(_cgiFd[1], _events);
	close(_cgiFd[0]);
	close(_cgiFd[1]);
	delete _cgi;
	_cgi = NULL;
}
