#include "../includes/lib.hpp"

/*
	Constructor que inicializa una instancia de la clase Configuration. 
	Verifica el número de argumentos recibidos, asegurándose de que se proporcione 
	exactamente un archivo de configuración. Si no se especifica un archivo, 
	utiliza un archivo de configuración predeterminado. A continuación, comprueba 
	si el archivo existe y es accesible. Si no lo es, lanza una excepción. 
	Finalmente, inicializa variables internas que gestionan el número de línea 
	y en qué bloque se está leyendo del archivo (si estamos dentro de un bloque server
	o en una directiva de location).
*/
Configuration::Configuration(int argc, char **argv)
{
	if (argc > 2)
		throw std::runtime_error("Error: Invalid number of arguments. You must specify exactly one configuration file.");
	argc == 1 ? _fileName = "./default.conf" : _fileName = argv[1];
	checkFileOrDirectory(_fileName, "file");
	_file.open(_fileName.c_str());
	if (!_file.is_open())
		throw std::runtime_error("Error: " + _fileName + ": " + std::string(strerror(errno)));
	_inServerBlock = false;
	_inLocationBlock = false;
	_loopCounter = 0;
}

/*
	Destructor que se encarga de liberar los recursos utilizados por la instancia de Configuration.
	Libera la memoria asignada a los objetos Server_config almacenados en el vector _serversConfig,
	y cierra el archivo de configuración si aún está abierto.
*/ 
Configuration::~Configuration()
{
	for (std::vector<Server_config*>::iterator it = _serversConfig.begin(); it != _serversConfig.end(); it++)
		delete *it;
	if (_file.is_open())
		_file.close();
}

/*
	Genera un mensaje de error detallado, incluyendo la información del archivo de configuración 
	y la línea donde ocurrió el error.
*/
std::string Configuration::logError(std::string message)
{
	std::ostringstream ss;
	ss << message << " in " << _fileName << ":" << _loopCounter;
	std::string error_message = ss.str();
	return error_message;
}
/*
	Verifica si una ruta específica corresponde a un archivo o directorio y si es accesible.
	Si la ruta no cumple con los criterios o no tiene permisos de lectura, lanza una excepción
	con un mensaje de error.
*/
void Configuration::checkFileOrDirectory(std::string &path, const std::string type)
{
	struct stat stat_buffer;

	if (stat(path.c_str(), &stat_buffer) < 0)
	       throw std::runtime_error(logError("Error: \"" + path + "\": " + std::string(strerror(errno))));
	if (type == "dir" && !S_ISDIR(stat_buffer.st_mode))
		throw std::runtime_error(logError("Error: \"" + path + "\": is not a directory"));
	if (type == "file" && S_ISDIR(stat_buffer.st_mode))
		throw std::runtime_error(logError("Error: \"" + *_itToken + "\": is a directory"));
	if (!(stat_buffer.st_mode & S_IRUSR))
		throw std::runtime_error(logError("Error: \"" + path + "\": permission denied"));
}

/*
	Divide una línea de texto en tokens separados por espacios.
	Si se encuentra un símbolo de comentario ('#'), elimina todo el contenido posterior
	y agrega el token resultante al vector _tokens, terminando el proceso.
	Los tokens válidos (no vacíos) anteriores al comentario se almacenan en el vector 
	_tokens.
*/
void Configuration::createTokens(std::string& line)
{
	std::istringstream stream(line);
	std::string token;
	size_t cPos;

	while (stream >> token)
	{
		cPos = token.find('#');
		if (cPos != std::string::npos)
		{
			token.erase(cPos);
			if (!token.empty())
				_tokens.push_back(token);
			break ;
		}
        _tokens.push_back(token);
	}
}

