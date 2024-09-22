#include "../includes/lib.hpp"

Client::Client(Server_config *config, struct Epoll_events *events, int fd, int port, std::string host)
:_isCgi(false), _config(config), _events(events), _fd(fd), _port(port), _isReady(false), _host(host)
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
	int bytesRead;

	std::memset(buffer, 0, 10000);
	if (!_isCgi)
	{
		bytesRead = read(_fd, buffer, 10000);
		if (bytesRead == 0 || bytesRead < 0)
		{
			std::cout << Red << "Connection closed on fd " << _fd << Reset << std::endl;
			return -1;
		}
		std::cout << Cyan << "Message received from fd " << _fd << "\taddress " << _host << ":" << _port << Reset << std::endl;
		_request->fillRequest(buffer, bytesRead);
		_response->buildResponse();
		if (_request->isParsed())
			_isReady = true;
	}
	else
	{
		bytesRead = read(_cgiFd[0], buffer, 10000);
		if (bytesRead == 0)
			_isReady = true;
		else if (bytesRead < 0)
			throw std::runtime_error("Error: read: " + std::string(strerror(errno)));
		else
			_request->fillCgi(buffer);
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
	if (!_isCgi)
	{
		int bytesSent;
		_request->reset();
		std::string res = _response->getResString(); 
		bytesSent = send(_fd, res.c_str(), res.size(), 0);
		if (bytesSent < 0)
			throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
		_response->reset();
		_isReady = false;
		return 0;
	}
	// Alfonsete:
	// Si es CGI habría simplemente que gestionar el volcado del contenido de _cgiString de _request.
	// Antes de hacer cualquier cosa, Response debería checkear si hay un flag CGI establecido o no.
	// En Send hay que gestionar que se mande toda la respuesta y no una parte de ella.
	// Si Send retorna < 0 y no es un CGI entiendo que se debería entonces simplemente eliminar el cliente junto a los fds ya que el cliente ha cerrado la conexión antes de poder enviarle una respuesta.
	// Después de enviar la respuesta hay que cerrar los fds y eliminarlos del container de tracking de Epoll.
	// Ismael:
	// Esto me gustaría verlo juntos, Hay que chequear las locations y ver si se permiten cgi antes de lanzarlo
	// y una vez que se lanza creo que los formularios le envían directamente los datos del formulario al archivo especificado en action 
	// y el archivo se encarga de procesar los datos y devolver la respuesta o establecer los códigos de error.	
	
	return 0;
}

void Client::initCgi()
{
	_cgi = new Cgi(_fd ,_request, _events);
	_cgi->executeCgi(_cgiFd);
	this->_isCgi = true;
}
