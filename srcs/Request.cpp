#include "../includes/lib.hpp"

Request::Request(Client *client, Server_config *config, struct Epoll_events *events)
:_client(client), _config(config), _events(events)
{
	if (DEBUG)
		std::cout << "Request constructor" << std::endl;
	_initMethodStr();
	_path = "";
	_query = "";
	_fragment = "";
	_body_str = "";
	_error_code = 0;
	_uri_size = 0;
	_chunk_size = 0;
	_method = NONE;
	_ix = 0;										//	
	//_ix = 0;
	_fillStatus = get_First;					  // el estado inicial es get_First 
	_headers_parsed = false;
	_get_body_flag = false;
	_body_parsed = false;
	_chunked_body_flag = false;
	_body_size = 0;
	_temp = "";
	_header_name_temp = "";
	_multiform_flag = false;
	_boundary = "";
	(void)_events;
}

Request::~Request()
{
	if (DEBUG)
			std::cout << "Request destructor" << std::endl;
}

void	Request::_initMethodStr()		// incluir tanto aquí como en la enumeración los métodos permitidos
{
	_methods_str[::GET] = "GET";
	_methods_str[::POST] = "POST";
	_methods_str[::DELETE] = "DELETE";
}

void Request::setClient(Client* client) {
    _client = client;  // Guarda el puntero al objeto Client
}

int		Request::getErrorCode(){
    return (this->_error_code);
}

std::string	Request::getcgiString()
{
	return _cgiString;
}

void Request::printParsed()
{
	std::cout << _config << std::endl;
	std::cout << "Parsed" << std::endl;
	std::cout << "Method: " << _methods_str[_method] << std::endl;
	std::cout << "Path: " << _path << std::endl;
	std::cout << "Query: " << _query << std::endl;
	std::cout << "Fragment: " << _fragment << std::endl;
	std::cout << "Server Name: " << _server_name << std::endl;
	std::cout << "Body: " << _body_str << std::endl;
	std::cout << "Headers: " << std::endl;
	for(std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
		std::cout << "Clave: " << it->first << " Valor: " << it->second << std::endl;
	}
	std::cout << "Boundary: " << _boundary << std::endl;
	std::cout << "Multiform Flag: " << _multiform_flag << std::endl;
	std::cout << "Body Size: " << _body_size << std::endl;
	std::cout << "Chunked Body Flag: " << _chunked_body_flag << std::endl;
	std::cout << "Get Body Flag: " << _get_body_flag << std::endl;
	std::cout << "_headers.size(): " << _headers.size() << std::endl;
	std::cout << "URI Size: " << _uri_size << std::endl;
}

Methods  &Request::getMethod(){
    return (_method);
}

std::string &Request::getPath(){
    return (_path);
}

std::string &Request::getQuery(){
    return (_query);
}

std::string &Request::getFragment(){
    return (_fragment);
}

std::string Request::getHeader(std::string const &name){
    return (_headers[name]);
}

const std::map<std::string, std::string> &Request::getHeaders() const{
	return (this->_headers);
}

std::string Request::getMethodStr(){
	return (_methods_str[_method]);
}

/**
 * @brief devolver a _body_str
 * 
 * @return std::string& 
 */
std::string &Request::getBody(){
    return (_body_str);
}

std::string     Request::getServerName(){
    return (this->_server_name);
}

int	Request::getPort(){
	return _client->_port;
}

bool    Request::getMultiformFlag(){
    return (this->_multiform_flag);
}

std::string     &Request::getBoundary(){
    return (this->_boundary);
}

/**
 * @brief Almacenar los datos limpios sin espacios al final y al principio y con los 
 * nombres en minúsculas, (RFC 7230 especifica que deben ser case-insensitive por lo que 
 * nos pueden llegar en minúsculas o mayúsculas.
 * Se puede separar en dos funciones, si es necesaria usar algo similar en otro sitio se pueden separar 
 * @param name 
 * @param value 
 */
void	Request::saveHeader(std::string &name, std::string &value){
	static const char* spaces = " \t";
	value.erase(0, value.find_first_not_of(spaces));
	value.erase(value.find_last_not_of(spaces) + 1);
	for (size_t i = 0; i < name.length(); ++i)		
		name[i] = std::tolower(name[i]);
	_headers[name] = value;
}

