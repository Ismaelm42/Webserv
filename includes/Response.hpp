#pragma once
#include "./common.hpp"

class Response
{
	private:

	public:
		Response();
		~Response();
		void addResponse(int fd, std::string request);
		void eraseResponse();
};