/*
	Inicializa un bloque de configuración del servidor a partir de los tokens proporcionados.
	Verifica que la directiva "server" esté correctamente configurada con exactamente dos tokens, 
	donde el primero debe ser "server" y el segundo debe ser una llave de apertura ("{"). 
	Si la configuración es válida, establece el indicador _inServerBlock en true, crea una nueva 
	instancia de Server_config, la agrega al vector _serversConfig, y actualiza los iteradores 
	_itServer e _itConfig para apuntar a la nueva configuración del servidor.
*/
void Configuration::initServerBlock()
{
	if (_tokens.size() != 2)
		throw std::runtime_error(logError("Error: syntax error in \"server\" directive"));
	if (_tokens.size() == 2)
	{
		if (_tokens[0] != "server")
			throw std::runtime_error(logError("Error: unexpected \"" + _tokens[0] + "\""));
		if (_tokens[1] != "{")
			throw std::runtime_error(logError("Error: unexpected \"" + _tokens[1] + "\""));
		_inServerBlock = true;
		Server_config* config = new Server_config;
		_serversConfig.push_back(config);
		_itServer = _serversConfig.end() - 1;
		_itConfig = *_itServer;
	}
}

/*
	Establece los métodos HTTP permitidos para la configuración actual de la ubicación.
	Itera sobre los tokens agregando cada método permitido (debe ser "GET", "POST" o "DELETE")
	al conjunto de métodos permitidos de la última ubicación en la configuración.
	Verifica que el último token termine con un punto y coma (";") y asegura que solo se acepten
	los métodos válidos. Lanza una excepción si se encuentra un método inesperado o un error
	de sintaxis en la directiva.
*/
void Configuration::setAllowedMethods()
{
	_itToken++;
	for (; _itToken != _tokens.end(); _itToken++)
	{
		std::string str = *_itToken;
		if (_itToken == _tokens.end() - 1)
		{
			if (str.find(";") != str.size() - 1)
				throw std::runtime_error(logError("Error: syntax error in \"allow_methods\" directive"));
			str.erase(str.size() - 1);
		}
		if (str != "GET" && str != "POST" && str != "DELETE")
			throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"allow_methods\" directive"));
		_itConfig->locations.back().methods.insert(str);
	}
}

/*
	Configura el índice automático para la última ubicación en la configuración.
	Verifica que la directiva "autoindex" tenga exactamente dos argumentos, donde el segundo 
	debe ser "on" o "off". Asegura que el argumento termine con un punto y coma (";") y 
	remueve este carácter antes de realizar la verificación. Si el argumento es válido, 
	configura el índice automático de la última ubicación en la configuración. Lanza una 
	excepción si el número de argumentos es incorrecto o si el valor proporcionado es inesperado.
*/
void Configuration::setAutoindex()
{
	if (_tokens.end() - _tokens.begin() != 2)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"autoindex\" directive"));
	std::string str = *(_tokens.begin() + 1);
	if (str.find(";") != str.size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"autoindex\" directive"));
	str.erase(str.size() - 1);
	if (str != "on" && str != "off")
		throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"autoindex\" directive"));
	if (str == "on")
		_itConfig->locations.back().autoindex = true;
}

/*
	Configura la directiva de redirección para la última ubicación en la configuración.
	Verifica que la directiva "return" tenga dos o tres argumentos. Si tiene dos, asegura que el 
	segundo argumento termine con un punto y coma (";") y remueve este carácter antes de la 
	validación. Verifica que el código de error sea exactamente de 3 dígitos, que esté dentro 
	del rango válido (100-599) y que sea numérico. Si el número de argumentos es tres, el tercer 
	argumento debe ser una URL de redirección que también debe terminar con un punto y coma. 
	La configuración del redireccionamiento se actualiza en la última ubicación de la configuración. 
	Lanza excepciones en caso de errores de sintaxis o valores inválidos.
*/
void Configuration::setReturn()
{
	if (_tokens.end() - _tokens.begin() != 2 && _tokens.end() - _tokens.begin() != 3)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"return\" directive"));
	std::string str = *(_itToken + 1);
	if (_tokens.end() - _tokens.begin() == 2)
	{
		if (str.find(";") != str.size() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"return\" directive"));
		str.erase(str.size() - 1);
	}
	if (str.size() != 3)
		throw std::runtime_error(logError("Error: error code must be exactly 3 digits in \"return\" directive"));
	if (!std::isdigit(str[0]) || !std::isdigit(str[1]) || !std::isdigit(str[2]))
		throw std::runtime_error(logError("Error: unexpected \"" + str + "\" in \"return\" directive"));
	int nbr = std::atoi(str.c_str());
	if (nbr < 100 || nbr > 599)
		throw std::runtime_error(logError("Error: error code is out of range: \"" + str + "\" in \"return\" directive"));
	_itConfig->locations.back().redir.first = nbr;
	if (_tokens.end() - _tokens.begin() == 3)
	{
		str = *(_itToken + 2);
		if (str.rfind(";") != str.size() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"return\" directive"));
		str.erase(str.size() - 1);
		_itConfig->locations.back().redir.second = str;
	}
}

