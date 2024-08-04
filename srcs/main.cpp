#include "../includes/lib.hpp"

/** 
    Instancia de Server con puerto 8080 usado para inciar las pruebas y conectarse con la ip http://127.0.0.1:8080/
    Entiendo que este valor se deberá coger del archvio de configuración del valor liste: 8080;
    lanzamos try catch para capturar excepciones y mostrarlas por pantalla
 */
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