void Request::_returnErr(int err, std::string msg,uint8_t charRead = 0){
	_error_code = err;
	std::cout << "Error = " << err <<": "<< msg << ": " << charRead << std::endl;
}

/**

 * Checks if character is allowed to be in a URI
 * Characters allowed as specifed in RFC:
   Alphanumeric: A-Z a-z 0-9
   Unreserved: - _ . ~
   Reserved:  * ' ( ) ; : @ & = + $ , / ? % # [ ]

 **/
bool	allowedURIChar(uint8_t ch){
	if ((ch >= '#' && ch <= ';') || 
		(ch >= '?' && ch <= '[') || 
		(ch >= 'a' && ch <= 'z') ||
	  ch == '!' || ch == '=' || 
	  ch == ']' || ch == '_' || 
	  ch == '~')
		return (true);
	return (false);
}

/**
 * @brief compueba el path de la URI y en el caso de encontrar .. va restando niveles para devolver 1				// Pendiente de revisar y ver opciones, aunque de lo que he visto es la mejor
 * si el path lleva a un directorio superior al raíz o incluso si intenta subir su nivel en el camino
 * 
 * @param path 
 * @return true 
 * @return false 
 */
bool	checkPath(std::string path){
	std::string tmp(path);							 // se copia el path en tmp
	// if (DEBUG)
	// {
	// 	std::cout << "checkPath_______________________________________-" << std::endl;
	// 	std::cout << "path = " << path << std::endl;
	// }
	char *res = strtok((char*)tmp.c_str(), "/");		// se usa strtok para separar el path por las / y se almacena en res la primera sección
	int pos = 0;										// se inicializa pos a 0
	while (res != NULL)								// mientras res no sea NULL  
	{
		if (!strcmp(res, ".."))						// si res es igual a .. se decrementa pos
			pos--;
		else
			pos++;									 // si no, se incrementa pos
		if (pos < 0)
		{
			return (1);								// si pos es menor que 0 se devuelve 1
		}
		
		res = strtok(NULL, "/");						// se sigue con la siguiente sección separada por / o el final de la cadena
	}
	return (0);
}

bool	isValidChar(uint8_t ch){
	if (ch == '!' || (ch >= '#' && ch <= '\'') || 
		ch == '*'|| ch == '+' ||
		ch == '-'  || ch == '.' ||
		(ch >= '0' && ch <= '9') || 
		(ch >= 'A' && ch <= 'Z') || 
		(ch >= 'a' && ch <= 'z') ||
		(ch >= '^' && ch <= '`') ||
		ch == '|')
		return (true);
	return (false);
}

/**
 * @brief Analiza los headers y gestiona varios flags determina si:
 * si hay un host se asigna el valor del header a _server_name
 * si hay un content-length que determina el tamaño del body,
 * Si el body es chunked (es decir si va en fragmentos de tamaño variable), 
 * si hay un content-type con multipart/form-data se asigna el valor del boundary a _boundary
 * si hay un content-type con multipart/form-data se asigna true a _multiform_flag
 */
void	Request::_handle_headers(){
	
	if (DEBUG)
	if (_headers.count("host"))
	{
		size_t pos = _headers["host"].find_first_of(':');		// se busca el primer : para separar el nombre del servidor del puerto
		_server_name = _headers["host"].substr(0, pos);	
	}
	if (_headers.count("content-length"))   
	{
		_get_body_flag = true;
		std::istringstream iss(_headers["content-length"]);
		iss >> _body_size;
		if (_body_size > _config->body_size)
		{
			if (DEBUG)
			{
				std::cout << "Body size: " << _body_size << std::endl;
				std::cout << "Client max body size: " << _config->body_size << std::endl;	
			}
			return (_returnErr(413, "Request Entity Too Large", 0));
		}
	}
	if (_headers.count("transfer-encoding"))
	{
		_get_body_flag = true;
		if (_headers["transfer-encoding"].find("chunked") != std::string::npos)		// busca la cadena completa
			_chunked_body_flag = true;
	}
	if (_headers.count("content-type") && _headers["content-type"].find("multipart/form-data") != std::string::npos)
	{
		size_t pos = _headers["content-type"].find("boundary=");
		if (pos != std::string::npos)
			//this->_boundary = _headers["content-type"].substr(pos + 9, _headers["content-type"].size());
			this->_boundary = _headers["content-type"].substr(pos + 9);		// se puede quitar el segundo argumento pero mejor revisar en caso de problemas
		this->_multiform_flag = true;
		std::cout << "_multiform_flag: " << _multiform_flag << std::endl;
		std::cout << "Boundary: " << _boundary << std::endl;

	}
}

