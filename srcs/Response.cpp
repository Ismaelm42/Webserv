#include "../includes/lib.hpp"

Response::Response(Client *client, Server_config *config, Request *request, struct Epoll_events *events)
:_client(client), _config(config), _request(request), _events(events), cgiFlag(0),_auto_index_flag(0)												// constructor
{
	if (DEBUG)
		std::cout << "Response constructor" << std::endl;
	_responseString = "";
		_code = _request->getErrorCode(); 				// usado como condición no iniciar a 200
	_hasIndexFlag = 0;
	//_client = NULL;
}

void Response::reset() { 
	_responseString = ""; 
	_code = _request->getErrorCode(); 				// usado como condición no iniciar a 200
	_client = NULL;
	_response_str = "";
	_target = "";
	_response_body_str = "";
	mime =  "";
	cgiFlag = 0;
	_auto_index_flag = 0;
	_hasIndexFlag = 0;
	// _request = NULL;
	// _config = NULL;
}	// resetea la respuesta

Response::Response(const Response& other) {
    if (DEBUG)
        std::cout << "Response copy constructor" << std::endl;
    
    _responseString = other._responseString;  // Copiar la cadena de respuesta
    _code = other._code;                      // Copiar el código de respuesta
    _client = other._client;                  // Copiar el puntero (copia superficial)
}

Response& Response::operator=(const Response& other) {
    if (this != &other) {  // Evitar autoasignación
        if (DEBUG)
            std::cout << "Response assignment operator" << std::endl;
        
        _responseString = other._responseString;  // Copiar la cadena de respuesta
        _code = other._code;                      // Copiar el código de respuesta
        _client = other._client;                  // Copiar el puntero (copia superficial)
    }
    return *this;
}

/*Response::Response(Request& req)										// constructor con request
{
}*/

Response::~Response()													// destructor								
{
	if(DEBUG)
		std::cout << "Response destructor" << std::endl;
};

int Response::getCode() const
{
	if (DEBUG)
		std::cout << "getCode is called" << std::endl;
	return _code;
}

int Response::setReturnCode(int code)
{
	if (DEBUG)
		std::cout << "setReturnCode is called" << std::endl;
	_code = code;
	return (_code);
}

/**
 * @brief set code and return 0
 * 
 * @param code 
 * @return int 
 */
int Response::setCode(int code)
{
	if (DEBUG)
		std::cout << "setCode is called" << std::endl;
	_code = code;
	return(0);
}

void Response::setClient(Client* client) {
    _client = client;  // Guarda el puntero al objeto Client
}

std::string statusString(short statusCode)
{
    switch (statusCode)
    {
        case 100:
            return "Continue";
        case 101:
            return "Switching Protocol";
        case 200:
            return "OK";
        case 201:
            return "Created";
        case 202:
            return "Accepted";
        case 203:
            return "Non-Authoritative Information";
        case 204:
            return "No Content";
        case 205:
            return "Reset Content";
        case 206:
            return "Partial Content";
        case 300:
            return "Multiple Choice";
        case 301:
            return "Moved Permanently";
        case 302:
            return "Moved Temporarily";
        case 303:
            return "See Other";
        case 304:
            return "Not Modified";
        case 307:
            return "Temporary Redirect";
        case 308:
            return "Permanent Redirect";
        case 400:
            return "Bad Request";
        case 401:
            return "Unauthorized";
        case 403:
            return "Forbidden";
        case 404:
            return "Not Found";
        case 405:
            return "Method Not Allowed";
        case 406:
            return "Not Acceptable";
        case 407:
            return"Proxy Authentication Required";
        case 408:
            return "Request Timeout";
        case 409:
            return "Conflict";
        case 410:
            return "Gone";
        case 411:
            return "Length Required";
        case 412:
            return "Precondition Failed";
        case 413:
            return "Payload Too Large";
        case 414:
            return "URI Too Long";
        case 415:
            return "Unsupported Media Type";
        case 416:
            return "Requested Range Not Satisfiable";
        case 417:
            return "Expectation Failed";
        case 418:
            return "I'm a teapot";
        case 421:
            return "Misdirected Request";
        case 425:
            return "Too Early";
        case 426:
            return "Upgrade Required";
        case 428:
            return "Precondition Required";
        case 429:
            return "Too Many Requests";
        case 431:
            return "Request Header Fields Too Large";
        case 451:
            return "Unavailable for Legal Reasons";
        case 500:
            return "Internal Server Error";
        case 501:
            return "Not Implemented";
        case 502:
            return "Bad Gateway";
        case 503:
            return "Service Unavailable";
        case 504:
            return "Gateway Timeout";
        case 505:
            return "HTTP Version Not Supported";
        case 506:
            return "Variant Also Negotiates";
        case 507:
            return "Insufficient Storage";
        case 510:
            return "Not Extended";
        case 511:
            return "Network Authentication Required";
        default:
            return "Undefined";
        }
}

