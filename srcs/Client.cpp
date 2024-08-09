#include "../includes/lib.hpp"

Client::Client(int fd)
:_fd(fd), _status(0)
{
	
}

int Client::getStatus()
{
	return _status;
}

int Client::getRequest()
{
	char buffer[10000];
	int bytesRead;

	std::memset(buffer, 0, 10000);
	bytesRead = read(_fd, buffer, 10000);
	if (bytesRead == 0 || bytesRead < 0)
	{
		std::cout << "Error: read: " << std::string(strerror(errno)) << std::endl;
		return -1;
	}
	_request = buffer;
	std::cout << High_Cyan << "Message received from fd " << _fd << ":" << Reset << std::endl;
	_status = 1;
	return 0;
}

int Client::sendResponse()
{
	int bytesSent;
	std::stringstream responseStream;

	responseStream << "HTTP/1.1 200 OK\r\n";
	responseStream << "Content-Type: text/plain\r\n";
	responseStream << "Content-Length: " << 29 << "\r\n";
	responseStream << "\r\n";
	responseStream << "Hello, world! from socket " << _fd << "\r\n";

	std::string response = responseStream.str();
	bytesSent = send(_fd, response.c_str(), response.size(), 0);
	if (bytesSent < 0)
		throw std::runtime_error("Error: send: " + std::string(strerror(errno)));
	_status = 0;
	return 0;
}
