#pragma once
#include "./common.hpp"

class Configuration
{
	private:
		std::vector<Server_config*>::iterator _itServer;				// Iterador del container de Server_config
		std::vector<Server_config*> _serversConfig;						// Container de estructuras Server_config (common.hpp)
		std::vector<std::string>::iterator _itToken;					// Iterador del container de _itToken
		std::vector<std::string> _tokens;								// Container de strings tokens del archivo de configuración proporcionado
		Server_config *_itConfig;										// Puntero a una estructura Server_config (necesario para poder acceder a sus elementos)
		std::string _fileName;											// Argumento que representa el archivo de configuración
		std::ifstream _file;											// Stream de entrada para el archivo de configuración del servidor
		bool _inServerBlock;											// Indica si se está dentro de un bloque de server
		bool _inLocationBlock;											// Indica si se está dentro de un bloque de location
		int _loopCounter;												// Contador de líneas leídas
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
		void setListen();
		void setServerName();
		void setRootDirectory();
		void setIndexFiles();
		void setErrorPages();
		void setMaxBodySize();
		void endServerBlock();
		void checkServerBlockErrors();
		void handleConfigLine();
		void parsing();

		void printServerConfig();

};

