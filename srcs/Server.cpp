#include "../includes/lib.hpp"

/*
	Constructor de la clase Server que configura el socket y lo prepara para aceptar conexiones.
		Inicializa el socket usando AF_INET (IPv4) y SOCK_STREAM (TCP).
		Configura opciones del socket:
			- SO_REUSEADDR: Permite reutilizar la dirección local.
			- SO_REUSEPORT: Permite que varios sockets se vinculen al mismo puerto.
		Configura el socket en modo no bloqueante usando fcntl.
		Configura la estructura sockaddr_in para que el socket acepte conexiones en cualquier dirección IP 
		y puerto especificado por _port.
		Asocia el socket con la dirección y puerto especificados mediante bind.
		Configura el socket para que escuche conexiones entrantes con una cola de 50 conexiones pendientes usando listen.
		Imprime un mensaje de confirmación indicando que el servidor está escuchando en el puerto especificado.
*/
Server::Server(std::string ip, int port, struct Epoll_events *events, Server_config *config)
:_port(port), _ip(ip), _config(config), _events(events)
{
	int optval = 1;

    _socket = ::socket(AF_INET, SOCK_STREAM, 0);
   	if (_socket < 0)
        throw std::runtime_error("Error: socket: " + std::string(strerror(errno)));

    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) != 0)
        throw std::runtime_error("Error: setsockopt: " + std::string(strerror(errno)));
    if (setsockopt(_socket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) != 0)
        throw std::runtime_error("Error: setsockopt: " + std::string(strerror(errno)));
    if (fcntl(_socket, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Error: fcntl: " + std::string(strerror(errno)));
	
    _sockaddr.sin_addr.s_addr = inet_addr(_ip.c_str());
	_sockaddr.sin_family = AF_INET;
	_sockaddr.sin_port = htons(_port);

    if (bind(_socket, (struct sockaddr *)&_sockaddr, sizeof(_sockaddr)) < 0)
        throw std::runtime_error("Error: bind: " + std::string(strerror(errno)));

    if (listen(_socket, 50) < 0)
        throw std::runtime_error("Error: listen: " + std::string(strerror(errno)));
    std::cout << High_Green << "Server listenning on " << _ip << ":" << _port << Reset << std::endl;
}
/*
	Destructor de la clase Server.
		Recorre el contenedor _clients, que es un std::map de descriptores de cliente (fd) a punteros de objetos Client.
		Para cada elemento del std::map, elimina el objeto Client asociado al descriptor de cliente utilizando delete.
			- Esto libera la memoria dinámica asignada a cada objeto Client.
		El contenedor _clients se limpia automáticamente después de que el destructor haya terminado, ya que solo se eliminan los objetos, no el propio contenedor.
*/
Server::~Server()
{
	for (std::map<int, Client*>::iterator it = _clients.begin(); it != _clients.end(); it++)
		delete it->second;
	_clients.clear();
}

/*
	Método que acepta nuevas conexiones entrantes en el socket.
		Declara una variable client para almacenar el descriptor del nuevo socket de cliente.
		Llama a accept para aceptar una conexión entrante en el socket _socket.
			- _sockaddr se utiliza para almacenar la dirección del cliente que se está conectando.
			- _sockaddrlen es el tamaño de la estructura _sockaddr.
		Si accept devuelve un valor menor a 0 (indicando un error) y el error no es EAGAIN ni EWOULDBLOCK,
		lanza una excepción con el mensaje de error correspondiente usando strerror para obtener la descripción del error.
		Configura el fd en modo no bloqueante usando fcntl (en principio necesario para las siguientes operaciones de lectura).
		Devuelve el descriptor del socket del cliente (client) si la conexión fue aceptada exitosamente.
*/
int Server::acceptConnections()
{
	_sockaddrlen = sizeof(_sockaddr);
	int client = accept(_socket, (struct sockaddr *)&_sockaddr, (socklen_t *)&_sockaddrlen);
	if (client < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
		throw std::runtime_error("Error: accept: " + std::string(strerror(errno)));
    if (client > 0 && fcntl(client, F_SETFL, O_NONBLOCK) < 0)
		throw std::runtime_error("Error: fcntl: " + std::string(strerror(errno)));
	return client;
}

/*
	Registra un nuevo evento en el epoll y añade un nuevo cliente a la lista de clientes.
		Si accept devuelve por un fd válido y no -1 se asigna un nuevo cliente con ese fd y se crea un nuevo cliente.
		Configura un objeto epoll_event con los eventos EPOLLIN y EPOLLOUT para el descriptor de archivo fd.
		Almacena el evento en el contenedor _events->added.
		Usa epoll_ctl para añadir el descriptor de archivo y el evento al epoll.
		Lanza una excepción si epoll_ctl falla.
		Crea un nuevo objeto Client utilizando el descriptor de archivo fd.
		Asocia este objeto al descriptor de archivo en el contenedor _clients (un std::map de descriptores de archivo a punteros de Client).
		La función asegura que cada descriptor de archivo tenga un objeto Client asociado.
*/
void Server::addEventandClient(int fd)
{
	if (fd > 0)
	{
		if (_clients.find(fd) == _clients.end())
		{
			std::cout << High_Cyan << "Socket with fd " << fd << " has been created" << Reset << std::endl;
    		Client *client = new Client(_ip, _port, fd, _config);
    		_clients[fd] = client;
		}
		struct epoll_event event;
		event.events = EPOLLIN | EPOLLOUT;
		event.data.fd = fd;
		_events->added[fd] = event;
		if (epoll_ctl(_events->epfd, EPOLL_CTL_ADD, fd, &_events->added[fd]) == -1)
			throw std::runtime_error("Error: epoll_ctl_add: " + std::string(strerror(errno)));
	}
}

/*
	Elimina un evento del epoll y del contenedor de eventos y elimina el cliente de la lista de clientes.
		Usa epoll_ctl para eliminar el descriptor de archivo fd del epoll.
		Borra el evento correspondiente del contenedor _events->added.
		Recorre el contenedor _events->log para encontrar y eliminar el evento asociado al descriptor de archivo fd.
		Cierra el descriptor de archivo utilizando close.
		Borra el objeto Client asociado al descriptor de archivo fd del contenedor _clients.
		La memoria del objeto Client se libera utilizando delete.
		El descriptor de archivo se elimina del contenedor _clients.
*/
void Server::deleteEventandClient(int fd)
{
	if (epoll_ctl(_events->epfd, EPOLL_CTL_DEL, fd, NULL) == -1)
		throw std::runtime_error("Error: epoll_ctl_delete: " + std::string(strerror(errno)));
    _events->added.erase(fd);
	for (std::vector<struct epoll_event>::iterator it = _events->log.begin(); it != _events->log.end(); it++)
    {
        if (it->data.fd == fd)
        {
            it = _events->log.erase(it);
            break ;
        }
    }
	delete _clients[fd];
	_clients.erase(fd);
	close(fd);
	std::cout << "Event and client associated with fd " << fd << " has been removed." << std::endl;   
}

/*
	Maneja la solicitud de un cliente.
		Verifica si el descriptor de archivo fd está en el contenedor _clients.
		Si el cliente está presente, llama a getRequest() para procesar la solicitud.
		Si getRequest() devuelve un valor negativo, elimina el evento y el cliente, indicando que la conexión ha fallado o se ha cerrado.
*/
void Server::handleRequest(int fd)
{
	if (_clients.find(fd) != _clients.end())
		if (_clients[fd]->getRequest() < 0)
			deleteEventandClient(fd);
}

/*
	Maneja la respuesta para un cliente.
		Verifica si el descriptor de archivo fd está en el contenedor _clients y si el cliente tiene un estado que requiere una respuesta.
		Si es así, llama a sendResponse() del cliente para enviar la respuesta.
*/
void Server::handleResponse(int fd)
{
	if (_clients.find(fd) != _clients.end() && _clients[fd]->getStatus())
		_clients[fd]->sendResponse();
}

/*
	Maneja los eventos de epoll.
		Ajusta el tamaño del contenedor _events->log para que coincida con el número de eventos en _events->added.
		Usa epoll_wait para esperar eventos y llenar _events->log con los eventos que han ocurrido.
		Recorre _events->log y maneja cada evento:
			- Si el evento indica un error (EPOLLERR) o una desconexión (EPOLLHUP), es que una desconexión
				abrupta por parte del cliente se ha originado. Se elimina el cliente y los eventos relacionados con
				ese fd.
			- Si el evento es de lectura (EPOLLIN), llama a handleRequest() para procesar la solicitud del cliente.
			- Si el evento es de escritura (EPOLLOUT), llama a handleResponse() para enviar una respuesta al cliente.
*/
void Server::handleEvents()
{
	if (_events->added.empty())
		return;
	_events->log.resize(_events->added.size());
    if (epoll_wait(_events->epfd, _events->log.data(), _events->log.size(), -1) == -1)
		throw std::runtime_error("Error: epoll_wait: " + std::string(strerror(errno)));
	for (long unsigned i = 0; i < _events->log.size(); i++)
	{
		if (_events->log[i].events & EPOLLERR || _events->log[i].events & EPOLLHUP)
		{
			std::cout << Red << "Connection unexpectedly reset by peer" << Reset << std::endl;
			deleteEventandClient(_events->log[i].data.fd);
		}
		else
		{
			if (_events->log[i].events & EPOLLIN)
				handleRequest(_events->log[i].data.fd);
			if (_events->log[i].events & EPOLLOUT)
				handleResponse(_events->log[i].data.fd);
		}
	}
}
