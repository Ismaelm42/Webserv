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
	bool autoindex;
	size_t body_size;
	std::string location;
	std::set<std::string> methods;
	std::vector<std::string> index; 		// chequear este antes que el general
	std::pair<int, std::string> redir;		// código de error nuevadirección
	std::vector<std::pair<std::string, std::string> > cgi;
	Location_config() : autoindex(false) , body_size(0) {};
};

struct Server_config
{
	size_t body_size;
	std::string root;
	std::vector<std::string> index;      	// sino hay en location se mira en general y si no 404
	std::vector<std::string> server_names;
	std::map<int, std::string> error_pages; // mapa con código de error y ruta al target marcado por confirguración
	std::vector<Location_config> locations;	// chequear este antes que el general y si no 404
	std::vector<std::pair<std::string, int> > ip_port;
	Server_config() : body_size(1000000) {};
};

struct Epoll_events
{
	int epfd;										// Epoll fd
	std::vector<struct epoll_event> log;			// Notificaciones de Eventos
	std::map<int, struct epoll_event> added;		// Configuración de Eventos
};

inline void addEvent(int fd, struct Epoll_events *events)
{
	if (fd > 0)
	{
		struct epoll_event newEvent;
		newEvent.events = EPOLLIN | EPOLLOUT;
		newEvent.data.fd = fd;
		events->added[fd] = newEvent;
		if (epoll_ctl(events->epfd, EPOLL_CTL_ADD, fd, &events->added[fd]) == -1)
			throw std::runtime_error("Error: epoll_ctl_add: " + std::string(strerror(errno)));
	}
}

inline void deleteEvent(int fd, struct Epoll_events *events)
{
	if (epoll_ctl(events->epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw std::runtime_error("Error: epoll_ctl_delete: " + std::string(strerror(errno)));
    events->added.erase(fd);
	for (std::vector<struct epoll_event>::iterator it = events->log.begin(); it != events->log.end(); it++)
    {
        if (it->data.fd == fd)
        {
            it = events->log.erase(it);
            break ;
        }
    }

}

enum Methods   // incluir tanto aquí como en _initMethodStr los métodos permitidos
{
	GET,
	POST,
	DELETE,
	NONE
};

/**
 * @brief definir estado del parseo.
 * 
 * Pendiente de revisar para ver si es posible dividir en más estado y 
 * pendiente de corregir la función de error para ver si es necesario incluir los mensajes
 * o si lo dejamos como un debugger.
 * 
 *    ----- Pendiente de revisar la forma de lectura de las partes que se hacen comparando con cadenas
 *    ----- Sobre todo el protocolo ya que he dejado por defecto el 1.1 como único protocolo aceptado
 * 
 */
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