/**
 * Checks the value of header "Connection". If keep-alive, don't close the connection.
 * @brief comprobar si hay algun header con nombre connection y si su valor es close
 * devuelve false, devuelve true en caso contrario 
 * 
 * @return true 
 * @return false 
 */
bool        Request::isKeepAlive(){
    if (_headers.count("connection"))
    {
        if (_headers["connection"].find("close", 0) != std::string::npos)
            return (false);
    }
    return (true);
}

size_t hexStringToInt(const std::string& hexStr) {
    size_t result = 0;
    std::stringstream ss(hexStr);
    ss >> std::hex >> result;
    return result;
}

bool Request::isValidUri()
{	

	std::stringstream ss;
	ss << _client->_port;
	std::string portStr = ss.str();
	std::string scheme = "http://";
	std::string uriTotal = scheme + _client->_host  + ":" + portStr + _path + _query + _fragment;
	if (DEBUG)
	{	
		std::cout << Yellow << "URI Total: " << uriTotal << White << std::endl;
		std::cout << "URI Total Length: " << uriTotal.length() << std::endl;
		std::cout << "URI_MAX_LENGTH: " << URI_MAX_LENGTH << White<< std::endl;
	}

	if (uriTotal.length() > URI_MAX_LENGTH)
		return false;
	return true;
}