/*
	Procesa las directivas dentro de un bloque de ubicación en la configuración.
	Primero, verifica que la directiva "root" esté configurada antes de definir ubicaciones. Si no 
	estamos dentro de un bloque de ubicación, valida que la directiva "location" tenga exactamente 
	tres argumentos y que la ubicación especificada sea un directorio válido. Luego, añade la 
	configuración de la ubicación y establece en true que estamos dentro de un bloque de ubicación. Si ya 
	estamos en un bloque de ubicación, maneja directivas específicas como "allow_methods", 
	"autoindex", y "return". Si se encuentra un cierre de bloque "}" y no hay más tokens, sale del 
	bloque de ubicación. Lanza excepciones en caso de errores de sintaxis o configuraciones inesperadas.
*/
void Configuration::handleLocations()
{
	if (_itConfig->root == "")
		throw std::runtime_error(logError("Error: \"root\" directive is required to set \"location\" directive"));
	_itToken = _tokens.begin();
	if (_inLocationBlock == false)
	{
		if (_tokens.end() - _tokens.begin() != 3)
			throw std::runtime_error(logError("Error: invalid number of arguments in \"location\" directive"));
		*(_itToken + 1) = _itConfig->root + *(_itToken + 1);
		checkFileOrDirectory(*(_itToken + 1), "dir");
		if (*(_itToken + 2) != "{")
			throw std::runtime_error(logError("Error: unexpected \"" + *(_itToken + 2) + "\". open curly brace missing in \"location\" directive"));
		Location_config config;
		_itConfig->locations.push_back(config);
		_itConfig->locations.back().location = *(_itToken + 1);
		_inLocationBlock = true;
	}
	else if (*_itToken == "allow_methods")
		setAllowedMethods();
	else if (*_itToken == "autoindex")
		setAutoindex();
	else if (*_itToken == "return" && !_itConfig->locations.back().redir.first)
		setReturn();
	else if (*_itToken == "}" && (_itToken + 1) == _tokens.end())
		_inLocationBlock = false;
	else
		throw std::runtime_error(logError("Error: unexpected \"" + *(_itToken) + "\" in \"location\" directive"));
}

