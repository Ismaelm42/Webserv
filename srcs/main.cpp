#include "../includes/lib.hpp"

int globalSigint = 0;

/*
    Gestiona la señal SIGINT para apagar el servidor.
*/
void	handleSignal(int signal)
{
	static_cast<void>(signal);
	globalSigint = 1;
	std::cout << "\n" << Log << Red << "Server shutdown" << Reset << std::endl;
}

int main(int argc, char **argv)
{
	signal(SIGINT, handleSignal);
    try
    {
        Webserv webserv(argc, argv);
        webserv.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

// Gestionar CGI de forma aproximada ya que dependerá de cómo reciba los datos


// Es necesario gestionar primero un evento read PERO CREO que es importante
// que una vez read realizado el request se vuelva de nuevo al bucle para ver si
// es posible escribir sobre el fd

// Sólo está permitido leer o escribir del fd si éste ha pasado antes por epoll.
// Aunque si tiene los dos eventos listos no debería haber problema.


//Construír request con reserva de memoria

// URI MAXIMA
