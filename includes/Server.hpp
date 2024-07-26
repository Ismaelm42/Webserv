#pragma once
#include "./common.hpp"
#include "./Socket.hpp"

class Server
{
	private:
		Socket _socket;
		struct sockaddr_in _address;
		int _serverFd;
		int _port;

	public:
		Server(int port);
		~Server();
		void configServer();
		void bindServer();
		void listenning();
		void acceptConnections();
};
