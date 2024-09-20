#pragma once
#include "./common.hpp"
#include "./Request.hpp"

class Request;

class Cgi
{
	private:
		// int _fd;
		pid_t _pid;
		char **_argv;
		char **_envp;
		int _pipeFd[2];
		Request *_request;
		struct Epoll_events *_events;
	public:
		Cgi(Request *request, Epoll_events *events);
		~Cgi();
		void setEnvironment();
		void setArguments();
		void childProcess();
		void executeCgi(std::pair<int, int> &cgiFd)
};
