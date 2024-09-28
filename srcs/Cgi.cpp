#include "../includes/lib.hpp"

Cgi::Cgi(int fd, Request *request, Server_config *config)
:_fd(fd), _request(request), _config(config)
{
	_envp = NULL;
	_argv = NULL;
	_path = _config->root + _request->getPath();
    if (_request->getPath().rfind("/") != std::string::npos)
        _cmd = _request->getPath().substr(_request->getPath().rfind("/") + 1);
    else
		_cmd = _request->getPath();
}

Cgi::~Cgi()
{
	if (_envp != NULL)
	{
		for (int i = 0; _envp[i] != NULL; i++)
			free(_envp[i]);
		free(_envp);
	}
	if (_argv != NULL)
	{
		for (int i = 0; _argv[i] != NULL; i++)
			free(_argv[i]);
		free(_argv);
	}
}

void Cgi::setEnvironment()
{
	std::string envp[13];

	envp[0] = "REQUEST_METHOD=" + _request->getMethodStr(); 
	envp[1] = "SCRIPT_NAME" + _request->getPath();
	envp[2] = "PATH_INFO=" + _request->getPath();
	envp[3] = "PATH_TRANSLATED=" + _request->getPath();
	envp[4] = "QUERY_STRING=" + _request->getQuery();
	envp[5] = "CONTENT_LENGTH=" + _request->getBody().size();
	envp[6] = "CONTENT_TYPE=" + _request->getHeader("content-type");
	envp[7] = "HTTP_USER_AGENT=" + _request->getHeader("user-agent");
	envp[8] = "HTTP_COOKIE=" + _request->getHeader("cookie");
	envp[9] = "SERVER_NAME=" + _request->getServerName();
	envp[10] = "SERVER_PORT=" + toStr(_request->getPort());
	envp[11] = "SERVER_PROTOCOL=HTTP/1.1";

	_envp = (char **)calloc(sizeof(char *), 13);
	for (int i = 0; i < 12; i++)
		_envp[i] = strdup(envp[i].c_str());
	_envp[12] = NULL; 
}

int Cgi::checkCmdAndPath()
{

	int ret = checkFileOrDirectory(_path, "file");
	if (ret != 0)
		return ret;
	return 0;
}

void Cgi::setArguments()
{
	_argv = (char **)calloc(sizeof(char *), 2);
	_argv[0] = strdup(_cmd.c_str());
	_argv[1] = NULL;
}

//Pruebas de Alfonso:
// para revertir comentar todo hasta la línea "fin de las pruebas" comentada más abajo y descomentar la parte inferior

