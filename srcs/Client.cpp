#include "../includes/lib.hpp"

Client::Client()
{

}

Client::~Client()
{

}

void Client::addClient(int fd)
{
	_data[fd].first = Request();
	_data[fd].second = Response();
}

void Client::deleteClient(int fd)
{
	_data.erase(fd);
}

void Client::addRequest(int fd)
{
	_data[fd].first.addRequest(fd);
}

void Client::sendResponse(int fd)
{
	std::string request = _data[fd].first.getRequest();

	if (!request.empty())
	{
		_data[fd].second.addResponse(fd, request);
		_data[fd].first.eraseRequest();
		_data[fd].second.eraseResponse();
	}
}

int Client::clientCounter()
{
	return _data.size();
}