void	Request::fillRequest(char *dt, size_t bytesRead)
{
	if (DEBUG){
		std::cout << "fillRequest" << std::endl;
		std::cout << "bytesRead = " << bytesRead << std::endl;
		std::cout << "dt = " << dt << std::endl;
	}
	u_int8_t charRead;														// unsigned char  charRead para igualar a dt[i]   
	std::string protocol = "HTTP/1.1";										// string HTTP para comparar con el protocolo HTTP/1.1 
	static std::stringstream s;												// stringstream s para almacenar los datos recibidos como stringstream en el body con chunked fla
	for (size_t i = 0; i < bytesRead; ++i)									// itera sobre los datos recibidos
	{
		charRead = dt[i];													// caracter igual a dt[i] para usar su valor en las comparativas
		switch (_fillStatus)												// switch para comparar el estado actual del objeto Request la priemra vez es Request_Line
		{
			//////	REQUEST FIRST LINE OR PETITION LINE		//////												
			case get_First:						 							// lee el primer caracter de la petición y lo compara con los valores posibles de los métodos GET, POST, DELETE, PUT, HEAD
			{
				if (charRead == 'G')				  						// si el caracter es G
				  _method = GET;					 						// el método es GET
				else if (charRead == 'P')			 						// si e P cambia 
					_method = POST;											// el método es POST en caso de aceptar put hay que modificar para añadir un paso más cuando encuentra una p
				else if (charRead == 'D')			 						// si es D
					_method = DELETE;				 						// el método es DELETE
				else														// si no es ninguno de los anteriores	
					return (_returnErr(501, "Method not Implemented, detected in char", charRead)); // lanza un error, el error es 501;
				_ix++;														// incrementa el indice para comparar con el siguiente caracter del método recibido
				_fillStatus = get_Method;		  							// Si el método es get, delete o head _fillStatus lo pasa a get_Method
				break ;	 
			}
			case get_Method:
			{
				if (charRead == _methods_str[_method][_ix])   				// si el caracter es igual al caracter del método, al que se referencia usando los valores de la vuelta anterior del bucle y los valores inciales
					_ix++;
				else
					return (_returnErr(501, "Method Error, charRead is = ", charRead)); // lanza un error, el error es 501;
				if (size_t(_ix) == _methods_str[_method].length())			// en cada vuelta se compara el indice que el tamño para ver si hemos llegado al final  
					_fillStatus = get_First_Space;					 		// si hemos llegado al final se pasa al siguiente estado Request_Line_First_Space
				break ;														// y se sale del switch para analizar el siguiente caracter de la petición que debe ser un espacio
			}
			case get_First_Space:									 		// si el caracter debe ser un espacio
			{																// si no es un espacio lanza un error
				_ix = 0;													// se reinicia el indice	
				if (charRead != ' ')																	 
					return (_returnErr(400, "Bad charRead", charRead)); 		// lanza un error, el error es 501;
 				_fillStatus = get_First_Slash;						  		// si es un espacio se pasa al siguiente estado Request_Line_URI_Path_Slash
				continue ;													// usa continue para saltar a la siguiente iteración del for sin pasara por el codigo después del switch
			}   
			case get_First_Slash:								  			// una vez analizado todo el método pasamos a analizar la / 
			{
				if (charRead == '/')										// si el caracter es la / 
				{
					_fillStatus = get_URI_Path;								// se pasa al siguiente estado Request_Line_URI_Path 
					_temp.clear();										  	// se limpia el string temp. el methos queda almacenado en _method
				}
				else
					return (_returnErr(400, "Bad charRead", charRead)); 		// lanza un error, el error es 400;
				break ;
			}
			case get_URI_Path:												// si _fillStatus es esperamos rellenar el _path o en su defecto la versión
			{
				if (charRead == ' ')										// si el caracter es un espacio		
				{
					_fillStatus = get_Protocol;							  	// se pasa al siguiente estado get_Version
					_path.append(_temp);									// hacemos un append en _path de lo que tengamos en temp		
					_temp.clear();										  	// limpiamos temp
					// if (DEBUG)
					// 	std::cout << "_path = " << _path << std::endl;
					if ( !isValidUri())								// si el tamaño de la URI es mayor que el máximo permitido en este caso 4096 definido en header
						return (_returnErr(414, "URI Too Long", charRead)); 	// lanza un error, el error es 400;
					if (checkPath(_path))									// checkPath comprueba que wl path no vaya por encima de la raiz					return (_returnErr(400, "wrong URI location", charRead));
						return (_returnErr(400, "wrong path address", charRead));	// salimos de la función
					continue ;												// saltamos a la siguiente iteración del for
				}
				else if (charRead == '?')									// si el caracter es un ? 
				{
					_fillStatus = get_URI_Query;							// se pasa al siguiente estado get_URI_Query
					_path.append(_temp);									// hacemos un append en _path de lo que tengamos en temp		
					if (checkPath(_path))									// checkPath comprueba que wl path no vaya por encima de la raiz					return (_returnErr(400, "wrong URI location", charRead));
						return (_returnErr(400, "wrong path address", charRead));	// salimos de la función
					_temp.clear();										  	// limpiamos temp
					continue ;												// saltamos a la siguiente iteración del for	
				}
				else if (charRead == '#')									// si el caracter es un #
				{
					_fillStatus = get_URI_Fragment;							// se pasa al siguiente estado get_URI_Fragment
					_path.append(_temp);									// hacemos un append en _path de lo que tengamos en temp		
					if (checkPath(_path))									// checkPath comprueba que wl path no vaya por encima de la raiz					return (_returnErr(400, "wrong URI location", charRead));
						return (_returnErr(400, "wrong path address", charRead));	// salimos de la función
					_temp.clear();										  	// limpiamos temp
					continue ;												// saltamos a la siguiente iteración del for
				}
				else if (!allowedURIChar(charRead))							// si no es un caracter permitido según la RFC
					return (_returnErr(400, "character not allowed", charRead)); // lanza un error, el error es 400;				
				else if ( i > URI_MAX_LENGTH)								// si el tamaño de la URI es mayor que el máximo permitido en este caso 4096 definido en header
					return (_returnErr(414, "URI Too Long", charRead)); 	// lanza un error, el error es 400;
				break ;														// se sale del switch									 
			}
			case get_URI_Query: // (?)
			{
				if (charRead == ' ')										  	// si es un espacio hemos llegado al fin de la URI Query
				{
				_fillStatus = get_Protocol;								 		// se pasa al siguiente estado get_Version
				_query.append(_temp);											// se añade el contenido de temp a _query
				_temp.clear();
				if ( !isValidUri())								// si el tamaño de la URI es mayor que el máximo permitido en este caso 4096 definido en header
					return (_returnErr(414, "URI Too Long", charRead)); 	// lanza un error, el error es 400;
				continue ;												 		// saltamos a la siguiente iteración del for  
				}
				else if (charRead == '#')								 	// si es un #
				{
				_fillStatus = get_URI_Fragment;						 		// se pasa al siguiente estado get_URI_Fragment
				_query.append(_temp);									 	// se añade el contenido de temp a _query
				_temp.clear();										  	 	// se limpia temp
				continue ;												 	// saltamos a la siguiente iteración del for
				}
				else if (!allowedURIChar(charRead))
					return (_returnErr(400, "character not allowed", charRead)); // lanza un error, el error es 400;												// salimos de la función 
				else if ( i > URI_MAX_LENGTH)
					return (_returnErr(414, "URI Too Long", charRead)); 		// lanza un error, el error es 400;
				break ;
			}
			case get_URI_Fragment: // (#)									// si es get_URI_Fragment se rellena _fragment
			{
				if (charRead == ' ')										// si es un espacio
				{
					_fragment.append(_temp);								// se añade el contenido de temp a _fragment
					_temp.clear();											// se limpia temp
					_fillStatus = get_Protocol;								// se pasa al siguiente estado get_Version
					if ( !isValidUri())								// si el tamaño de la URI es mayor que el máximo permitido en este caso 4096 definido en header
						return (_returnErr(414, "URI Too Long", charRead)); 	// lanza un error, el error es 400;
					continue ;												// saltamos a la siguiente iteración del for
				}
				else if (!allowedURIChar(charRead))							// si no es un caracter permitido según la RFC
					return (_returnErr(400, "character not allowed", charRead)); // lanza un error, el error es 400;												// salimos de la función 
				else if ( i > URI_MAX_LENGTH)								  // si el tamaño de la URI es mayor que el máximo permitido en este caso 4096 definido en header
					return (_returnErr(414, "URI Too Long", charRead)); 		// lanza un error, el error es 400;
				break ;														// se sale del switch
			}
			case get_Protocol:												// si _fillStatus es get_Version se rellena la versión
			{
				if (charRead != protocol[_ix])								// usamos la versión para comparar con el protocolo HTTP/1.1
					return (_returnErr(400, "wrong protocol or protocol not admitted", charRead));		// salimos de la función
				_ix++;
				if (size_t(_ix) == protocol.length())						// si el caracter es 
					_fillStatus = get_CR;									// si hemos llegado al final se pasa al siguiente estado Request_Line_HTTP_Slash
				break ;														// se sale del switch
			}
			case get_CR:												 	// en caso de Parsed
			{
				_ix= 0;														// se reinicia el indice
				if (charRead != '\r')										// si el caracter es distinto de \r
					return (_returnErr(400, "wrong CR", charRead));			// salimos de la función
				_fillStatus = get_LF;										// si el caracter es \r se pasa al siguiente estado Request_Line_HTTP_Slash
				break ;														// se sale del switch
			}
			case get_LF:										  			// buscamos el salto de línea
			{
				if (charRead != '\n')										// si el caracter no es un salto de línea
					return (_returnErr(400, "wrong CR", charRead));			// salimos de la función
				_fillStatus = header_Name_Start;							// si el caracter es un salto de línea cambiamos el estado a Field_Name_Start		
				_temp.clear();										  		// limpiamos temp
				continue ;													// saltamos a la siguiente iteración del for
			}
			//////									END OF REQUEST FIRST LINE OR PETITION LINE									//////	
			
			//////////////////////////								HEADERS 								//////////////////////////
			case header_Name_Start:											// buscamos el nombre del campo
			{
				if (charRead == '\r')										// si el caracter es un retorno de carro
					_fillStatus = headers_End;								// cambiamos el estado a Fields_End ya que se han encontrado juntos \r\n\r	
				else if (isValidChar(charRead))								// si el caracter es un token válido 
					_fillStatus = header_Name;								// cambiamos el estado a Field_Name
				else														// si no es un token
					return (_returnErr(400, "wrong char in header name", charRead));	
				break ;														// salimos del switch
			}	
			case headers_End:												// sive para comprobar si se han acabado los campos	
			{
				if (charRead == '\n')										// si el caracter es un salto de línea
				{
					_temp.clear();										  	// limpiamos temp
					_headers_parsed = true;									// el flag de campos se pone a true ya que se han encontrado juntos \r\n
					// if (DEBUG){
					// 	std::cout << "Headers parsed" << std::endl;
					// 	// Imprimir el mapa
					// 	for(std::map<std::string, std::string>::iterator it = _headers.begin(); it != _headers.end(); ++it) {
					// 		std::cout << "Clave: " << it->first << " Valor: " << it->second << std::endl;
					// 	}
					// }
					_handle_headers();										// se llama a la función _handle_headers Pendiente de realizar 			
					if (_get_body_flag == 1)								// si el flag de body es 1, damos el estado como parseado
					{
						_fillStatus = get_Body;								// cambiamos el estado a Message_Body		<-- incluido para hace bypass a chunked
						if (_chunked_body_flag == true)						// si el flag de chunked es true				
							_fillStatus = Chunk_Length_Begin;				// cambiamos el estado a Chunk_Length_Begin
						else												// si no			
							_fillStatus = get_Body;							// cambiamos el estado a Message_Body   <--- Pendiente de revisar e incluir
					}				  
					else													// si no
						_fillStatus = Parsed;								// cambiamos el estado a Parsed
					continue ;												// saltamos a la siguiente iteración del for
					}
				else																			// si el caracter no es un salto de línea   
					return (_returnErr(400, "wrong char at the end of headers", charRead));		// si el caracter no es válido, lanza un error, el error 400;salimos del switch 
				break ;																			// salimos del switch							
			}
			case header_Name:												// buscamos el nombre del campo para los headers		
			{
				if (charRead == ':')								 		// si el caracter es : es el delimitador entre el nombre y el valor del header
 				{
					_fillStatus = header_Value;							  	// cambiamos el estado a Field_Value
					_header_name_temp = _temp;								// almacenamos el nombre del campo en _header_name_temp
					_temp.clear();								  			// limpiamos temp
					continue ;										 		// saltamos a la siguiente iteración del for	
				}
				else if (!isValidChar(charRead))								  	
					return (_returnErr(400, "wrong char in header name", charRead));	// si el caracter no es válido, lanza un error, el error 400;salimos del switch 
				break ;																
			}
			case header_Value:												// en este estado guardamos el contenido de los valroes de los headers
			{
				if (charRead == '\r')										// si el caracter es un retorno de carro
				{
					saveHeader(_header_name_temp, _temp);					// se llama a la función saveHeader con los valores de _header_name_temp y _temp
					_temp.clear();										 	// se limpia _temp
					_header_name_temp.clear();								// se limpia _header_name_temp
					_fillStatus = header_Value_End;							// cambiamos el estado a Field_Value_End
					continue ;												// saltamos a la siguiente iteración del for
				}
				break ;														// salimos del switch
			}
			case header_Value_End:											// en este estado se comprueba si se han acabado los valores de los headers
			{
				if (charRead == '\n')										// si el caracter es un salto de línea
				{
					_fillStatus = header_Name_Start;						// cambiamos el estado a Field_Name_Start
					continue ;												// saltamos a la siguiente iteración del for
				}
				else														// si el caracter no es un salto de línea
					return (_returnErr(400, "wrong char after header value", charRead));	// si el caracter no es válido, lanza un error, el error 400;salimos del switch 
				break ;														// salimos del switch
			}
			//////									END OF HEADERS 								//////
		
			//////									MESSAGE BODY 								//////
			case get_Body:													// en caso de Message_Body
			{
				if (_body.size() < _body_size )							  	// si el tamaño de _body es menor que _body_size
					_body.push_back(charRead);								// añadimos el caracter a _body
				if (_body.size() == _body_size )							// si el tamaño de _body es igual a _body_size
				{
					_body_parsed = true;									// el flag de body_done se pone a true
					_fillStatus = Parsed;									// cambiamos el estado a Parsed
				}
				break ;														// salimos del switch
			}	
			case Chunk_Length_Begin:										// en este estado se busca el inicio de la longitud de los chunks
			{
				if (!isxdigit(charRead))								  	// si el caracter no es un dígito hexadecimal
					return (_returnErr(400, "wrong chunk size", charRead));	// si el caracter no es válido, lanza un error, el error 400;salimos del switch 
				s.str("");													// iniciamos s como una cadena vacía 
				s.clear();													// limpiamos las flags del stream para que no haya errores de entrada y salida ni bloquee en caso de error
				s << charRead;												// añadimos el caracter a s
				s >> std::hex >> _chunk_size;								// convertimos el valor de s a hexadecimal y lo almacenamos en _chunk_size
				if (_chunk_size == 0)										// si la longitud del chunk es 0
					_fillStatus = Chunk_Length_CR;							// cambiamos el estado a Chunk_Length_CR
				else														// si no
					_fillStatus = Chunk_Length;								// cambiamos el estado a Chunk_Length
				continue ;													// saltamos a la siguiente iteración del for
			} 
			case Chunk_Length:												// en caso de Chunk_lenght	
			{
				if (isxdigit(charRead) != 0)								// si el caracter es un dígito hexadecimal
				{
					int temp_len = 0;											// Asignamos a temp_len un valor de 0
					s.str("");												 	// Asignamos el valor de cadena vacía a s, esto eliminará cualquier valor que tenga
					s.clear();												 	// limpiamos las flags del stream para que no haya errores de entrada y salida ni bloquee en caso de error
					s << charRead;												// añadimos el caracter a s
					s >> std::hex >> temp_len;								 	// convertimos el valor de s a hexadecimal y lo almacenamos en temp_len
					_chunk_size *= 16;											// multiplicamos _chunk_size por 16
					_chunk_size += temp_len;								 	// sumamos temp_len a _chunk_size
				}
				else if (charRead == '\r')										// si el caracter es un retorno de carro
					_fillStatus = Chunk_Length_LF;								// cambiamos el estado a Chunk_Length_LF
				else															// si no
					_fillStatus = Chunk_Ignore;									// cambiamos el estado a Chunk_Ignore
				continue ;													 // saltamos a la siguiente iteración del for						
			}
			case Chunk_Length_CR:											// en caso de Chunk_Length_CR
			{
				if (charRead == '\r')										// si el caracter es un retorno de carro
					_fillStatus = Chunk_Length_LF;							// cambiamos el estado a Chunk_Length_LF
				else														// si no
					return (_returnErr(400, "wrong chunk size", charRead));	// si el caracter no es válido, lanza un error, el error 400;salimos del switch 
				continue ;													// saltamos a la siguiente iteración del for
			}
			case Chunk_Length_LF:											// en caso de Chunk_Length_LF
			{
				if (charRead == '\n')										// si el caracter es un salto de línea
				{
					if (_chunk_size == 0)									// si la longitud del chunk es 0
						_fillStatus = Chunk_End_CR;							// cambiamos el estado a Chunk_End_CR
					else													// si no
						_fillStatus = Chunk_body;							// cambiamos el estado a Chunk_body
				}
				else														// si el caracter no es un salto de línea
					return (_returnErr(400, "wrong chunk LF", charRead));	// si el caracter no es válido, lanza un error, el error 400;salimos del switch 
				continue ;													// saltamos a la siguiente iteración del for
			}
			case Chunk_Ignore:											 	// en caso de Chunk_Ignore				  
			{
				if (charRead == '\r')										// si el caracter es un retorno de carro
					_fillStatus = Chunk_Length_LF;							// cambiamos el estado a Chunk_Length_LF
				continue ;													// saltamos a la siguiente iteración del for
			}
			case Chunk_body:												// en caso de Chunk_body
			{
				_body.push_back(charRead);									// añadimos el caracter a _body
				--_chunk_size;												// decrementamos _chunk_size para ir contando los caracteres ya recibidos
				if (_chunk_size == 0)										// si _chunk_size es 0
					_fillStatus = Chunk_body_CR;							// cambiamos el estado a Chunk_body_CR esperamos un retorno de carro
				continue ;													// saltamos a la siguiente iteración del for
			}
			case Chunk_body_CR:												// en caso de Chunk_body_CR
			{
				if ( charRead == '\r')										// si el caracter es un retorno de carro
					_fillStatus = Chunk_body_LF;							// cambiamos el estado a Chunk_body_LF, esperamos un salto de línea
				else														// si no
					return (_returnErr(400, "wrong chunk CR", charRead));	// si el caracter no es válido, lanza un error, el error 400;salimos del switch 
				continue ;													// saltamos a la siguiente iteración del for
			}
			case Chunk_body_LF:												//  en caso de Chunk_body_LF
			{
				if ( charRead == '\n')										// si el caracter es un salto de línea
					_fillStatus = Chunk_Length_Begin;						// cambiamos el estado a Chunk_Length_Begin   esperamos un nuevo chunk
				else														// si no
					return (_returnErr(400, "wrong chunk LF", charRead));	// si el caracter no es válido, lanza un error, el error 400;salimos del switch 
				continue ;													// saltamos a la siguiente iteración del for
			}
			case Chunk_End_CR:												// en caso de Chunk_End_CR
			{
				if (charRead != '\r')										// si el caracter no es un retorno de carro
					return (_returnErr(400, "wrong chunk CR", charRead));	// si el caracter no es válido, lanza un error, el error 400;salimos del switch 
				_fillStatus = Chunk_End_LF;									// si el caracter es un retorno de carro cambiamos el estado a Chunk_End_LF		
				continue ;													// saltamos a la siguiente iteración del for						 
			}
			case Chunk_End_LF:												// en caso de Chunk_End_LF
			{
				if (charRead != '\n')										 // si el caracter no es un salto de línea	
				{
					_error_code = 400;										 // el error es 400				
					std::cout << "Bad charRead (Chunk_End_LF)" << std::endl; // mensaje de error distinto
					return ;													// salimos de la función
				}
				_body_parsed = true;										 // el flag de body_done se pone a true
				_fillStatus = Parsed;										 // cambiamos el estado a Parsed 
				continue ;													 // saltamos a la siguiente iteración del for		
			}
		
			///////									END OF MESSAGE BODY 								//////

			case Parsed:												 	// en caso de Parsed
			{	
				if (DEBUG)	
					std::cout << "Request parsed_________________________________________________" << std::endl;	// Ver si es realmente necesario				std::cout << "Method = " << _method << std::endl;
				return ;													// salimos de la función
			}
		}
		_temp += charRead;									                // se añade el caracter al string temp
	}
	if( _fillStatus == Parsed)												// si el estado es Parsed
	{
		if (DEBUG)	
			printParsed();						// se imprime el path
		_body_str.append((char*)_body.data(), _body.size());				// Se incluye el body en _body_str
	}																		// es más eficiente que hacer append de un char a un string
}

void Request::fillCgi(char *buffer)
{
	// _cgiString.append(buffer);
	_cgiString = buffer;
}

void	Request::reset(){
	_path.clear();
	_error_code = 0;
	_query.clear();
	_fragment.clear();
	_method = NONE;
	_ix = 0;
	_fillStatus = get_First;
	_uri_size = 0;
	_body_size = 0;
	_chunk_size = 0x0;
	_temp.clear();
	_body_str = "";
	_cgiString = "";
	_header_name_temp.clear();
	_headers.clear();
	_server_name.clear();
	_body.clear();
	_boundary.clear();
	_headers_parsed = false;
	_get_body_flag = false;
	_body_parsed = false;
	_chunked_body_flag = false;
	_multiform_flag = false;
}


/**
 * @brief compara si _state == parsing_done
 * 
 * @return true si es igual, false si no lo es 
 */
bool    Request::isParsed()
{
	if (_fillStatus == 	Parsed)
	    return (true);
	return (false);
}