/*
	Configura la directiva "listen" del servidor, que especifica la dirección IP y el puerto en el 
	que el servidor debe escuchar. Primero, valida que la directiva tenga el formato correcto, 
	incluyendo la presencia de un carácter de separación ":" y un carácter de finalización ";". 
	Determina la dirección IP y el puerto a partir del token, utilizando valores predeterminados si 
	es necesario. Verifica que la dirección IP sea válida y que el puerto esté en el rango permitido 
	(0-65535). Finalmente, agrega el par IP-puerto a la configuración del servidor.
*/
void Configuration::setListen()
{
	size_t cPos = _itToken->find(":");
	size_t c2Pos = _itToken->find(";");
	if (_itToken == _tokens.end() || c2Pos == 0 || _itToken != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"listen\" directive"));
	if (c2Pos != _itToken->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"listen\" directive"));
	std::string host = (cPos == std::string::npos) ? "0.0.0.0" : _itToken->substr(0, cPos);
	std::string port = (cPos == std::string::npos) ? *_itToken : _itToken->substr(cPos + 1, 6);
	struct sockaddr_in host_check;
	if (inet_pton(AF_INET, host.c_str(), &(host_check.sin_addr)) != 1)
		throw std::runtime_error(logError("Error: ip adress is not valid \"" + host + "\" in \"listen\" directive"));
	port.erase(port.size() - 1);
	for (int i = 0; port[i] != 0; i++)
		if (port[i] < '0' || port[i] > '9')
			throw std::runtime_error(logError("Error: unexpected \"" + port + "\""));
	int port_check = std::atoi(port.c_str());
	if (port_check < 0 || port_check > 65536)
		throw std::runtime_error(logError("Error: port number is out of range: \"" + port + "\" in \"listen\" directive"));
	std::pair<std::string, int> pair = std::make_pair(host, std::atoi(port.c_str()));
	_itConfig->ip_port.push_back(pair);
}

/*
	Valida y procesa los nombres del servidor en la configuración.
	Verifica si el número de argumentos es correcto y si hay errores de sintaxis.
	Procesa cada token que representa un nombre de servidor, asegurándose de que el token finaliza en un punto y coma.
*/
void Configuration::setServerName()
{
	if (_itToken == _tokens.end())
		throw std::runtime_error(logError("Error: invalid number of arguments in \"server_name\" directive"));
	for (; _itToken != _tokens.end(); _itToken++)
	{
		size_t cPos = _itToken->find(";");
		if (cPos != std::string::npos && (_itToken + 1 != _tokens.end() || cPos != _itToken->size() - 1 || cPos == 0))
			throw std::runtime_error(logError("Error: syntax error in \"server_name\" directive"));
		if (cPos == std::string::npos && _itToken == _tokens.end() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"server_name\" directive"));
		if (cPos != std::string::npos)
		{
			_itToken->erase(_itToken->size() - 1);
			_itConfig->server_names.push_back(*_itToken);
		}
		else
			_itConfig->server_names.push_back(*_itToken);
	}
}

/*
	Establece el directorio raíz en la configuración.
	La función verifica que el número de argumentos sea el esperado y que no haya errores de sintaxis.
	Elimina el punto y coma final del token y asegura que el directorio raíz no termine en una barra inclinada.
	Verifica que el directorio raíz exista y se pueda acceder, y luego lo asigna a la configuración.
*/
void Configuration::setRootDirectory()
{
	size_t cPos = _itToken->find(";");
	if (_itToken == _tokens.end() || cPos == 0)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"root\" directive"));
	if (_itToken != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: too many arguments in \"root\" directive"));
	if (cPos != _itToken->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"root\" directive"));
	_itToken->erase(_itToken->size() - 1);
	while (_itToken->rfind("/") == _itToken->size() - 1)
		_itToken->erase(_itToken->size() - 1);
	checkFileOrDirectory(*_itToken, "dir");
	_itConfig->root = *_itToken;
}

/*
	Establece los archivos de índice en la configuración.
	La función asegura que se haya definido una directiva "root" y verifica los argumentos.
	Procesa cada token como un archivo de índice, ajusta su ruta en función del directorio raíz y verifica si el archivo existe.
*/
void Configuration::setIndexFiles()
{
	if (_itConfig->root == "")
		throw std::runtime_error(logError("Error: \"root\" directive is required to set \"index\" directive"));
	if (_itToken == _tokens.end())
		throw std::runtime_error(logError("Error: invalid number of arguments in \"index\" directive"));
	for (; _itToken != _tokens.end(); _itToken++)
	{
		size_t cPos = _itToken->find(";");
		if (cPos != std::string::npos && (_itToken + 1 != _tokens.end() || cPos != _itToken->size() - 1 || cPos == 0))
			throw std::runtime_error(logError("Error: syntax error in \"index\" directive"));
		if (cPos == std::string::npos && _itToken == _tokens.end() - 1)
			throw std::runtime_error(logError("Error: syntax error in \"index\" directive"));
		if (cPos != std::string::npos)
			_itToken->erase(_itToken->size() - 1);
		if (_itToken->find('/') != 0)
			*_itToken = "/" + *_itToken;
		*_itToken = _itConfig->root + *_itToken;
		checkFileOrDirectory(*_itToken, "file");
		_itConfig->index.push_back(*_itToken);
	}
}

