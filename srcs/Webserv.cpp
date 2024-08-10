#include "../includes/lib.hpp"

/*
	Constructor de la clase Webserv.
		Crea una nueva instancia de Epoll_events y asigna a _events.
		Inicializa el epoll descriptor (epfd) usando epoll_create con un tamaño de 1.
			- Si epoll_create falla, lanza una excepción con el mensaje de error correspondiente.
		Añade puertos predeterminados (8080, 4040, 9090) a la lista _ports para que el servidor los use.
*/
Webserv::Webserv()
{
	_events = new Epoll_events;
	_events->epfd = epoll_create(1);
	if (_events->epfd < 0)
		throw std::runtime_error("Error: epoll_create: " + std::string(strerror(errno)));
	_ports.push_back(8080);
	_ports.push_back(4040);
	_ports.push_back(9090);
}
/*
	Destructor de la clase Webserv.
		Libera la memoria asignada a _events utilizando delete.
		Recorre la lista de servidores (_servers) y elimina cada servidor utilizando delete.
		   - Esto asegura que todos los recursos asociados a los servidores se liberen correctamente.
*/
Webserv::~Webserv()
{
	delete _events;
	for (std::vector<Server*>::iterator it = _servers.begin(); it != _servers.end(); it++)
		delete *it;
}

/*
	Inicializa los servidores en los puertos especificados.
		Recorre la lista de puertos (_ports).
		Crea una nueva instancia de Server para cada puerto, pasando el puerto y el objeto _events.
		Añade cada nuevo servidor a la lista _servers para su posterior uso.
*/
void Webserv::initializeServers()
{
	for(unsigned i = 0; i < _ports.size(); i++)
	{
		Server *server = new Server(_ports[i], _events);
		_servers.push_back(server);
	}
}

/*
	Ejecuta el servidor, aceptando conexiones y manejando eventos.
		Llama a initializeServers() para configurar los servidores en los puertos especificados.
		Entra en un bucle que continúa hasta que globalSigint sea verdadero:
			- Acepta conexiones entrantes de los servidores, añadiendo nuevos clientes y registrando eventos.
			- Si hay clientes para procesar, maneja los eventos correspondientes.
			- Recorre los servidores en la lista _servers en un bucle circular, reiniciando al llegar al final de la lista.
*/
void Webserv::run()
{
	initializeServers();
	
	std::vector<Server*>::iterator it = _servers.begin();
	Server *server;
	int fd;

	while (!globalSigint)
	{
		server = *it;
		fd = server->acceptConnections();
		if (fd >= 0)
		{
			server->addClient(fd);
			server->recordEvent(fd);
		}
		if (server->hasClientsToProcess())
			server->handleEvents();
		it = (++it == _servers.end()) ? _servers.begin() : it;
	}
}
