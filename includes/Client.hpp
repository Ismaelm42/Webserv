#pragma once
#include "./common.hpp"
#include "./Request.hpp"
#include "./Response.hpp"

class Client //Quizás debería hacerse como una clase virtual?
{
	private:
		std::map<int, std::pair<Request, Response> > _data; // map of clients with request and response
	public:
		Client();
		~Client();

		void addClient(int fd); 		// add client
		void deleteClient(int fd);		// delete client

		void addRequest(int fd);		// add request	
		void sendResponse(int fd);		// send response

		int clientCounter();			// client counter
};