/* Constructs Status line based on status code. */

void        Response::setStatusline()
{
	std::ostringstream temp; // usamos un osstream para convertir e int a str
	if (_code == 0)
		_code = 200;
	temp << _code;	// Pasar el entero al stream
    std::string codeStr = temp.str();     // Convertir el stream a std::string

    _response_str.append("HTTP/1.1 " + codeStr + " ");
    _response_str.append(statusString(_code));
    _response_str.append("\r\n");
}

/**
 * @brief comprueba si el request tiene un error
 *  si hay error lo asigna a la variable _code y retorna true
 *  si no hay error retorna false
 * @return true 
 * @return false 
 */
bool Response::isErrorCode()
{
	if (DEBUG)
		std::cout << "isErrorCode is called" << std::endl;
    if(_request->getErrorCode())
    {
        _code = _request->getErrorCode();
        return (1);
    }
    return (0);
}

std::string concatenatePaths(std::string str1, std::string str2, std::string str3)
{
    std::string res;
    int         len1;
    int         len2;

    len1 = str1.length();
    len2 = str2.length();
    if (str1[len1 - 1] == '/' && !str2.empty() && str2[0] == '/')		// si el último caracter de str1 es un "/" y el primer caracter de str2 es un "/"
        str2.erase(0, 1);												// elimina el primer caracter de str2
    if (str1[len1 - 1] != '/' && !str2.empty() && str2[0] != '/')		// si el último caracter de str1 no es un "/" y el primer caracter de str2 no es un "/"	
        str1.append(1, '/');											// inserta un "/" al final de str1	
    if (str2[len2 - 1] == '/' && !str3.empty() && str3[0] == '/')		// si el último caracter de str2 es un "/" y el primer caracter de str3 es un "/"
        str3.erase(0, 1);												// elimina el primer caracter de str3
    if (str2[len2 - 1] != '/' && !str3.empty() && str3[0] != '/')		// si el último caracter de str2 no es un "/" y el primer caracter de str3 no es un "/"
        str2.append(1, '/');											// inserta un "/" al final de str2	
    res = str1 + str2 + str3;											// concatena los tres strings				
    return (res);														// retorna el resultado	
}

int Response::getFile()
{
    std::ifstream file( _target.c_str());
	std::cout << "en getFile: "<< _target.c_str();
	std::cout <<std::endl;
    if (file.fail())
    {
        _code = 404;
        return (1);
    }
    std::ostringstream ss;
	ss << file.rdbuf();
    _response_body_str = ss.str();
    return (0);
}

void    Response::setHeaders()								// setea los headers de la respuesta
{	
	////////////////////////////////////
	//     **   Content-Type    **    //
	////////////////////////////////////
	MimeType _mime;
    _response_str.append("Content-Type: ");		
    mime = "text/html";
	if(_target.rfind(".", std::string::npos) != std::string::npos && _code == 200)	// si el target file tiene un punto, empezando por el final lo identifica como extension 
        mime = _mime.getMimeType(_target.substr(_target.rfind(".", std::string::npos) + 1)); // obtiene el mime type de la extension del archivo
    _response_str.append(mime);   // Corregido: remover el paréntesis extra
    _response_str.append("\r\n");
	std::cout << White << "contentType: " <<_response_str << White;

	////////////////////////////////////
	//  ** Location - redirection **  //				OJO !!! REvisar redirecciones no funcionan sin la ultima barra en la location
	////////////////////////////////////
	if (_location.length())
	{
		if(_location[_location.length() - 1] != '/')		
			_location.append("/");
		std::cout << "Location: " << _location << std::endl;
		_response_str.append("Location: "+ _location +"\r\n");
	}

	////////////////////////////////////
	//   **   Content-Length    **    //
	////////////////////////////////////
	std::stringstream ss;
	ss << _response_body_str.length();
	_response_str.append("Content-Length: " + ss.str() + "\r\n");

	////////////////////////////////////
	//    **   End of headers    **   //
	////////////////////////////////////
	_response_str.append("\r\n");

	if (DEBUG)
		std::cout << White << "setHeaders: " <<_response_str << White <<std::endl;
	
}

