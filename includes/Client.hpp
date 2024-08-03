#pragma once
#include "./common.hpp"
#include "./Request.hpp"
#include "./Response.hpp"

class Client //Quizás debería hacerse como una clase virtual?
{
	private:
		std::map<int, std::pair<Request, Response> > _data;
	public:
		Client();
		~Client();

		void addClient(int fd);
		void deleteClient(int fd);

		void addRequest(int fd);
		void sendResponse(int fd);

		int clientCounter();
};
