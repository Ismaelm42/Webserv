#pragma once
#include "./common.hpp"
#include "./Cgi.hpp"
#include "./Request.hpp"
#include "./Response.hpp"

class Cgi;

class Client
{
	private:
		Cgi *_cgi;
		Request *_request;
		Response *_response;
		Server_config *_config;

	public:
		int _fd;
		int _port;
		bool _isReady;
		std::string _res;
		std::string _host;
		ssize_t _bytesRead;
		ssize_t _bytesSent;

		Client(Server_config *config, int fd, int port, std::string host);
		~Client();
		int getRequest();
		int sendResponse();
		void resetClient();
		void buildResponse();
		void initCgi(int *code, std::string &output);
};
