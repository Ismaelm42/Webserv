#include "../includes/lib.hpp"

Response::Response(Client *client, Server_config *config, Request *request, struct Epoll_events *events)
:_client(client), _config(config), _request(request), _events(events)												// constructor
{
	if (DEBUG)
		std::cout << "Response constructor" << std::endl;
	_responseString = "";
		_code = _request->getErrorCode(); 				// usado como condición no iniciar a 200
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

void Response::setCode(int code)
{
	if (DEBUG)
		std::cout << "setCode is called" << std::endl;
	_code = code;
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
	temp << _code;	// Pasar el entero al stream
    std::string codeStr = temp.str();     // Convertir el stream a std::string

	/*_response_str.append("HTTP/1.1 ");
	_response_str.append("200");
	_response_str.append(" OK\r\n");*/
	
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
	std::cout << "isErrorCode: " << _code << std::endl;
    if(_request->getErrorCode())
    {
        _code = _request->getErrorCode();
		std::cout << "isErrorCode: " << _code << std::endl;
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

/*int Response::getFile()
{
    std::ifstream file;
    
    // Determina si el archivo es binario o no usando el MIME type
    bool isBinary = (mime == "image/png" || mime == "image/jpeg" || mime == "image/jpg");
    
    // Abrir el archivo en modo texto o binario dependiendo del tipo
    if (isBinary) {
        file.open(_target.c_str(), std::ios::binary); // Abrir en modo binario
    } else {
        file.open(_target.c_str()); // Abrir en modo texto por defecto
    }
    
    // Verificar si la apertura del archivo ha fallado
    if (file.fail()) {
        _code = 404;
        return (1); // Error 404 si el archivo no existe
    }

    // Si es binario, leerlo en un std::vector<char>
    if (isBinary) {
        file.seekg(0, std::ios::end); // Moverse al final para obtener el tamaño del archivo
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg); // Volver al inicio

        // Redimensionar el vector para contener todo el archivo
        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            _code = 500; // Error 500 si la lectura falla
            return (1);
        }

        // Convertir el vector a string para almacenarlo en _response_body_str
        _response_body_str.assign(buffer.begin(), buffer.end());
    } 
    // Si es un archivo de texto, leerlo directamente en un string
    else {
        std::ostringstream ss;
        ss << file.rdbuf();
        _response_body_str = ss.str();
    }

    return (0); // Éxito
}
*/


void   Response::contentType()									// setea el content type de la respuesta
{
	MimeType _mime;
    _response_str.append("Content-Type: ");					// agrega el string: "Content-type: "a la respuesta
    mime = "text/html";
	if(_target.rfind(".", std::string::npos) != std::string::npos && _code == 200)	// si el target file tiene un punto, empezando por el final lo identifica como extension 
        mime = _mime.getMimeType(_target.substr(_target.rfind(".", std::string::npos) + 1)); // obtiene el mime type de la extension del archivo
    _response_str.append(mime);   // Corregido: remover el paréntesis extra
    _response_str.append("\r\n");
	std::cout << "en contentType inicio de response_str: " << std::endl;
	std::cout << _response_str << "Find de response_str" << Red << std::endl;
}


void    Response::setHeaders()								// setea los headers de la respuesta
{
	std::stringstream responseStream;
	std::stringstream ss;
    ss << _response_body_str.length();
	//responseStream << "Content-Type: text/html\r\n";		//	
	contentType();
	responseStream << "Content-Length: " << ss.str() << "\r\n";
	_response_str.append(responseStream.str());
    _response_str.append("\r\n");
}

std::string Response::getMatch(std::string path, std::vector<Location_config> locations)
{
	std::cout << "en getMatch____________________" << std::endl;

	std::string locationMatch = "";                           	// variable que almacena la key de la location
	std::string longestMatch = "";                            	// variable que almacena la key de la location más larga
	size_t longestMatchLength = 0;
	
	// La location tiene ya incluida la raiz
	std::string combinedPath = concatenatePaths(_config->root, path, "");;                                 // variable que almacena la longitud de la location más larga

	// Ver como afectan las directivas para la incorporación de la raiz y ver 
	// como variaría la dirección del target cuando la directiva esté 
	// incluida en ambos sitios - pendiente de ver con Ismael

	for (std::vector<Location_config>::iterator it = locations.begin(); it != locations.end(); it++) // itera sobre las locations
	{
		std::cout << "location: " << it->location << std::endl;
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
				return (0);			
		}
		return (setReturnCode(404));
	}
	return (0);
}

int Response::getTarget()
{
	//--------------------------------------------------------------------------//
	//						 ** obtener la location ** 							//
	//--------------------------------------------------------------------------//
	_location_config = NULL;
	std::cout << "en getTarget____________________" << std::endl;
    std::string locationMatch = "";                     	//clave de la location
    locationMatch = getMatch(_request->getPath(), _config->locations);                                                        // variable que almacena la key de la location
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
	
	if (_request->getBody().length() > _location_config->body_size)       						// obtiene el tamaño del body del request y si el tamaño del body es mayor que el tamaño maximo permitido
		return (setReturnCode(413));        		// retorna 413 y lo setea como error para la gestión de errores
		
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
	// 		SetIdex devuelve 0 si el index es válido o si no debe haber index   //
	//		uso el size del target para saber si debemos seguir buscandolo		//							//
	//--------------------------------------------------------------------------//
	if (setIndex())							
		return (413);				
	else if(_target.size() == 0 )	// si el target es vacío lo asgina asginación temporal 
	{
			std::cout << "locationMatch: " << locationMatch << std::endl;
			std::cout << "_request->getPath(): "  << _request->getPath() << std::endl; 
			_target= concatenatePaths(_config->root, _request->getPath(), "");		// combina el root del server con el path del request
	
	}
	//--------------------------------------------------------------------------//
	//								** autoindex **								//
	//--------------------------------------------------------------------------//
	//		Comprueba si el target es un directorio y si no tiene autoindex		//
	//--------------------------------------------------------------------------//
	//        	Pendiente de configurar y ver como hay que devolverlo 			//	
	//--------------------------------------------------------------------------//
	
	return 0;
}

int Response::buildBody()
{
	std::cout << "en buildBody____________________" << std::endl;
	// 					Confirmacion con parámetros del server
	// ***********   client_max_body_size			***********
	
	if (_request->getBody().length() > _config->body_size)       						// obtiene el tamaño del body del request y si el tamaño del body es mayor que el tamaño maximo permitido
		return (setReturnCode(413));                                                         // retorna 413 y lo setea como error para la gestión de errores       
	if (getTarget())
		return (1);
	return (0);
}

void Response::buildResponse()
{	
	if (isErrorCode() || buildBody())                                     		 // forma de comprobar si hay error en el request y si no lo hay error construye el body
	{
		std::cout << "en build response codigo de error" << _code << std::endl;                                               // si hay error construye el error body	
	}
	if (DEBUG)
		std::cout << "Building response is called" << std::endl;

	if (_request->getMethod() == GET )			// si el método del request es GET o HEAD
    {
		std::cout << "GET" << std::endl;
		std::cout << _request->getMethod() << std::endl;
        if (getFile())														// lee el archivo					
            return ;
    }
	setStatusline();											// construye la linea de estado de la respuesta	
	setHeaders();														// setea los headers de la respuesta
    if(_request->getMethod() == GET || _code != 200) 	// si el método del request no es HEAD y el método del request es GET o el código no es 200
    {
		_response_str.append(_response_body_str);										// agrega el body a la respuesta	
	}
	_responseString = _response_str;
	
	//std::stringstream responseStream;

	//responseStream << "HTTP/1.1 ";
	//responseStream << "200";
	//responseStream << " OK\r\n";
	//responseStream << "Content-Type: text/plain\r\n";
	//responseStream << "Content-Length: " << 29 << "\r\n";
	//responseStream << "\r\n";
	//responseStream << "Hello, world! from socket " << "\r\n\r\n";
	//_responseString = _response_str.append(responseStream.str());
//	_responseString = responseStream.str();

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
