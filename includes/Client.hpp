#pragma once
#include "./common.hpp"

class Client
{
	private:
		int _fd;								// Fd del cliente
		int _status;							// Flag que indica si el cliente ha recibido una request
		std::string _request;					// Request (quizás sea interesante guardar la request en un stream para despiezarla luego en varios strings)
		std::string _response;					// Response
	public:
		Client(int fd);
		~Client();
		int getStatus();
		int getRequest();
		int sendResponse();
};
