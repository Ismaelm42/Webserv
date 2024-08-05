#include "../includes/lib.hpp"

Client::Client()
{
	
}

Client::Client(int fd)
{
	_fd = fd;
}

Client::~Client()
{

}

void Client::handleRequest()
{
	_request.addRequest(_fd);
}

void Client::handleResponse()
{
	std::string str = _request.getRequest();

	if (!str.empty())
	{
		_response.addResponse(_fd, str);
		_request.eraseRequest();
		_response.eraseResponse();
	}
}
