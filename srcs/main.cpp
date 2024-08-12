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
        Configuration config(argc, argv);
        Webserv webserv;
        webserv.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
