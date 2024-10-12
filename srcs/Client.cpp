#include "../includes/lib.hpp"
#include "../includes/Client.hpp"

Client::Client(Server_config *config, int fd, int port, std::string host)
:_config(config), _fd(fd), _port(port), _isReady(false), _host(host)
{
	_bytesRead = 0;
	_bytesSent = 0;
	_request = new Request(this, _config);
	_response = new Response(this, _config, _request);
	_cgi = NULL;
}

Client::~Client()
{
	delete _request;
	delete _response;
	if (_cgi != NULL)
		delete _cgi;
}

void Client::buildResponse()
{
	std::cout << Log << Cyan << "Message received from socket " << _fd << " " << _host << ":" << _port << Reset << std::endl;
	_response->buildResponse();
	_res = _response->getResString();
	_isReady = true;
}

int Client::getRequest()
{
	char buffer[10000];

	std::memset(buffer, 0, 10000);

	_bytesRead = read(_fd, buffer, 10000);
	if (_bytesRead == 0 || _bytesRead < 0)
	{
		std::cout << Log << Red << "Connection closed on socket " << _fd << " " << _host << ":" << _port << Reset << std::endl;
		return -1;
	}
	_request->fillRequest(buffer, _bytesRead);
	if (_request->isParsed())
		buildResponse();
	return 0;
}

int Client::sendResponse()
{
	ssize_t bytes = send(_fd, _res.c_str() + _bytesSent, _res.size() - _bytesSent, 0);
	if (bytes <= 0)
	{
		if (bytes < 0)
			std::cout << Log << Red << "Error sending data on socket " << _fd << " " << _host << ":" << _port << Reset << std::endl;
		else
			std::cout << Log << Red << "Connection closed on socket " << _fd << " " << _host << ":" << _port << Reset << std::endl;
		return -1;
	}
	_bytesSent += bytes;
	if (_bytesSent == static_cast<ssize_t>(_res.size()))
	{
		std::cout << Log << High_Cyan << "Response sent to socket " << _fd << " " << _host << ":" << _port << Reset << std::endl;
		resetClient();
		_isReady = false;
	}
	return 0;
}

void Client::initCgi(int *code, std::string &output)
{
	_cgi = new Cgi(_request, _config);
	*code = _cgi->executeCgi(output, _request->getBody());
}

void Client::resetClient()
{
	_bytesRead = 0;
	_bytesSent = 0;
	_request->reset();
	_response->reset();
	if (_cgi != NULL)
		delete _cgi;
	_cgi = NULL;
}
