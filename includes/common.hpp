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
#include <dirent.h> 

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
#define Reset		"\e[0m"
#define Log			"\e[0;90m[Webserv] \e[0m"

extern int globalSigint;

struct Location_config
{
	bool autoindex;
	size_t body_size;
	std::string location;
	std::set<std::string> methods;
	std::vector<std::string> index;
	std::pair<int, std::string> redir;
	std::vector<std::pair<std::string, std::string> > cgi;
	std::string auth_basic;
	std::string auth_basic_user_file;
	Location_config() : autoindex(false) , body_size(0) {};
};

struct Server_config
{
	size_t body_size;
	std::string root;
	std::vector<std::string> index;
	std::vector<std::string> server_names;
	std::map<int, std::string> error_pages;
	std::vector<Location_config> locations;
	std::vector<std::pair<std::string, int> > host_port;
	Server_config() : body_size(1000000) {};
};

struct Epoll_events
{
	int epfd;
	std::vector<struct epoll_event> log;
	std::map<int, struct epoll_event> added;
};

template <typename T>
std::string toStr(const T toConvert)
{
    std::stringstream stream;
	stream << toConvert;
    return stream.str();
}

inline int checkFileOrDirectory(std::string &path, const std::string type)
{
	struct stat stat_buffer;

    if (stat(path.c_str(), &stat_buffer) < 0)
    {
		if (errno == ENOENT)
			return 404;
		else if (errno == EACCES || !(stat_buffer.st_mode & S_IRUSR))
			return 403;
        else
			return 500;
    }
	if ((type == "dir" && !S_ISDIR(stat_buffer.st_mode)) || (type == "file" && S_ISDIR(stat_buffer.st_mode)))
		return 400;
	return 0;
}

const std::string base64_chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

enum Methods
{
	GET,
	POST,
	DELETE,
	NONE
};

enum fillStatusEnum				
{
	get_First,
	get_Method,
	get_First_Space,
	get_First_Slash,
	get_URI_Path,
	get_URI_Query,
	get_URI_Fragment,
	get_Protocol,
	get_CR,
	get_LF,
	header_Name_Start,
	headers_End,
	header_Name,
	header_Value,
	header_Value_End,
	Chunk_Length_Begin,
	Chunk_Length,
	Chunk_Ignore,
	Chunk_Length_CR,
	Chunk_Length_LF,
	Chunk_body,
	Chunk_body_CR,
	Chunk_body_LF,
	Chunk_End_CR,
	Chunk_End_LF,
	get_Body,
	Parsed
};
