#include "../includes/lib.hpp"

int main()
{
    Server server(8080);
    
    try
    {
        server.configServer();
        server.bindServer();
        server.listenning();
        server.acceptConnections();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}


//crear un epoll_create(1)
//el evento tiene que ser tanto de lectura como de escritura
//hay que configurar para que los sockets no sean bloqueantes
//después hay que ir agregando los fds de las conexiones al socket
//se hace una lectura de los eventos, si está para lectura pues se procesa
//¿Cómo realizar la lectura de los eventos?
//una vez procesado y con el evento de escritura ok se envía la respuesta al cliente

