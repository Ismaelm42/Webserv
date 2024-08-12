#include "../includes/lib.hpp"

Configuration::Configuration(int argc, char **argv)
{
	if (argc > 2)
		throw std::runtime_error("Error: Expected only one argument");
	argc == 1 ? _configFile = "./default.conf" : _configFile = argv[1];
}

Configuration::~Configuration()
{
	for (std::map<int, Server_config*>::iterator it = _serversConfig.begin(); it != _serversConfig.end(); it++)
		delete it->second;
}

