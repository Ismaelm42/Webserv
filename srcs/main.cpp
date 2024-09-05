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

// Incluir CGI en el parsing de location
// Línea similar a esta: cgi .py /cgi/python

// Gestionar CGI de forma aproximada ya que dependerá de cómo reciba los datos
