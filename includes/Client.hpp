#pragma once
#include "./common.hpp"

class Client
{
	private:
		int _fd;
		int _status;
		std::string _request;
		std::string _response;
	public:
		Client(int fd);
		~Client();
		int getStatus();
		int getRequest();
		int sendResponse();
};
