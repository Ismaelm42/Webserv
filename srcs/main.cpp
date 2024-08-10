#include "../includes/lib.hpp"

int globalSigint = 0;


void	handleSignal(int signal)
{
	static_cast<void>(signal);
	globalSigint = 1;
	std::cout << Red << "\nServer shutdown" << Reset << std::endl;
}

int main()
{
	signal(SIGINT, handleSignal);
    Webserv webserv; 
    try
    {
        webserv.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