int Cgi::executeCgi(std::string &output, std::string &body)
{
    // Verificar si el archivo existe
    if (int ret = checkFileOrDirectory(_path, "file") != 0)
        return ret;

    setEnvironment();  // Configurar las variables de entorno
    setArguments();    // Configurar los argumentos del script CGI

    // Crear los pipes
    if (pipe(_pipeFd) == -1)  // Pipe para pasar el cuerpo de la solicitud (stdin)
        return 500;
    if (pipe(_responsePipeFd) == -1)  // Pipe para la salida del CGI (stdout)
        return 500;

    _pid = fork();

    if (_pid == -1) {
        return 500;  // Error en el fork
    }

    if (_pid == 0) {  // Proceso hijo (script CGI)
        // Cerrar el extremo de escritura del pipe de la solicitud
        close(_pipeFd[1]);
        // Redirigir stdin al extremo de lectura del pipe de solicitud
        if (dup2(_pipeFd[0], STDIN_FILENO) < 0)
            exit(EXIT_FAILURE);
        close(_pipeFd[0]);  // Cerrar el extremo de lectura después de duplicarlo

        // Cerrar el extremo de lectura del pipe de salida
        close(_responsePipeFd[0]);
        // Redirigir stdout al extremo de escritura del pipe de salida
        if (dup2(_responsePipeFd[1], STDOUT_FILENO) < 0)
            exit(EXIT_FAILURE);
        close(_responsePipeFd[1]);  // Cerrar el extremo de escritura después de duplicarlo

        // Ejecutar el script CGI
        if (execve(_path.c_str(), _argv, _envp) < 0)
            exit(EXIT_FAILURE);  // Terminar el proceso hijo si execve falla
    }

    // Proceso padre (servidor)
    // Cerrar el extremo de lectura del pipe de solicitud
    close(_pipeFd[0]);

    // Escribir los datos del cuerpo de la solicitud en el pipe
    FILE* pipeStream = fdopen(_pipeFd[1], "w");
    if (pipeStream == NULL) {
        std::cerr << "Error al abrir el stream del pipe: " << strerror(errno) << std::endl;
        return 500;
    }

    // Escribir los datos del body al CGI
    size_t written = fwrite(body.c_str(), 1, body.size(), pipeStream);
    if (written < body.size()) {
        std::cerr << "Error: no se pudo escribir todo el cuerpo en el pipe" << std::endl;
        fclose(pipeStream);
        return 500;
    }

    // Cerrar el stream para que el hijo sepa que ya no hay más datos
    fclose(pipeStream);

    // Cerrar el extremo de escritura del pipe de salida del CGI (ya no lo necesitamos)
    close(_responsePipeFd[1]);

    // Leer la respuesta del CGI
    FILE *readStream = fdopen(_responsePipeFd[0], "r");
    if (!readStream) {
        std::cerr << "Error al abrir el stream de lectura: " << strerror(errno) << std::endl;
        return 500;
    }

    char line[1024];
    std::stringstream buffer;
    while (fgets(line, sizeof(line), readStream)) {
        buffer << line;
    }

    if (ferror(readStream)) {
        std::cerr << "Error al leer desde el pipe" << std::endl;
        fclose(readStream);
        return 500;
    }

    fclose(readStream);  // Cerrar el stream de lectura
    close(_responsePipeFd[0]);  // Cerrar el pipe de lectura

    // Esperar a que el proceso hijo termine
    int status;
    waitpid(_pid, &status, 0);
    if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
        return 500;
    }

    // Devolver la salida generada por el CGI
    output = buffer.str();
    return 0;
}


// int Cgi::executeCgi(std::string &output, std::string &body)
// {
//     // Verificar si el archivo existe
//     if (int ret = checkFileOrDirectory(_path, "file") != 0)
//         return ret;

//     setEnvironment();  // Configura las variables de entorno
//     setArguments();    // Configura los argumentos del script CGI

//     // Crear los pipes
//     if (pipe(_pipeFd) == -1)  								// Pipe para pasar el cuerpo de la solicitud (stdin)
//         return 500;
//     if (pipe(_responsePipeFd) == -1)  						// Pipe para la salida del CGI (stdout)
//         return 500;
//     _pid = fork();
//     if (_pid == -1)
//         return 500;  										// Error en el fork
//     if (_pid == 0) {  
        
//         close(_pipeFd[1]); 									// Cerramos el extremo de escritura del primer pipe de la solicitud
//         if (dup2(_pipeFd[0], STDIN_FILENO) < 0) 			// Redirigimos stdin al extremo de lectura del pipe de solicitud
//             exit(EXIT_FAILURE);
//         close(_pipeFd[0]);  								// Cerramos el extremo de lectura del primer pipe después de duplicarlo      
//         close(_responsePipeFd[0]);							// Cerramos el extremo de lectura del segundo pipe de salida    
//         if (dup2(_responsePipeFd[1], STDOUT_FILENO) < 0) 	// Redirigimos stdout al extremo de escritura del pipe de salida
//             exit(EXIT_FAILURE);
//         close(_responsePipeFd[1]); 							// Cerramos el extremo de escritura después de duplicarlo      
//         if (execve(_path.c_str(), _argv, _envp) < 0)		// Ejecutar el script CGI
//             exit(EXIT_FAILURE);  							// Terminar el proceso hijo si execve falla
//     }
//     // Proceso padre (servidor)
//     close(_pipeFd[0]);						    			// Cerramos el extremo de lectura del pipe de solicitud  
//     FILE* requestStream = fdopen(_pipeFd[1], "w");				// Escribir los datos del cuerpo de la solicitud en el primer pipe
//     if (requestStream == NULL)									// Verificar si se pudo abrir el stream del pipe
//         return 500;							
//     size_t written = fwrite(body.c_str(), 1, body.size(), requestStream);    // Escribir los datos del body al CGI
    
