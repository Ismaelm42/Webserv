#pragma once
#include "./common.hpp"
#include "./Socket.hpp"
#include "./Client.hpp"

class Server
{
	private:
		Socket _socket;
		Client _client;
		struct sockaddr_in _address;
		int _addressLen;
		int _serverFd;
		int _optval;
		int _port;
		int _epfd;
		int _socketFd;

	public:
		Server(int port);
		~Server();
		void configServer();
		void bindServer();
		void listenning();
		void acceptConnections();
};
