#include "../includes/lib.hpp"

Client::Client(Server_config *config, struct Epoll_events *events, int fd, int port, std::string host)
:_config(config), _events(events), _fd(fd), _port(port), _isReady(false), _host(host)
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
	std::cout << Cyan << "Message received from fd " << _fd << "\taddress " << _host << ":" << _port << Reset << std::endl;
	_request->fillRequest(buffer, bytesRead);
	if (_request->isParsed())
		_isReady = true;
	
	return 0;
}

int Client::sendResponse()
{
	_response->buildResponse();

	std::string res = _response->getResString(); 
	int bytesSent = send(_fd, res.c_str(), res.size(), 0);
	if (bytesSent < 0)
	{
		std::cout << Red << "Connection closed on fd " << _fd << Reset << std::endl;
		return -1;
	}
	resetClient();
	_isReady = false;
	return 0;
}

void Client::initCgi(int *code, std::string &output)
{
	std::cout << Yellow << "InitCGI" << Reset << std::endl; 
	_cgi = new Cgi(_fd, _request, _config);
	*code = _cgi->executeCgi(output);
}

void Client::resetClient()
{
	_request->reset();
	_response->reset();
	if (_cgi != NULL)
		delete _cgi;
	_cgi = NULL;
}