std::string Response::getMatch(std::string path, std::vector<Location_config> locations)
{

	if (DEBUG)
	{	
		std::cout << Green << "en getMatch____________________" << std::endl;
		std::cout << "path: " << path << std::endl;
		std::cout << "locations: " << std::endl;
		for (std::vector<Location_config>::iterator it = locations.begin(); it != locations.end(); it++) // itera sobre las locations
		{
			std::cout << "location: " << it->location << std::endl;
		}
		std::cout << White << std::endl;
	}
	std::string locationMatch = "";                           	// variable que almacena la key de la location
	std::string longestMatch = "";                            	// variable que almacena la key de la location más larga
	size_t longestMatchLength = 0;
	
	// La location tiene ya incluida la raiz
	std::string combinedPath = concatenatePaths(_config->root, path, "");;                                 // variable que almacena la longitud de la location más larga

	for (std::vector<Location_config>::iterator it = locations.begin(); it != locations.end(); it++) // itera sobre las locations
	{
		std::string locationToFind = it->location;                                                    // obtiene la key de la location
		if (combinedPath.find(locationToFind) == 0 && locationToFind.length() > longestMatchLength)              // si el path del request contiene la location y la longitud de la location es mayor que la longitud de la location más larga
		{
			longestMatch = locationToFind;                                                         // asigna la location a la variable de la location más larga
			longestMatchLength = locationToFind.length();                                          // asigna la longitud de la location a la variable de la longitud de la location más larga
		}
	}
	if (longestMatchLength > 0)                                                              // si la longitud de la location más larga es mayor que 0
		locationMatch = longestMatch;                                                        // asigna la location más larga a la variable de la location
	std::cout << "locationMatch: " << locationMatch << std::endl;
	return locationMatch;                                                                    // retorna la location
}

// Función para obtener Location_config usando el valor de location
static Location_config* find_location(Server_config &server, const std::string &location_value) {
    for (size_t i = 0; i < server.locations.size(); ++i) {
        if (server.locations[i].location == location_value) {
            return &server.locations[i];  // Devuelve un puntero al Location_config encontrado
        }
    }
    return NULL;  // Si no se encuentra, devuelve NULL
}

