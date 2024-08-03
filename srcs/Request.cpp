#include "../includes/lib.hpp"

Request::Request()
{

}

Request::~Request()
{

}

std::string Request::getRequest()
{
	return _request;
}

void Request::addRequest(int fd)
{
	char buffer[10000];
	int bytesRead;

	std::memset(buffer, 0, 10000);
	bytesRead = read(fd, buffer, 10000);
	if (bytesRead == 0 || bytesRead < 0)
	{
		close(fd);
		throw std::runtime_error("Error: read: " + std::string(strerror(errno)));
	}
	_request = buffer;
	std::cout << "Received message:\n" << _request << std::endl;
	std::cout << fd << std::endl;
}

void Request::eraseRequest()
{
	_request.clear();
}
