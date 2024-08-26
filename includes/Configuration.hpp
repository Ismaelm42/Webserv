#pragma once
#include "./common.hpp"

class Configuration
{
	private:
		std::vector<Server_config*>::iterator _itServer;
		std::vector<Server_config*> _serversConfig;
		std::vector<std::string>::iterator _itToken;
		std::vector<std::string> _tokens;
		Server_config *_itConfig;
		std::string _fileName;
		std::ifstream _file;
		bool _inServerBlock;
		bool _inLocationBlock;
		int _loopCounter;
	public:
		Configuration(int argc, char **argv);
		~Configuration();
		std::string logError(std::string message);
		void checkFileOrDirectory(std::string &path, const std::string type);
		void createTokens(std::string &line);
		void initServerBlock();
		void setAllowedMethods();
		void setAutoindex();
		void setReturn();
		void handleLocations();
		void setListenPort();
		void setServerName();
		void setRootDirectory();
		void setIndexFiles();
		void setErrorPages();
		void setMaxBodySize();
		void endServerBlock();
		void checkFormatError();
		void handleConfigLine();
		void parsing();

		void printServerConfig();

};

