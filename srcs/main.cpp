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