// 	// En caso de problemas a la hora de pasar requests grandes o porblema con codificación de archivos habría que revisar este punto 
// 	if (written < body.size()) {								// Comprobamos si se escribieron todos los datos
//         fclose(requestStream);
//         return 500;
//     }
//     fclose(requestStream);     									// Cerramos el stream para que el hijo sepa que ya no hay más datos  
//     close(_responsePipeFd[1]);									// Cerramos el extremo de escritura del pipe de salida del CGI (ya no lo necesitamos)   
//     FILE *responseStream = fdopen(_responsePipeFd[0], "r");		// Leer la respuesta del CGI
//     if (!responseStream) {
//         std::cerr << "Error al abrir el stream de lectura: " << strerror(errno) << std::endl;
//         return 500;
//     }

//     char line[1024];
//     std::stringstream buffer;								// Buffer para almacenar la respuesta
//     while (fgets(line, sizeof(line), responseStream)) {		// mete la respuesta línea por línea y almacenarla en un buffer	
//         buffer << line;										// Almacenar la línea en el buffer
//     }
//     if (ferror(responseStream)) {							// Verificar si hubo un error al leer la respuesta
//         fclose(responseStream);
//         return 500;
//     }
//     fclose(responseStream);  								// Cerramos el stream de lectura
//     close(_responsePipeFd[0]);  							// Cerramos el pipe de lectura   
//     int status;												// Variable para almacenar el estado de salida del hijo				
//     waitpid(_pid, &status, 0);
//     if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
//         return 500;
//     }
//     output = buffer.str();									// Asigna a output la respuesta generada por el CGI
//     return 0;
// }

	///////////////////////                                 fin de las pruebas         ////////////////////////////////////////

	// Código de Ismael para revertir 

	// void Cgi::childProcess()
	// {
	// 	if (dup2(_pipeFd[0], STDIN_FILENO) < 0 || dup2(_pipeFd[1], STDOUT_FILENO) < 0)
	// 		exit(-1);
	// 	close(_pipeFd[0]);
	// 	close(_pipeFd[1]);
	// 	if (execve(_path.c_str(), _argv, _envp) < 0)
	// 		exit(EXIT_FAILURE);
	// }

	/*
		Faltaría comprobar que tenemos permisos para ejecutar el ejecutable, es decir,
		/bin/bash o /usr/bin/python3 o el path que sea. Aunque en principio no
		debería servir de mucho. Se podría escribir cualquier cosa y funcionaría.
		Echarle un vistazo.
	*/
	// int Cgi::executeCgi(std::string &output,  std::string &body)
	// {
	// 	(void)body;
	// 	if (int ret = checkFileOrDirectory(_path, "file") != 0)
	// 		return ret;
	// 	setEnvironment();
	// 	setArguments();
	// 	if (pipe(_pipeFd) == -1)
	// 		return 500;
	// 	_pid = fork();
	// 	if (_pid == 0)
	// 		childProcess();
	// 	int status;
	// 	waitpid(_pid, &status, 0);
	// 	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
	// 		if (WEXITSTATUS(status) == 255 || WEXITSTATUS(status) == 1)
	// 			return 500;
	// 	close(_pipeFd[1]);
	// 	FILE *responsestream = fdopen(_pipeFd[0], "r");
	// 	if (!responsestream)
	// 		return 500;
	//     while (fgets(line, sizeof(line), responsestream))
	// 		buffer << line;
	// 	if (ferror(responsestream))
	// 		return 500;
	// 	fclose(responsestream);
	// 	close (_pipeFd[0]);
	// 	output = buffer.str();
	// 	return 0;
	// }
