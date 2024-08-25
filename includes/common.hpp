#pragma once
#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <set>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

#define	Black	"\e[0;30m"
#define	Red		"\e[0;31m"
#define	Green	"\e[0;32m"
#define	Yellow	"\e[0;33m"
#define	Blue	"\e[0;34m"
#define	Purple	"\e[0;35m"
#define	Cyan	"\e[0;36m"
#define	White	"\e[0;37m"

#define High_Black	"\e[0;90m"
#define	High_Red	"\e[0;91m"
#define	High_Green	"\e[0;92m"
#define	High_Yellow	"\e[0;93m"
#define	High_Blue	"\e[0;94m"
#define	High_Purple	"\e[0;95m"
#define	High_Cyan	"\e[0;96m"
#define	High_White	"\e[0;97m"

#define Reset	"\e[0m"

extern int globalSigint;

struct Location_config
{
	std::string location;
	std::set<std::string> methods;
	std::pair<int, std::string> redir;
	bool autoindex;
	Location_config() : autoindex(false) {};
};

struct Server_config
{
	std::vector<std::string> server_names;
	std::vector<std::pair<std::string, int> > ip_port;
	std::string root;
	std::vector<std::string> index;
	std::map<int, std::string> error_pages;
	std::vector<Location_config> locations;
	size_t body_size;
	Server_config() : body_size(0) {};
};

struct Epoll_events
{
	int epfd;										// Epoll fd
	std::map<int, struct epoll_event> added;		// Configuración de Eventos
	std::vector<struct epoll_event> log;			// Notificaciones de Eventos
};
