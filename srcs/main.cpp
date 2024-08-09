#include "../includes/lib.hpp"

int main()
{
    Webserv webserv; 
    try
    {
        webserv.initializeServers();
        webserv.run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
