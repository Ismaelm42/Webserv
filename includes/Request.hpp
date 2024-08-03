#pragma once
#include "./common.hpp"

class Request
{
	private:
		std::string _request;

	public:
		Request();
		~Request();

		std::string getRequest();
		void addRequest(int fd);
		void eraseRequest();

};
