#include "../includes/lib.hpp"

/**
 * Constructor de la clase Server, que toma el puerto como argumento
 * establece el valor de _port 
 * incicializa _optval en 1,
 * establece el tamaño de la dirección en _addressLen tamaño de la estructura declarada en .hpp
 
 * Asigna a int _serverFd el retorno de creación del socket del servidor para posteriores 
    llamadas y chequea si se ha producido algún error:
   `socket()` toma tres argumentos: familia de protocolos, 
                                    tipo de socket 
                                    protocolo específico a utilizar. 
    `AF_INET`       indica familia de protocolos de Internet IPv4.
    `SOCK_STREAM`   indica un socket de flujo de dos vías confiable, ordenada y basada en bytes 
                    entre los procesos que es comúnmente utilizado para la comunicación TCP.
    `0`             indica que el sistema operativo eligirá el protocolo para el tipo de socket.
                    En el caso de `SOCK_STREAM`, el protocolo seleccionado será TCP.

 * Asigna al int _epfd el retorno de la creación del epoll y chequea error:
   `epoll_create()` toma un argumento que es el tamaño de la tabla de descriptores de archivos 
                    que se va a crear para monitorear multiples descriptores para poder leer y escribir.
                    Actualemnte el argumento, lo dejamos en uno porque este valor en versiones linux 2.6.8
                    y posteriores se ignora y se deja un valor mayor que cero para mantener compatibilidad
                    con versiones anteriores.
                    El valor de retorno se utiliza para identificar la tabla de descriptores de archivos.
 */

Server::Server(int port)
{
    _port = port;
    _optval = 1;
    _bodySize = 10000;
    _addressLen = sizeof(_address);
    _serverFd = ::socket(AF_INET, SOCK_STREAM, 0);
   	if (_serverFd < 0)
        throw std::runtime_error("Error: socket: " + std::string(strerror(errno)));
	_epfd = epoll_create(1);
	if (_epfd < 0)
		throw std::runtime_error("Error: epoll_create: " + std::string(strerror(errno)));
    _socket.initConfigValues(_epfd, _bodySize);
}

Server::~Server()
{
    close(_serverFd);
}

/**
 * Configuración del servidor y control de posibles errores mediante excepciones.
  `setsockopt()` establecemos opciones para nuestro socket `_serverFd`.
                `SOL_SOCKET` es el nivel de protocolo en el que se define la opción, en este caso a nivel de socket.
                `SO_REUSEADDR` permite que el socket reutilice direcciones locales y
                    es util en el caso de que una palicación necesite cerrarse y abrirse rápidamente ya que 
                    usa la misma dirección que podía estar usandose ya en esa instancia.
                `SO_REUSEPORT` permite usar el mismo puerto para.
                `_optval` es un puntero al valor que se va a establecer. (en nuestro caso 1, si lo hacemos despues 
                    instanciar la clase server).
                `sizeof(_optval)` es el tamaño del valor que se va a establecer.
    Este fragmento de código en C++ está utilizando la función `fcntl()` para cambiar las propiedades del descriptor de archivo `_serverFd`.

    `fcntl()` realiza operaciones en descriptores de archivo, como consultar o cambiar propiedades.
                `F_SETFL`establece las banderas de estado de _srverFd a `O_NONBLOCK`.
                la bandera `O_NONBLOCK` configura el descriptor de archivo para que 
                las operaciones de entrada y salida no bloqueen. 
                Esto significa que si una operación de lectura o escritura no puede completarse inmediatamente,
                la función retornará en lugar de bloquear la ejecución hasta que la operación pueda completarse.

    `fcntl()` devuelve -1 si hay un error.
    Asignamos a nuestra estructura de dirección `_address` la dirección, familia y puerto (en formato de red) que vamos a utilizar.
                    INADDR_ANY : el socket puede escuchar en cualquier interfaz de red disponible.
                    AF_INET, que representa la familia de protocolos de Internet IPv4
                    htons() convierte el número de puerto de host byte order a network byte order
                    - estándar para evitar inconsistencia entre como guardan los números en cada sistema-.
 */
void Server::configServer()
{
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEADDR, &_optval, sizeof(_optval)) != 0)
        throw std::runtime_error("Error: setsockopt: " + std::string(strerror(errno)));
    if (setsockopt(_serverFd, SOL_SOCKET, SO_REUSEPORT, &_optval, sizeof(_optval)) != 0)
        throw std::runtime_error("Error: setsockopt: " + std::string(strerror(errno)));
    if (fcntl(_serverFd, F_SETFL, O_NONBLOCK) < 0)
        throw std::runtime_error("Error: fcntl: " + std::string(strerror(errno)));
	
    _address.sin_addr.s_addr = INADDR_ANY;
	_address.sin_family = AF_INET;
	_address.sin_port = htons(_port);
}

/**
 *  bindServer asigna la dirección y el puerto a nuestro socket `_serverFd` y arroja excepción en caso de error.
    `bind()` toma tres argumentos: el descriptor de archivo del socket, 
                                    un puntero a una estructura sockaddr que contiene la dirección y el puerto, 
                                    y el tamaño de la estructura sockaddr.
    `bind()` devuelve -1 si hay un error.
 */

void Server::bindServer()
{
    if (bind(_serverFd, (struct sockaddr *)&_address, sizeof(_address)) < 0)
        throw std::runtime_error("Error: bind: " + std::string(strerror(errno)));
}

/**
 * listenning() pone el servidor en modo de escucha (pasivo) y arroja excepción en caso de error.
    Es utilizado para posteriores llamas a `accept()`.
    `listen()` toma dos argumentos: 
                _serverFd ocket y el número máximo de conexiones pendientes que se permiten.
    `listen()` devuelve -1 si hay un error.
 */
void Server::listenning()
{
    if (listen(_serverFd, 1) < 0)
        throw std::runtime_error("Error: listen: " + std::string(strerror(errno)));
    std::cout << Yellow << "Server started listenning on port " << _port << Reset << std::endl;
}

/**
 * acceptConnections() acepta conexiones entrantes y las añade a la lista de clientes.
    `accept()` toma tres argumentos: 
                el descriptor de archivo del socket, 
                un puntero a una estructura sockaddr que contiene la dirección y el puerto del cliente, 
                y un puntero a un entero que contiene el tamaño de la estructura sockaddr.
    `accept()` devuelve -1 si hay un error.
    Si el valor de retorno es mayor o igual a 0, se añade el cliente a la lista de clientes
    instancia de cliente creada en la declaración de la clase Server. (.hpp)
    y se configura el evento del socket.
    Se chequean los eventos y se procesan.
 */
void Server::acceptConnections()
{
    while (true)
	{
	    _socketFd = accept(_serverFd, (struct sockaddr *)&_address, (socklen_t *)&_addressLen);
        if (_socketFd < 0 && (errno != EAGAIN && errno != EWOULDBLOCK))
            throw std::runtime_error("Error: accept: " + std::string(strerror(errno)));
        if (_socketFd >= 0)
        {
            std::cout << Green << "Connection accepted with socket fd " << _socketFd << Reset << std::endl;
            Client new_client(_socketFd);
            _client.emplace(_socketFd, new_client);
            _socket.configEvent(_socketFd);
            _socket.addEvent(_socketFd);
        }
        if (!_client.empty())
        {
            _socket.checkEvents();
            _socket.processEvents(_client);
        }
    }
}

