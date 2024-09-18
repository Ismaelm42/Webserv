#include "../includes/lib.hpp"

Cgi::Cgi(Request *request, struct Epoll_events *events)
:_request(request), _events(events)
{
	_envp = NULL;
	_argv = NULL;
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
	envp[5] = "CONTENT_LENGTH=";
	envp[6] = "CONTENT_TYPE=";
	envp[7] = "HTTP_USER_AGENT=";
	envp[8] = "HTTP_COOKIE=";
	envp[9] = "SERVER_NAME=";
	envp[10] = "SERVER_PORT=";
	envp[11] = "SERVER_PROTOCOL=HTTP/1.1";

	_envp = (char **)calloc(sizeof(char *), 13);
	for (int i = 0; i < 12; i++)
		_envp[i] = strdup(envp[i].c_str());
	_envp[12] = NULL; 
}

//Para set Arguments estaría bien tener un puntero a la clase Config. Se puede enviar perfectamente a Request.
void Cgi::setArguments()
{
	_argv = (char **)calloc(sizeof(char *), 2);
	_argv[0] = strdup("./sum.sh");
	_argv[1] = NULL;
}

// const char *pathname = "/bin/ls";
// // Argumentos del programa
// char *const argv[] = {(char *)"ls", (char *)"-l", NULL};
// // Variables de entorno
// char *const envp[] = {(char *)"PATH=/usr/bin", NULL};

void Cgi::childProcess()
{
	if (dup2(_pipeFd[0], STDIN_FILENO) < 0)
		exit(-2);
	close(_pipeFd[0]);
	if (dup2(this->_pipeFd[1], STDOUT_FILENO) < 0)
		exit(-3);
	close(_pipeFd[1]);
	if (execve("./root/cgi/sum.sh", _argv, _envp) < 0)
		exit(EXIT_FAILURE);
}

void Cgi::executeCgi()
{
	setEnvironment();
	setArguments();
	if (pipe(_pipeFd) == -1)
		throw std::runtime_error("Error: pipe: " + std::string(strerror(errno)));
	_pid = fork();
	if (_pid == 0)
		childProcess();
	else
	{
		//Quizás aquí enviar los datos a response para que se encargue de realizar la respuesta
		int status;
		waitpid(_pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
            throw std::runtime_error("Error: Fallo child process");
		std::cout << "FDIN = " << _pipeFd[0] << std::endl;
		std::cout << "FDOUT = " << _pipeFd[1] << std::endl << std::endl;
		addEvent(_pipeFd[0], _events);
		addEvent(_pipeFd[1], _events);
		// Crear un mapa en _events con estos dos fds.
		// El primero sería fdin y el segundo fdout
		// En cuanto lo procese
	}
}


// En response, procesar la lectura y una vez la lectura realizada procesar la respuesta?