/*
	Establece las páginas de error en la configuración. La función asegura que se haya definido una directiva
	"root" antes de configurar las páginas de error.
	Valida la cantidad de argumentos y la sintaxis de la directiva "error_page".
	Procesa el último token como la ruta del archivo de página de error, asegurándose de que esté correctamente formateado
	y que el punto y coma final se elimine.
	Verifica que la ruta del archivo exista y sea accesible. Itera sobre los códigos de error especificados en los tokens,
	validando su formato y rango. Los códigos de error deben tener exactamente 3 dígitos o 2 dígitos y la x como wildcard
	en la tercera posición. Actualiza el mapa de páginas de error en la configuración con la ruta del archivo para cada
	código de error especificado.
*/
void Configuration::setErrorPages()
{
	if (_itConfig->root == "")
		throw std::runtime_error(logError("Error: \"root\" directive is required to set \"error_page\" directive"));
	if (_itToken == _tokens.end() || _tokens.begin() + 2 == _tokens.end())
		throw std::runtime_error(logError("Error: invalid number of arguments in \"error_page\" directive"));
	std::string file = *(_tokens.end() - 1);
	size_t cPos = file.find(";");
	if (cPos != file.size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"error_page\" directive"));
	file.erase(file.size() - 1);
	if (file.find('/') != 0)
		file = "/" + file;
	file = _itConfig->root + file;
	checkFileOrDirectory(file, "file");
	for (; _itToken != _tokens.end() - 1; _itToken++)
	{
		std::string code = *_itToken;
		if (code.size() != 3)
			throw std::runtime_error(logError("Error: error code must be exactly 3 digits in \"error_page\" directive"));
		if (!std::isdigit(code[0]) || !std::isdigit(code[1]) || (!std::isdigit(code[2]) && code[2] != 'x'))
			throw std::runtime_error(logError("Error: unexpected \"" + code + "\" in \"error_page\" directive"));
		int nbr = std::atoi(code.c_str());
		if ((code[2] == 'x' && (nbr < 9 || nbr > 59)) || (code[2] != 'x' && (nbr < 100 || nbr > 599)))
			throw std::runtime_error(logError("Error: error code is out of range: \"" + code + "\" in \"error_page\" directive"));
		if (code[2] == 'x' && (_itToken != _tokens.end() - 2 || _itToken != _tokens.begin() + 1))
			throw std::runtime_error(logError("Error: syntax error in \"error_page\" directive"));
		if (code[2] == 'x')
			for (int i = 0; i < 10; i++)
				_itConfig->error_pages[(nbr * 10) + i] = file;
		else
			_itConfig->error_pages[nbr] = file;
	}
}

