#include "../includes/lib.hpp"

int globalSigint = 0;

/*
    Gestiona la señal SIGINT para apagar el servidor.
*/
void	handleSignal(int signal)
{
	static_cast<void>(signal);
	globalSigint = 1;
	std::cout << Red << "\nServer shutdown" << Reset << std::endl;
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

// MEJORAS:

// El container de std::vector<struct epoll_event> log está cada vez que procesa eventos haciendo resize.
// Quizás sea interesante establecer un tamaño fijo e incrementarlo por dos si necesita más para trackear
// los eventos de las conexiones de los clientes.
// Tamaño de 100 inicial. En cuanto necesita 1 conexión mas: 100x2 y así sucesivamente¿?

// Echar un ojo al método de handle request y handle response ya que creo que es bastante malo
// para seguir tal como está implementado.
