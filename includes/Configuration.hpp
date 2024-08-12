#pragma once
#include "./common.hpp"

class Configuration
{
	private:
		std::string _configFile;
		std::map<int, Server_config*> _serversConfig;
	public:
		Configuration(int argc, char **argv);
		~Configuration();
};
