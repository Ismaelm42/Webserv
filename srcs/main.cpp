#include "../includes/lib.hpp"

int globalSigint = 0;

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
