#pragma once
#include "./common.hpp"

class Configuration
{
	private:
		std::vector<Server_config*>::iterator _its;
		std::vector<Server_config*> _serversConfig;
		std::vector<std::string>::iterator _itt;
		std::vector<std::string> _tokens;
		std::string _fileName;
		std::ifstream _file;
		bool _inServerBlock;
		bool _inLocationBlock;
		int _loopCounter;
	public:
		Configuration(int argc, char **argv);
		~Configuration();
		std::string logError(std::string message);
		void splitWords(std::string &line);
		void initServerBlock();
		void handleLocations();
		void setListenPort();
		void setServerName();
		void setRootDirectory();
		void setIndexFile();
		void setErrorPage();
		void setMaxBodySize();
		void endServerBlock();
		void checkFormatError();
		void handleConfigLine();
		void parsing();

		void printServerConfig();

};