/*
	Establece el tamaño máximo permitido para el cuerpo de las solicitudes del cliente.
	La función valida la directiva "client_max_body_size" para asegurarse de que esté correctamente formateada
	y que contenga el número adecuado de argumentos. Verifica que la directiva termine con un punto y coma.
	Analiza el valor del tamaño del cuerpo, el cual puede estar en bytes, kilobytes (K), megabytes (M) o gigabytes (G).
	Valida que el valor sea un número entero o un número seguido de una unidad (K, M o G).
	Si no tiene una unidad, se asume que el valor está en bytes.
	Convierte el valor del tamaño del cuerpo a bytes de acuerdo con la unidad especificada y actualiza
	el tamaño máximo permitido en la configuración. Verifica que el tamaño máximo no exceda el límite de 5 GB.
	Si el tamaño excede este límite, lanza un error.
*/
void Configuration::setMaxBodySize()
{
	size_t cPos = _itToken->find(";");
	if (_itToken == _tokens.end() || cPos == 0)
		throw std::runtime_error(logError("Error: invalid number of arguments in \"client_max_body_size\" directive"));
	if (_itToken != _tokens.end() - 1)
		throw std::runtime_error(logError("Error: too many arguments in \"client_max_body_size\" directive"));
	if (cPos != _itToken->size() - 1)
		throw std::runtime_error(logError("Error: syntax error in \"client_max_body_size\" directive"));
	_itToken->erase(_itToken->size() - 1);
	std::string body = *_itToken;
	unsigned i = 0;
	for (; body[i] != 0; i++)
		if (body[i] < '0' || body[i] > '9')
			break ;
	if ((i != body.size() && i != body.size() - 1) || (i == body.size() - 1 && (body[i] != 'K' && body[i] != 'M' && body[i] != 'G')))
		throw std::runtime_error(logError("Error: unexpected \"" + body + "\" in \"client_max_body_size\" directive"));
	_itConfig->body_size = std::atoi(body.c_str());
	if (body[i] == 'K')
		_itConfig->body_size *= 1024;
	if (body[i] == 'M')
		_itConfig->body_size *= 1024 * 1024;
	if (body[i] == 'G')
		_itConfig->body_size *= 1024 * 1024 * 1024;
	if (_itConfig->body_size > 5368709120)
		throw std::runtime_error(logError("Error: size limited to 5GB in \"client_max_body_size\" directive"));
}

/*
	Verifica la validez y la completitud del bloque de configuración del servidor.
	La función realiza las siguientes comprobaciones:
	1. Verifica que haya al menos una directiva "listen" en el bloque del servidor. Si no hay ninguna, lanza un error.
	2. Verifica que haya al menos una directiva "server_name" en el bloque del servidor. Si no hay ninguna, lanza un error.
	3. Recorre la lista de configuraciones de ubicación ("locations") para asegurarse de que haya una configuración de ubicación para la raíz ("/"). 
	   Si no se encuentra una directiva "location" para la raíz, lanza un error.
	La directiva "root" también es esencial en el archivo de configuración pero no es necesario revisarlo ya que es imprescindible para
	location "/" por lo que el error está controlado a la hora de implementar otras partes.
*/
void Configuration::checkServerBlockErrors()
{
	if (_itConfig->ip_port.empty())
		throw std::runtime_error("Error: no \"listen\" directive found in server block");
	if (_itConfig->server_names.empty())
		throw std::runtime_error("Error: no \"server_name\" directive found in server block");
	std::vector<Location_config>::iterator it = _itConfig->locations.begin();
	for (; it != _itConfig->locations.end(); it++)
		if (it->location == _itConfig->root + "/")
			break ;
	if (it == _itConfig->locations.end())
		throw std::runtime_error("Error: no location \"/\" directive found in server block");
}
/*
	Finaliza el bloque de configuración del servidor. 
	Si no hay más tokens después de la llave de cierre "}", marca el fin del bloque del servidor.
	Si hay más tokens, lanza una excepción por error de sintaxis.
	Luego, se verifica errores en el bloque del servidor con checkServerBlockErrors().
*/
void Configuration::endServerBlock()
{
	if (_itToken == _tokens.end())
		_inServerBlock = false;
	else
		throw std::runtime_error(logError("Error: syntax error after closing brace"));
	checkServerBlockErrors();
}

