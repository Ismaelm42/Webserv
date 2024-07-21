#include "../includes/test.hpp"

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

	/*
		Creación del socket
		__domain	AF_INET			Adress Family con uso de direcciones IPV4.
		__type		SOCK_STREAM		Especifica que el socket utilizará el protocolo de transporte TCP.
		__protocol	0				Selecciona el valor predefinido que en este caso es IPPROTO_TCP
	*/
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    /*
		Configuración del socket
		__fd						server_fd			fd del socket.
		__level						SOL_SOCKET			Configura opciones que se aplican al socket. Opciones IPPROTO_TCP para TCP e IPPROTO_IP para IP.	
		__optname					SO_REUSEADRR		Permite reutilizar una dirección local para el socket.
		__optname					SO REUSEPORT		Permite que varios sockets en la misma máquina se enlacen al mismo puerto UDP o TCP.
		__optval					&opt				On/Off de optname. Si se establece en 1 se activa __optname, en 0 se desactivan.
   		
		address.sin_family			AF_INET				Adress Family con uso de direcciones IPV4.	
   		address.sin_addr.s_addr		INADDR_ANY			Indica que el servidor aceptará conexiones en todas las interfaces de red disponibles.
    	address.sin_port			htons(PORT)			Establece el puerto en el que el socket escuchará. htons convierte el int a big-endian (formato de red).
		address.sin_zero								No hay necesidad de modificarlo. Asegura el cast de una estructura sockaddr_in a sockaddr.
	*/
    int opt = 1;
    const int PORT = 8080;

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) != 0)
	{
        perror("setsockopt failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_family = AF_INET;
	address.sin_port = htons(PORT);

   /*
		Bind al puerto
		__fd		server_fd						fd del socket.
		__addr		(struct sockaddr *)&address		Dirección local a la que se debe asociar el socket. Se castea a sockaddr.
		__len		sizeof(address)					Tamaño de la estructura address.
   */
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /*
		Escucha en localhost y puerto
		__fd		server_fd		fd del socket 
		__n			1				Valor del "backlog". Establece el tamaño máximo de la cola de conexiones pendientes.
	*/
    if (listen(server_fd, 1) < 0)
	{
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    std::cout << "Server started on port " << PORT << std::endl;

    while (true)
	{
		new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
		{
            perror("accept failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        char buffer[1024] = {0};
		//utilizar mejor getline
        read(new_socket, buffer, 1024);
        std::cout << "Received request:\n" << buffer << std::endl;

        std::string response = "Hello, world!\n";
        send(new_socket, response.c_str(), response.size(), 0);
        close(new_socket);
    }

    close(server_fd);
    return 0;
}