// Función para imprimir los valores de un Location_config
static void printLocationConfig(const Location_config* locConf) {
    if (locConf == NULL) {
        std::cout << "El puntero a Location_config es nulo." << std::endl;
        return;
    }

    // Imprimir autoindex
    std::cout << "Autoindex: " << (locConf->autoindex ? "true" : "false") << std::endl;

    // Imprimir location
    std::cout << "Location: " << locConf->location << std::endl;

    // Imprimir index (vector de strings)
    std::cout << "Index files: ";
    if (locConf->index.empty()) {
        std::cout << "No hay archivos index definidos." << std::endl;
    } else {
        for (size_t i = 0; i < locConf->index.size(); ++i) {
            std::cout << locConf->index[i];
            if (i < locConf->index.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
    // Imprimir methods (set de strings)
    std::cout << "Allowed methods: ";
    if (locConf->methods.empty()) {
        std::cout << "No hay métodos permitidos." << std::endl;
    } else {
        for (std::set<std::string>::const_iterator it = locConf->methods.begin(); it != locConf->methods.end(); ++it) {
            std::cout << *it;
            if (it != --locConf->methods.end()) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
    // Imprimir redir (pair de int y string)
    std::cout << "Redirection: Código " << locConf->redir.first << ", Nueva dirección: " << locConf->redir.second << std::endl;
    // Imprimir cgi (vector de pares de strings)
    std::cout << "CGI mappings: ";
    if (locConf->cgi.empty()) {
        std::cout << "No hay CGI mappings." << std::endl;
    } else {
        for (size_t i = 0; i < locConf->cgi.size(); ++i) {
            std::cout << "(" << locConf->cgi[i].first << " -> " << locConf->cgi[i].second << ")";
            if (i < locConf->cgi.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << std::endl;
    }
}

int Response::isValidTargetFile()
{
	std::ifstream file(_target.c_str());
    return (file.good());
}

/**
 * @brief comparar si el método está entre los allowed methods del location
 * 
 * @return int 
 */
int Response::isNotValidMethod()
{
	// comentado para permitir los allowed method vacíos
	// std::cout << "isNotValidMethod: ";
    // if (_location_config->methods.empty()) {
    //     return (setReturnCode(501));
    // } else {
        for (std::set<std::string>::const_iterator it = _location_config->methods.begin(); it != _location_config->methods.end(); ++it) {
            if (*it == _request->getMethodStr())
				return (0);
        }
	// }
	return (setReturnCode(405));
}

/**
 * @brief join the path removing the firs directory is it is repeatted
 * Para su posible uso en el autoindex y si no se encuentra el archivo index
 * PEndiente de testear por un cambio de enfoque en la implementación
 * @param locationMatch 
 * @param requestPath 
 * @return std::string 
 */
// std::string joinPaths(const std::string& locationMatch, const std::string& requestPath) {
//     // Quitar la barra final de locationMatch si existe
//     std::string path1 = locationMatch;
//     if (path1.back() == '/') {
//         path1 = path1.substr(0, path1.size() - 1);
//     }
//     // Quitar la barra inicial de requestPath si existe
//     std::string path2 = requestPath;
//     if (path2.front() == '/') {
//         path2 = path2.substr(1);
//     }
//     // Encontrar el último directorio en path1
//     size_t lastSlash = path1.find_last_of('/');
//     std::string lastDir;
//     if (lastSlash != std::string::npos) {
//         lastDir = path1.substr(lastSlash + 1);
//         path1 = path1.substr(0, lastSlash); // Eliminar el último directorio
//     }
//     // Encontrar el primer directorio en path2
//     size_t firstSlash = path2.find('/');
//     std::string firstDir; 
//     if (firstSlash != std::string::npos) {
//         firstDir = path2.substr(0, firstSlash);
//     } else {
//         firstDir = path2; // Si no hay más barras, toda la cadena es el primer directorio
//     }
//     // Verificar si el último directorio de path1 es igual al primer directorio de path2
//     if (lastDir == firstDir) {
//         // Concatenar sin el último directorio de path1
//         return path1 + "/" + path2;
//     } else {
//         // Concatenar normalmente
//         return locationMatch + "/" + requestPath;
//     }
// }

bool isReadableDirectory(const std::string& path) {
	
	struct stat info;
    // Verificar si la ruta es un directorio
    if (stat(path.c_str(), &info) != 0) {
        // No se pudo acceder al archivo o directorio
        return false;
    } else if (!(info.st_mode & S_IFDIR)) {
        // No es un directorio
        return false;
    }
    // Verificar si el directorio tiene permisos de lectura
    if (access(path.c_str(), R_OK) != 0) {
        // No tiene permisos de lectura
        return false;
    }
    // Es un directorio y tiene permisos de lectura
    return true;
}

int Response:: setIndex()			// el index puede se un directorio?
{
	std::vector<std::string> index;
	std::cout << "en setIndex_______________________" << std::endl;
	std::cout << "_location_config->index.size(): " << _location_config->index.size() << std::endl;
	std::cout << "_config->index.size(): " << _config->index.size() << std::endl;
	if(_location_config->index.size() > 0)
		index = _location_config->index;
	else if (_config->index.size() > 0)
		index = _config->index;
	if (index.size() > 0 && (_request->getPath() == "/" || _request->getPath() == ""))
	{
		std::cout << "Entra en el bucle"<< std::endl;
		for (size_t i = 0; i < index.size(); i++){
			_target = index[i];
			std::cout << "target dentro del bucle: " << _target << std::endl;
			if(isValidTargetFile())
			{
				_hasIndexFlag = 1;
				return (0);			
			}
		}
		_target = concatenatePaths(_config->root, _request->getPath(), "");
		std::cout << "A evaluar en SetIndex" << _target << std::endl;
		std::cout << _location_config->autoindex << std::endl;
		if (!isReadableDirectory(_target) ||
			!_location_config->autoindex)
			return (setReturnCode(404));
	}
	return (0);
}

static bool hasValidExtension(const std::string& path, const std::vector<std::pair<std::string, std::string> >& config) {
    // Encontrar la última posición del punto en el path manualmente
    std::string::size_type dotPos = path.rfind('.');
    if (dotPos == std::string::npos) {
        return false; // No se encontró una extensión
    }

    // Obtener la extensión del path
    std::string extension = path.substr(dotPos);

    // Verificar si la extensión está en _location_config
    for (std::vector<std::pair<std::string, std::string> >::const_iterator it = config.begin(); it != config.end(); ++it) {
        if (it->first == extension) {
            return true; // Extensión válida encontrada
        }
    }
    return false; // No se encontró una extensión válida
}

/**
 * @brief Función para lanzar el CGI con las comporbaciones que estimemos oportunas
 * 
 * @return int 
 */
int Response::launchCgi()
{
	/* 
		Ismael en esta función podemos lanzar el CGI
		creo que necesito gestionar algo más dentro de la función
		buildBody() para que se ejecute el CGI pero estoy reventao.

		El flag del cgi lo necesito para que no continue la función en el caso 
		de estar eecutandose un CGI
		*/
	
	
	
	std::cout << "en launchCgi____________________" << std::endl;


	std::string cgi_path = _config->root + _request->getPath();
	std::cout << "cgi_path: " << cgi_path << std::endl;
	
	// Podemos chequear si es un archivo ejecutable y si tiene permisos de ejecución
	//	si no es así setReturnCode(403);

	//llamar una instancia del objeto cgi y a su llamada para resetear los valores																		// retorna 1		
    // asignarle el path 
	cgiFlag = true;								

    // Alfonsete:
    // Como tengo un puntero al cliente, he creado una función en Cliente que lo que hace es crear la clase CGI y lanzar el execute CGI.
    // De esta manera lo hace con el CGI del cliente y se lanza desde aquí. Matamos dos tiros de un pájaro ;)
    // El flag debe quedarse en true aunque igual ni hace falta. Creo que no hace falta pero por ahora lo vamos a dejar.

    _client->initCgi();
	cgiFlag = 0;
	// Desde esta función podríamos realizar el pipe y guardar el fd[2] en la response y setReturnCode(500) si falla																			// flag para saber si se esta ejecutando un cgi y su estado	

	// Llamar a la función de obtener las variables de entorno
	// llamara a la función execute del objeto cgi

	return (1);
}

int Response::getTarget()
{
	if(DEBUG)
		std::cout << "en getTarget____________________" << std::endl;
	//--------------------------------------------------------------------------//
	//						 ** obtener la location ** 							//
	//--------------------------------------------------------------------------//
	_location_config = NULL;
    std::string locationMatch = "";                     	//clave de la location
	std::cout << Blue <<
		"requestPath en getTarget: " << _request->getPath() <<
			White << std::endl;

    locationMatch = getMatch(_request->getPath(), _config->locations);                                                        // variable que almacena la key de la location
	if (!locationMatch.empty())
	{ 
	_location_config = find_location(*_config,locationMatch);
	std::cout << "_location_config:_______________	____________- "  << std::endl;
    printLocationConfig(_location_config);
	
	//--------------------------------------------------------------------------//
	//    cofigurar target con las directivas de la location y el server      	//
	//--------------------------------------------------------------------------//
	
	//--------------------------------------------------------------------------//
	//						 **	client_max_body_size **							//
	//--------------------------------------------------------------------------//
	//    Pendiente de ver si es necesario hacerlo aqui o en el buildbody	    //
	//--------------------------------------------------------------------------//
	
	if (_request->getBody().length() > _location_config->body_size)     // obtiene el tamaño del body del request y si el tamaño del body es mayor que el tamaño maximo permitido
		return (setReturnCode(413));        							// retorna 413 y lo setea como error para la gestión de errores		
	
	//--------------------------------------------------------------------------//
	//							 ** allowed methods	**							//
	//--------------------------------------------------------------------------//
	// 					Si hay allowed methods en location mira si 				//
	//						el método del request es válido 					//
	//--------------------------------------------------------------------------//
	
	if (!_location_config->methods.empty())
	{	
		if (isNotValidMethod())
			return (501);
	}
	//--------------------------------------------------------------------------//
	//								** index **									//
	//--------------------------------------------------------------------------//
	// 		SetIndex devuelve 0 si el index es válido o si no debe haber index   //
	//		uso el size del target para saber si debemos seguir buscandolo		//							//
	//--------------------------------------------------------------------------//

	if (setIndex())							
		return (413);				
	else if(_target.size() == 0 )	// si el target es vacío lo asgina asginación temporal 
	{
			std::cout << "locationMatch: " << locationMatch << std::endl;
			std::cout << "_request->getPath(): "  << _request->getPath() << std::endl; 
			_target= concatenatePaths(_config->root, _request->getPath(), "");		// combina el root del server con el path del request
			std::cout << "target: " << _target << std::endl;
	}
	//--------------------------------------------------------------------------//
	//								** return **								//
	//--------------------------------------------------------------------------//
	//		Comprueba si hay un return en la location y si lo hay lo setea		//
	//           Pendiente de configurar y ver como hay que devolverlo 			//
	//--------------------------------------------------------------------------//
     if(_location_config->redir.first)
 	{
		std::cout << "en return____________________" << std::endl;
		_location = _location_config->redir.second;
		_response_body_str = ""; // prueba para ver si funcionar la redirección con el header Location
		_target = "";
		return (setReturnCode(_location_config->redir.first));
	}

	//--------------------------------------------------------------------------//
	//								** cgi **									//
	//--------------------------------------------------------------------------//
	if (!_location_config->cgi.empty() && 
		hasValidExtension(_request->getPath(), _location_config->cgi)) {
        return(launchCgi());
        }
	//--------------------------------------------------------------------------//
	//								** autoindex **								//
	//--------------------------------------------------------------------------//
	//		Comprueba si el target es un directorio y si no tiene autoindex		//
	//--------------------------------------------------------------------------//
	// comprobar si el target es un directorio, si no tiene un index válidd 
	// y si no tiene autoindex

	std::cout << Yellow;
	std::cout << "autoindex: " << _location_config->autoindex << std::endl;
	std::cout << "target: " << _target << std::endl;
	std::cout << "isReadableDirectory: " << isReadableDirectory(_target) << std::endl;
	std::cout << "hasIndexFlag: " << _hasIndexFlag << std::endl;
	std::cout << White;

	if (!_hasIndexFlag && _location_config->autoindex && isReadableDirectory(_target))
	{
		std::cout << Red << "isReadableDirectory: " << _target << White << std::endl;
		_auto_index_flag = 1;
		return (0);
	}
	}
	else
	{
		_target= concatenatePaths(_config->root, _request->getPath(), "");		// combina el root del server con el path del request
	}
	return 0;
}

int Response::buildErrorPage(int code)
{

	std::cout << "en BuildErrorPage____________________" << std::endl;
	//	std::map<int, std::string> error_pages; // Mapa de páginas de error
	if(_config->error_pages.find(code)==_config->error_pages.end()|| _config->error_pages.at(code).empty())
	{
		std::cout << Yellow << "No hay página de error para el código: " << code <<White << std::endl;
		// llamar a la función que genera la página de error por defecto y la asigna a _response_body_str
		return (0);
	}
	
	for (std::map<int, std::string>::iterator itm = _config->error_pages.begin(); itm != _config->error_pages.end(); itm++)
	{
		if (itm->first == code)
			_target = itm->second;
		// std::cout << "Paáginas de errores:\t\t\t" << itm->first << "->" << itm->second << std::endl;
	}
	if (getFile())													// lee el archivo					
        return (1);
    // if (_config->error_pages.find(code) == _config->error_pages.end())		
	// //it = _config->error_pages.find(code);
	// std::string errorPage = "<html>\n<head>\n<title>Error</title>\n</head>\n<body>\n<h1>Error ";
	// errorPage.append(std::to_string(code));
	// errorPage.append("</h1>\n</body>\n</html>\n");
	return (0);
}

int Response::buildBody()
{
	if (DEBUG)
		std::cout << "en buildBody____________________" << std::endl;
	// 					Confirmacion con parámetros del server
	// ***********   client_max_body_size			***********
	
	if (_request->getBody().length() > _config->body_size)       						// obtiene el tamaño del body del request y si el tamaño del body es mayor que el tamaño maximo permitido
		return (setReturnCode(413));                             	// retorna 413 y lo setea como error para la gestión de errores       
	if (getTarget())
		return (1);
	if (cgiFlag || _auto_index_flag || _code)	//	Condiciones para que no se ejecute el getFile pendientes de diseñar o terminar d epulir	
	    return (0);
	if (_request->getMethod() == GET )			// si el método del request es GET o HEAD
    {
		std::cout << "GET" << std::endl;
		std::cout << _request->getMethod() << std::endl;
        if (getFile())													// lee el archivo					
            return (1);
    }
	//////////////////////////////////////////	Pendiente de retomar coon los cgi ///////////////////////////////
    else if (_request->getMethod() == POST)
    {
		std::cout <<" El método es post----------------------->>" << std::endl;
        std::cout << "_target: " << _target << std::endl;
		// if (isValidTargetFile()){								// Comentado para permitir la creación de archivos
		// 	std::cout << "isValidTargetFile" << std::endl;			// si el archivo es válido me detecta lacarpeta /upload como archivo
		// 	return (setCode(204));									// y retorna 204 Ismael, habría que cambiar la llamada en el htl y enlazarlo von un archivo cgi...
		// }
        // std::ofstream file(_target.c_str(), std::ios::binary);
        // if (file.fail())
        //     return (setReturnCode(404));
        if (_request->getMultiformFlag()){							// si el request es multiform deberá ir enlazado a un archivo que en condiciones normales
																	// sería el encargado de validar los datos y establecer los códigos de salida en caso de
																	// éxito u error
																	// pendiente de limpiar los boundaries ver como debería guardarlos para enlazarlo con el archivo del servidor		
            std::string body = _request->getBody();
			std::cout << "_target: " << _target << std::endl;
			std::cout << "body en if: " << body << std::endl;
            //body = removeBoundary(body, _request->getBoundary());
            //file.write(body.c_str(), body.length());
			return (setCode(204));
        }
        else{
			std::cout << "body en else: " << _request->getBody() << std::endl;
            //file.write(_request->getBody().c_str(), _request->getBody().length());
        }
    }
    else if (_request->getMethod() == DELETE)
    {
        if (!isValidTargetFile())
            return (setReturnCode(404));
        if (remove( _target.c_str() ) != 0 )
            return (setReturnCode(500));
    }
    _code = 200;
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	return (0);
}


std::string removeRoot(std::string target, std::string root)
{
	std::string::size_type pos = target.find(root);
	if (pos != std::string::npos)
		target.erase(pos, root.length());
	return target;
}

int Response::buildDirHtml()
{
	std::string root = _config->root;
	std::cout << Yellow << "en buildDirHtml____________________" << White << std::endl;
    struct dirent   *structDirent;
    DIR             *dir;
	_response_body_str = "";
    dir = opendir(_target.c_str());
    if (dir == NULL)
    {   
        std::cerr << "Error in opendir" << std::endl;
        return (1);
    }
    _response_body_str.append("<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n<meta charset='UTF-8'>\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
    _response_body_str.append(
    "    <style>\n"
  	"        .footer {\n"
    "            text-align: center;\n"
    "            font-size: 0.8rem;\n"
    "            margin-top: 50px;\n"
    "            color: #940f0f;\n"
	"            text-shadow: #e8fd81ad 1px 0px 2px;\n"
    "        }\n"
    "    </style>\n"
    );
	_response_body_str.append("<title>Index of ");
    _response_body_str.append(_target);
    _response_body_str.append("</title>\n</head>\n<body style=\"background-color: black; color: red; font-family: Arial, sans-serif;\">\n");
    _response_body_str.append("<h1 style=\"text-align: center;\">Index of " + _target + "</h1>\n");
    _response_body_str.append("<table style=\"width:80%; margin: auto; border-collapse: collapse; font-size: 15px;\">\n");
    _response_body_str.append("<hr style=\"border: 1px solid red; width:90%;margin-bottom: 0px;\">\n");
    _response_body_str.append("<hr style=\"border: 1px solid #e8fd81; width:90%; margin-top:0px;\">\n");
    _response_body_str.append("<thead style=\"color: #e8fd81;\">\n<tr>\n<th style=\"text-align:left; padding: 10px; border-bottom: 1px solid red;\">File Name</th>\n");
    _response_body_str.append("<th style=\"text-align:left; padding: 10px; border-bottom: 1px solid red;\">Last Modification</th>\n");
    _response_body_str.append("<th style=\"text-align:left; padding: 10px; border-bottom: 1px solid red;\">File Size</th>\n");
    _response_body_str.append("</tr>\n</thead>\n<tbody>\n");
    struct stat file_stat;
    std::string file_path;
	
    while((structDirent = readdir(dir)) != NULL)
    {
        if(strcmp(structDirent->d_name, ".") == 0)
            continue;
        file_path = concatenatePaths(_target, structDirent->d_name,"");
        stat(file_path.c_str(), &file_stat);
		file_path = removeRoot(file_path, root);
		std::cout << Red << "file_path: " << file_path << White << std::endl;
        _response_body_str.append("<tr>\n");
        _response_body_str.append("<td style=\"padding: 10px; border-bottom: 1px solid red;\">\n");
        _response_body_str.append("<a href=\"");
        _response_body_str.append(file_path);
        // _response_body_str.append(structDirent->d_name);
        if (S_ISDIR(file_stat.st_mode))
            _response_body_str.append("/");
        _response_body_str.append("\" style=\"color: red; text-decoration: none;\">");
        _response_body_str.append(structDirent->d_name);
        if (S_ISDIR(file_stat.st_mode))
            _response_body_str.append("/");
        _response_body_str.append("</a>\n");
        _response_body_str.append("</td>\n");
        _response_body_str.append("<td style=\"padding: 10px; border-bottom: 1px solid red;\">");
        _response_body_str.append(ctime(&file_stat.st_mtime));
        _response_body_str.append("</td>\n");
        _response_body_str.append("<td style=\"padding: 10px; border-bottom: 1px solid red;\">");
        if (!S_ISDIR(file_stat.st_mode))
            _response_body_str.append(toStr(file_stat.st_size));
        _response_body_str.append("</td>\n");  
        _response_body_str.append("</tr>\n");
		std::cout << Yellow << "file_path: " <<file_path << White << std::endl;
    }   
    _response_body_str.append("</tbody>\n</table>\n");
	_response_body_str.append("<div class='footer'>\n<p>Powered by Imoro-sa & Alfofern C++98 WebServer</p>\n</div>\n");
    _response_body_str.append("</body>\n</html>\n");
    return (0);
}

// int Response::buildDirHtml()
// {
// 	std::cout << Yellow << "en buildDirHtml____________________" << White << std::endl;
//     struct dirent   *structDirent;
//     DIR             *dir;
//     std::string     dirHtmlStr;  
//     dir = opendir(_target.c_str());
//     if (dir == NULL)
//     {    
//         std::cerr << "opendir failed" << std::endl;
//         return (1);
//     }
//     dirHtmlStr.append("<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n<meta charset='UTF-8'>\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
//     dirHtmlStr.append(
//     "    <style>\n"
//   	"        .footer {\n"
//     "            text-align: center;\n"
//     "            font-size: 0.8rem;\n"
//     "            margin-top: 50px;\n"
//     "            color: #ff0000;\n"
//     "        }\n"
//     "    </style>\n"
//     );
// 	dirHtmlStr.append("<title>Index of ");
//     dirHtmlStr.append(_target);
//     dirHtmlStr.append("</title>\n</head>\n<body style=\"background-color: black; color: red; font-family: Arial, sans-serif;\">\n");
//     dirHtmlStr.append("<h1 style=\"text-align: center;\">Index of " + _target + "</h1>\n");
//     dirHtmlStr.append("<table style=\"width:80%; margin: auto; border-collapse: collapse; font-size: 15px;\">\n");
//     dirHtmlStr.append("<hr style=\"border: 1px solid red;\">\n");
//     dirHtmlStr.append("<thead style=\"color: yellow;\">\n<tr>\n<th style=\"text-align:left; padding: 10px; border-bottom: 1px solid red;\">File Name</th>\n");
//     dirHtmlStr.append("<th style=\"text-align:left; padding: 10px; border-bottom: 1px solid red;\">Last Modification</th>\n");
//     dirHtmlStr.append("<th style=\"text-align:left; padding: 10px; border-bottom: 1px solid red;\">File Size</th>\n");
//     dirHtmlStr.append("</tr>\n</thead>\n<tbody>\n");
//     struct stat file_stat;
//     std::string file_path;
//     while((structDirent = readdir(dir)) != NULL)
//     {
//         if(strcmp(structDirent->d_name, ".") == 0)
//             continue;
//         file_path = _target + structDirent->d_name;
//         stat(file_path.c_str(), &file_stat);
//         dirHtmlStr.append("<tr>\n");
//         dirHtmlStr.append("<td style=\"padding: 10px; border-bottom: 1px solid red;\">\n");
//         dirHtmlStr.append("<a href=\"");
//         dirHtmlStr.append(structDirent->d_name);
//         if (S_ISDIR(file_stat.st_mode))
//             dirHtmlStr.append("/");
//         dirHtmlStr.append("\" style=\"color: red; text-decoration: none;\">");
//         dirHtmlStr.append(structDirent->d_name);
//         if (S_ISDIR(file_stat.st_mode))
//             dirHtmlStr.append("/");
//         dirHtmlStr.append("</a>\n");
//         dirHtmlStr.append("</td>\n");
//         dirHtmlStr.append("<td style=\"padding: 10px; border-bottom: 1px solid red;\">");
//         dirHtmlStr.append(ctime(&file_stat.st_mtime));
//         dirHtmlStr.append("</td>\n");
//         dirHtmlStr.append("<td style=\"padding: 10px; border-bottom: 1px solid red;\">");
//         if (!S_ISDIR(file_stat.st_mode))
//             dirHtmlStr.append(toStr(file_stat.st_size));
//         dirHtmlStr.append("</td>\n");  
//         dirHtmlStr.append("</tr>\n");
//     }   
//     dirHtmlStr.append("</tbody>\n</table>\n");
// 	dirHtmlStr.append("<div class='footer'>\n<p>Powered by Imoro-sa & Alfofern C++98 WebServer</p>\n</div>\n");
//     dirHtmlStr.append("</body>\n</html>\n");
//     _response_body_str = dirHtmlStr;
//     return (0);
// }

void Response::buildResponse()
{	
	if (DEBUG)
		std::cout << "Building response is called" << std::endl;
	if (isErrorCode() || buildBody())                                     		 // forma de comprobar si hay error en el request y si no lo hay error construye el body
		buildErrorPage(_code);                                           // si hay error construye el error body	
    std::cout<< "en build response hasIndexFlag: " << _hasIndexFlag << std::endl;
	if	(cgiFlag)                                                           // si es un cgi retorna
		return;
	else if (_auto_index_flag)                                               // si es un auto index
    {	
        if (buildDirHtml())          												// construye el index con el _target
		{
			_code = 500;
      		// Llamar a la construcción del error body crear uno con int como argumento y otro sin
			// debe rellenar _response_body_str
        }
        else
            _code = 200;
    }
	setStatusline();													// construye la linea de estado de la respuesta	
	setHeaders();														// setea los headers de la respuesta
    if(_request->getMethod() == GET || _code != 200) 					// si el método del request no es HEAD y el método del request es GET o el código no es 200
    {
		_response_str.append(_response_body_str);										// agrega el body a la respuesta	
	}
	_responseString = _response_str;
	
	if (DEBUG)
	{
		//std::cout << "Response: " << _response_str << std::endl;
		std::cout << "Response size: " << _response_str.size() << std::endl;
		//std::cout << "_client: " << _client << std::endl;
		//std::cout << "en buildResponse Client_fd: " << _client->_fd << std::endl;
		//_client->request.printParsed();	// Imprime la request a través del puntero del cliente y su objeto request
		std::cout << "Response built" << std::endl;
	}
}

std::string Response::getResString()
{
	if (DEBUG)
		std::cout << "getResString is called" << std::endl;
	return _responseString;
}