/*
	Maneja cada línea de configuración de acuerdo a su contexto. 
	Verifica si _tokens está vacío, en cuyo caso no hace nada. 
	Si no se está en un bloque de servidor, se inicializa el bloque del servidor. 
	Si estamos en un bloque de ubicación o el primer _token es "location", maneja las directivas de ubicación. 
	De lo contrario, analiza el primer _token y llama a la función correspondiente según éste. 
	Si se encuentra una directiva no reconocida, lanza una excepción. 
*/
void Configuration::handleConfigLine()
{
	if (_tokens.empty())
		return ;
	else if (_inServerBlock == false)
		initServerBlock();
	else if (_inLocationBlock == true || _tokens[0] == "location")
		handleLocations();
	else
	{
		_itToken = _tokens.begin() + 1;
		if (_tokens[0] == "listen")
			setListen();
		else if (_tokens[0] == "server_name")
			setServerName();
		else if (_tokens[0] == "root")
			setRootDirectory();
		else if (_tokens[0] == "index")
			setIndexFiles();
		else if (_tokens[0] == "error_page")
			setErrorPages();
		else if (_tokens[0] == "client_max_body_size")
			setMaxBodySize();
		else if (_tokens[0] == "}")
			endServerBlock();
		else
			throw std::runtime_error(logError("Error: unexpected \"" + _tokens[0] + "\""));
	}
}

/*
	Función principal.
	Lee y procesa cada línea del archivo de configuración.
	Incrementa el contador de líneas leídas.
	Crea tokens a partir de la línea.
	Gestiona la línea de configuración con la función handleConfigLine();
	Limpia el contenedor de tokens.
*/
void Configuration::parsing()
{
	std::string line;

	while (getline(_file, line))
	{
		_loopCounter++;
		createTokens(line);
		handleConfigLine();
		_tokens.clear();
	}
	printServerConfig();
}

/*
	Función complementaria de ayuda que imprime en la consola la configuración de todos los servidores
	guardados en el container _serversConfig de la clase Configuration. 
*/
void Configuration::printServerConfig()
{
	int i = 0;
	for (_itServer = _serversConfig.begin(); _itServer != _serversConfig.end(); _itServer++)
	{
		Server_config *it = *_itServer;
		std::cout << Yellow << "[SERVER " << i + 1 << "]" << Reset << std::endl << std::endl;
		for (std::vector<std::pair<std::string, int> >::iterator itip = it->ip_port.begin(); itip != it->ip_port.end(); itip++)
		{
			std::cout << "Listen IP:\t\t" << itip->first << std::endl;
			std::cout << "Listen PORT:\t\t" << itip->second << std::endl;
		}
		std::cout << "Server names:\t\t";
		for (std::vector<std::string>::iterator its = it->server_names.begin(); its != it->server_names.end(); its++)
		{
			std::cout << *its;
			if (its != it->server_names.end() - 1)
				std::cout << "\t";
		}
		std::cout << std::endl;
		std::cout << "Root:\t\t\t" << it->root << std::endl;
		std::cout << "Index:\t\t\t";
		for (std::vector<std::string>::iterator its = it->index.begin(); its != it->index.end(); its++)
		{
			std::cout << *its;
			if (its != it->index.end() - 1)
				std::cout << "  ";
		}
		std::cout << std::endl;
		for (std::map<int, std::string>::iterator itm = it->error_pages.begin(); itm != it->error_pages.end(); itm++)
		{
			std::cout << "Error page:\t\t" << itm->first << "->" << itm->second << std::endl;
		}
		std::cout << "Client max body size:\t" << it->body_size << " bytes" << std::endl;
		for (std::vector<Location_config>::iterator itl = it->locations.begin(); itl != it->locations.end(); itl++)
		{
			std::cout << "\nLocation:\t\t" << itl->location << std::endl;
			std::cout << "\tmethods\t\t";
			for (std::set<std::string>::iterator it = itl->methods.begin(); it != itl->methods.end(); it++)
			{
				std::cout << *it << " ";
			}
			std::cout << std::endl;
			if (itl->autoindex)
				std::cout << "\tautoindex:\ton" << std::endl;
			else
				std::cout << "\tautoindex:\toff" << std::endl;
			if (itl->redir.first != 0)
				std::cout << "\tredir code:\t" << itl->redir.first << std::endl;
			if (itl->redir.second != "")
				std::cout << "\tredir URI:\t" << itl->redir.second << std::endl;
		}
		std::cout << std::endl << std::endl;
		i++;
	}
}
