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
