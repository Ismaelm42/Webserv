#include "../includes/lib.hpp"

Cgi::Cgi(Request *request, Epoll_events *events)
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
	envp[9] = "SERVER_NAME=" + _request->getServerName();
	envp[10] = "SERVER_PORT=" + _request->getPort();
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
	if (dup2(_pipeFd[0], STDIN_FILENO) < 0 || dup2(_pipeFd[1], STDOUT_FILENO) < 0)
		exit(-1);
	close(_pipeFd[0]);
	close(_pipeFd[1]);
	if (execve("./root/cgi/sum.sh", _argv, _envp) < 0)
		exit(EXIT_FAILURE);
}

void Cgi::executeCgi(std::pair<int, int> &cgiFd)
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
		int status;
		waitpid(_pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		{
			if (WEXITSTATUS(status) == 255)
            	throw std::runtime_error("Error: dup2 failed in child process");
			if (WEXITSTATUS(status) == 1)
				throw std::runtime_error("Error: execve script failed");
		}
		if (fcntl(_pipeFd[0], F_SETFL, O_NONBLOCK) < 0 || fcntl(_pipeFd[1], F_SETFL, O_NONBLOCK) < 0)
        	throw std::runtime_error("Error: fcntl: " + std::string(strerror(errno)));
		addEvent(_pipeFd[0], _events);
		addEvent(_pipeFd[1], _events);
		cgiFd.first = _pipeFd[0];
		cgiFd.second = _pipeFd[1];
	}
}
