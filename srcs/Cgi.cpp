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

void Cgi::childProcess()
{
	if (dup2(_pipeFd[0], STDIN_FILENO) < 0 || dup2(_pipeFd[1], STDOUT_FILENO) < 0)
		exit(-1);
	close(_pipeFd[0]);
	close(_pipeFd[1]);
	if (execve(_path.c_str(), _argv, _envp) < 0)
		exit(EXIT_FAILURE);
}

/*
	Faltaría comprobar que tenemos permisos para ejecutar el ejecutable, es decir,
	/bin/bash o /usr/bin/python3 o el path que sea. Aunque en principio no
	debería servir de mucho. Se podría escribir cualquier cosa y funcionaría.
	Echarle un vistazo.
*/
int Cgi::executeCgi(std::string &output)
{
	if (int ret = checkFileOrDirectory(_path, "file") != 0)
		return ret;
	setEnvironment();
	setArguments();
	if (pipe(_pipeFd) == -1)
		return 500;
	_pid = fork();
	if (_pid == 0)
		childProcess();
	int status;
	waitpid(_pid, &status, 0);
	if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		if (WEXITSTATUS(status) == 255 || WEXITSTATUS(status) == 1)
			return 500;
	close(_pipeFd[1]);
	FILE *readstream = fdopen(_pipeFd[0], "r");
	if (!readstream)
		return 500;
    while (fgets(line, sizeof(line), readstream))
		buffer << line;
	if (ferror(readstream))
		return 500;
	fclose(readstream);
	close (_pipeFd[0]);
	output = buffer.str();
	return 0;
}
