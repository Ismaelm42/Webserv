#pragma once
#include "./common.hpp"

class Configuration
{
	private:
		int _loopCounter;
		bool _inLocationBlock;
		bool _inServerBlock;
		std::ifstream _file;
		std::string _fileName;
		std::vector<std::string> _tokens;
		std::vector<std::string>::iterator _itToken;

		std::string logError(std::string message);
		void createTokens(std::string &line);
		void initServerBlock();
		void setAllowedMethods();
		void setAutoindex();
		void setRedir();
		void setCgi();
		void handleLocations();
		void setListen();
		void setServerName();
		void setRootDirectory();
		void setIndexFiles(std::string path, std::vector<std::string> &container);
		void setErrorPages();
		void setMaxBodySize(size_t &body_size);
		void setAuthBasic();
		void setAuthBasicUserFile();
		void endServerBlock();
		void checkServerBlockErrors();
		void handleConfigLine();
		void printServerConfig();

	public:
		Server_config *_itConfig;
		std::vector<Server_config*> _serversConfig;
		std::vector<Server_config*>::iterator _itServer;

		void checkFileOrDirectory(std::string &path, const std::string type);
		Configuration(int argc, char **argv);
		~Configuration();
		void parsing();
};
