#pragma once
#include "./common.hpp"
#include "./Socket.hpp"
#include "./Client.hpp"

class Server
{
	private:
		Socket _socket; 				// socket instance
		std::map<int, Client> _client;	// client instance
		struct sockaddr_in _address; 	// address structure
		int _addressLen; 				// address length
		int _serverFd; 					// server file descriptor
		int _bodySize;
		int _optval; 					// to cheange the socket propeties
		int _port; 						// conection port
		int _epfd; 						// epoll file descriptor
		int _socketFd; 					// socket file descriptor

	public:
		Server(int port);
		~Server();
		void configServer();		    // config server
		void bindServer();				// bind server
		void listenning();				// listenning
		void acceptConnections();		// accept connections
};
