#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <sys/wait.h>
#include <sstream>

#define PORT 8080

void handle_request(int new_socket);

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Crear socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forzar la vinculación del socket al puerto 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Vincular socket al puerto 8080
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Escuchar conexiones entrantes
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Servidor escuchando en el puerto " << PORT << std::endl;

    while (true) {
        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        handle_request(new_socket);
        close(new_socket);
    }

    return 0;
}

void handle_request(int new_socket) {
    const int buffer_size = 1024;
    char buffer[buffer_size] = {0};
    read(new_socket, buffer, buffer_size);

    std::string request(buffer);
    std::cout << "Solicitud recibida: \n" << request << std::endl;

    // Parsear la solicitud para obtener el script CGI
    std::string method, path;
    std::istringstream request_stream(request);
    request_stream >> method >> path;

    // Simple check for GET method
    if (method != "GET") {
        std::string response = "HTTP/1.1 405 Method Not Allowed\r\n\r\n";
        send(new_socket, response.c_str(), response.size(), 0);
        return;
    }

    // Remover el prefijo '/' de la ruta
    if (path[0] == '/') {
        path = path.substr(1);
    }

    // Ejecutar el script CGI
    int cgi_output[2];
    pipe(cgi_output);
    pid_t pid = fork();

    if (pid == 0) {
        // Proceso hijo: ejecutar el script CGI
        dup2(cgi_output[1], STDOUT_FILENO);
        close(cgi_output[0]);
        close(cgi_output[1]);
        execl(path.c_str(), path.c_str(), NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    } else {
        // Proceso padre: capturar la salida del script CGI
        close(cgi_output[1]);
        waitpid(pid, NULL, 0);

        char cgi_buffer[buffer_size];
        int bytes_read = read(cgi_output[0], cgi_buffer, buffer_size);
        close(cgi_output[0]);

        // Enviar la respuesta al cliente
        std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        send(new_socket, response.c_str(), response.size(), 0);
        send(new_socket, cgi_buffer, bytes_read, 0);
    }
}

