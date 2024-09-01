#include "../includes/Request.hpp"

Request::Request(){
	std::cout << "Request constructor" << std::endl;
	_initMethodStr();
	_path = "";
	// _query = "";
	// _fragment = "";
	// _body_str = "";
	_error_code = 0;
	// _chunk_length = 0;
	_method = NONE;
	_method_ix = 1;
	//_http_ix = 0;
	_fillStatus = get_First;					   // el estado inicial es get_First 
	// _fields_done_flag = false;
	// _body_flag = false;
	// _body_done_flag = false;
	// _chunked_flag = false;
	// _body_length = 0;
	// _storage = "";
	// _key_storage = "";
	// _multiform_flag = false;
	// _boundary = "";
}

Request::~Request() {
	std::cout << "Request destructor" << std::endl;
}

void	Request::_initMethodStr()		// incluir tanto aquí como en la enumeración los métodos permitidos
{
	_methods_str[::GET] = "GET";
	_methods_str[::POST] = "POST";
	_methods_str[::DELETE] = "DELETE";
}

void Request::returnErr(int err, std::string msg,uint8_t charRead){
	//_error_code = err;
	std::cout << "Error = " << err <<": "<< msg << ": " << charRead << std::endl;
}

/**

 * Checks if character is allowed to be in a URI
 * Characters allowed as specifed in RFC:
   Alphanumeric: A-Z a-z 0-9
   Unreserved: - _ . ~
   Reserved:  * ' ( ) ; : @ & = + $ , / ? % # [ ]

 **/
bool    allowedURIChar(uint8_t ch){
    if ((ch >= '#' && ch <= ';') || 
		(ch >= '?' && ch <= '[') || 
		(ch >= 'a' && ch <= 'z') ||
       ch == '!' || ch == '=' || 
	   ch == ']' || ch == '_' || 
	   ch == '~')
        return (true);
    return (false);
}

void	Request::fillRequest(char *dt, size_t bytesRead)
{
	std::cout << "fillRequest" << std::endl;
	std::cout << "bytesRead = " << bytesRead << std::endl;
 	std::cout << "dt = " << dt << std::endl;
	u_int8_t charRead;					 						// unsigned char  charRead para igualar a dt[i]   
	std::string version = "HTTP/";								// string HTTP para comparar con el protocolo HTTP/1.1 
	std::cout << "version = " << version << std::endl; 
// //	static std::stringstream s;							// stringstream s para almacenar los datos recibidos como stringstream en el body con chunked fla
	for (size_t i = 0; i < bytesRead; ++i)						// itera sobre los datos recibidos
	{
		charRead = dt[i];										// caracter igual a dt[i] para usar su valor en las comparativas
		std::cout << "charRead = " << charRead << std::endl;
		std::cout << "_fillStatus = " << _fillStatus << std::endl;
		switch (_fillStatus)									// switch para comparar el estado actual del objeto HttpRequest la priemra vez es Request_Line
		{
			case get_First:						  			// lee el primer caracter de la petición y lo compara con los valores posibles de los métodos GET, POST, DELETE, PUT, HEAD
	 		{
				if (charRead == 'G')				   			// si el caracter es G
				   _method = GET;					  			// el método es GET
				else if (charRead == 'P')			  			// si e P cambia 
					_method = POST;								// el método es POST en caso de aceptar put hay que modificar para añadir un paso más cuando encuentra una p
				else if (charRead == 'D')			  			// si es D
					_method = DELETE;				  			// el método es DELETE
				else											// si no es ninguno de los anteriores	
				{
					return (returnErr(501, "Method not Implemented, detected in char", charRead)); // lanza un error, el error es 501;
				}
				_fillStatus = get_Method;		   			// Si el método es get, delete o head _fillStatus lo pasa a get_Method
				break ;	  
			}
			case get_Method:
			{
				if (charRead == _methods_str[_method][_method_ix])   // si el caracter es igual al caracter del método, al que se referencia usando los valores de la vuelta anterior del bucle y los valores inciales
					_method_ix++;
				else
					return (returnErr(501, "Method Error Request_Line and charRead is = ", charRead)); // lanza un error, el error es 501;
				if (size_t(_method_ix) == _methods_str[_method].length())	// en cada vuelta se compara el indice que el tamño para ver si hemos llegado al final  
					_fillStatus = get_First_Space;					  // si hemos llegado al final se pasa al siguiente estado Request_Line_First_Space
				break ;														 // y se sale del switch para analizar el siguiente caracter de la petición que debe ser un espacio
			}
			case get_First_Space:									  // si el caracter debe ser un espacio
			{																   // si no es un espacio lanza un error
				if (charRead != ' ')																	  
					return (returnErr(400, "Bad charRead", charRead)); // lanza un error, el error es 501;
 				_fillStatus = get_First_Slash;						   // si es un espacio se pasa al siguiente estado Request_Line_URI_Path_Slash
 				_fillStatus = Parsed;						   
				continue ;													  // usa continue para saltar a la siguiente iteración del for sin pasara por el codigo después del switch
			}   
			case get_First_Slash:								   // una vez analizado todo el método pasamos a analizar la / 
			{
				if (charRead == '/')										   // si el caracter es la / 
				{
					_fillStatus = get_URI_Path;							 // se pasa al siguiente estado Request_Line_URI_Path 
					_temp.clear();										   // se limpia el string storage. el methos queda almacenado en _method
				}
				else
					return (returnErr(400, "Bad charRead", charRead)); // lanza un error, el error es 400;
				break ;
			}
			case get_URI_Path:												// si _fillStatus es esperamos rellenar el _path o en su defecto la versión
			{
				if (charRead == ' ')										// si el caracter es un espacio		
				{
					_fillStatus = get_Version;							   	// se pasa al siguiente estado get_Version
					_path.append(_temp);									// hacemos un append en _path de lo que tengamos en storage		 
					_temp.clear();										   	// limpiamos storage
					continue ;												// saltamos a la siguiente iteración del for
				}
				else if (charRead == '?')									// si el caracter es un ? 
				{
					_fillStatus = get_URI_Query;							// se pasa al siguiente estado get_URI_Query
					_path.append(_temp);									// hacemos un append en _path de lo que tengamos en storage
					_temp.clear();										   	// limpiamos storage
					continue ;												// saltamos a la siguiente iteración del for	
				}
				else if (charRead == '#')									// si el caracter es un #
				{
					_fillStatus = get_URI_Fragment;						 	// se pasa al siguiente estado get_URI_Fragment
					_path.append(_temp);									// hacemos un append en _path de lo que tengamos en storage
					_temp.clear();										   	// limpiamos storage
					continue ;												// saltamos a la siguiente iteración del for
				}
				else if (!allowedURIChar(charRead))							// si no es un caracter permitido según la RFC
					return (returnErr(400, "character not allowed", charRead)); // lanza un error, el error es 400;
				else if ( i > URI_MAX_LENGTH)								   // si el tamaño de la URI es mayor que el máximo permitido en este caso 4096 definido en header
					return (returnErr(414, "URI Too Long", charRead)); // lanza un error, el error es 400;
				break ;														 // se sale del switch									  
			}
			case get_Version:
			{
				//pendiente de incluir
			}
			case get_URI_Query:
			{
				//pendiente de incluir
			}
			case get_URI_Fragment:
			{
				//pendiente de incluir
			}
			case Parsed:												  // en caso de Parsing_Done
			{
				std::cout << "Parsed" << std::endl;
				std::cout << "Method = " << _method << std::endl;
				return ;														// salimos de la función
			}
		}
		_temp += charRead;	 
	}
}




// // //			 case get_URI_Path:										 // si _fillStatus es Request_Line_URI_Path se rellena _path
// // //			 {
// // //				 // if (charRead == ' ')										   // si el caracter es un espacio		
// // //				 // {
// // //					 _fillStatus = get_Version;								  // se pasa al siguiente estado get_Version
// // //				 //	 _path.append(_temp);									 // hacemos un append en _path de lo que tengamos en storage		 
// // //				 //	 _temp.clear();										   // limpiamos storage
// // //				 //	 continue ;												  // saltamos a la siguiente iteración del for
// // //				 // }
// // //				 // else if (charRead == '?')									  // si el caracter es un ? 
// // //				 // {
// // //				 //	 _fillStatus = get_URI_Query;							// se pasa al siguiente estado get_URI_Query
// // //				 //	 _path.append(_temp);									 // hacemos un append en _path de lo que tengamos en storage
// // //				 //	 _temp.clear();										   // limpiamos storage
// // //				 //	 continue ;												  // saltamos a la siguiente iteración del for	
// // // //				 }
// // // //				 else if (charRead == '#')									  // si el caracter es un #
// // // //				 {
// // // //					 _fillStatus = get_URI_Fragment;						 // se pasa al siguiente estado get_URI_Fragment
// // // //					 _path.append(_temp);									 // hacemos un append en _path de lo que tengamos en storage
// // // //					 _temp.clear();										   // limpiamos storage
// // // //					 continue ;												  // saltamos a la siguiente iteración del for
// // // //				 }
// // // //				 else if (!allowedCharURI(charRead))							// si no es un caracter permitido según la RFC
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400 de nuevo con un mensaje distinto
// // // //					 std::cout << "Bad charRead (Request_Line_URI_Path)" << std::endl;
// // // //					 return ;													// salimos de la función		
// // // //				 }
// // // //				 else if ( i > MAX_URI_LENGTH)								   // si el tamaño de la URI es mayor que el máximo permitido en este caso 4096 definido en header
// // // //				 {
// // // //					 _error_code = 414;										  // el error es 414
// // // //					 std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl;  // mensaje de error
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 break ;														 // se sale del switch									  
// // //			 }
// // //			 case get_URI_Query:										// si _fillStatus es get_URI_Query se rellena _query
// // //			 {
// // // //				 if (charRead == ' ')										   // si es un espacio
// // // //				 {
// // // //					 _fillStatus = get_version;								  // se pasa al siguiente estado get_Version
// // // //					 _query.append(_temp);									// se añade el contenido de storage a _query
// // // //					 _temp.clear();										   // se limpia storage
// // // //					 continue ;												  // saltamos a la siguiente iteración del for  
// // // //				 }
// // // //				 else if (charRead == '#')									  // si es un #
// // // //				 {
// // // //					 _fillStatus = get_URI_Fragment;						 // se pasa al siguiente estado get_URI_Fragment
// // // //					 _query.append(_temp);									// se añade el contenido de storage a _query
// // // //					 _temp.clear();										   // se limpia storage
// // // //					 continue ;												  // saltamos a la siguiente iteración del for
// // // //				 }
// // // //				 else if (!allowedCharURI(charRead))
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400 de nuevo con un mensaje distinto						  
// // // //					 std::cout << "Bad charRead (get_URI_Query)" << std::endl;
// // // //					 return ;													// salimos de la función 
// // // //				 }
// // // //				 else if ( i > MAX_URI_LENGTH)
// // // //				 {
// // // //					 _error_code = 414;										  // el error es 414
// // // //					 std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl; // mismo mensaje de error
// // // //					 return ;													// salimos de la función 
// // // //				 }
// // // //				 break ;
// // //			 }
// // //			 case get_URI_Fragment:									 // si es get_URI_Fragment se rellena _fragment
// // //			 {
// // // //				 if (charRead == ' ')										   // si es un espacio
// // // //				 {
// // // //					 _fillStatus = get_Version;								  // se pasa al siguiente estado get_Version
// // // //					 _fragment.append(_temp);								 // se añade el contenido de storage a _fragment
// // // //					 _temp.clear();										   // se limpia storage
// // // //					 continue ;												  // saltamos a la siguiente iteración del for
// // // //				 }
// // // //				 else if (!allowedCharURI(charRead))							// si no es un caracter permitido según la RFC
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400 de nuevo con un mensaje distinto
// // // //					 std::cout << "Bad charRead (get_URI_Fragment)" << std::endl;
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 else if ( i > MAX_URI_LENGTH)								   // si el tamaño de la URI es mayor que el máximo permitido en este caso 4096 definido en header
// // // //				 {
// // // //					 _error_code = 414;										  // el error es 414
// // // //					 std::cout << "URI Too Long (Request_Line_URI_Path)" << std::endl; // mensaje de error
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 break ;														 // se sale del switch
// // //			 }
// // //			 case get_Version:											  // si _fillStatus es get_Version se rellena la versión
// // //			 {
// // // //				 if (checkUriPos(_path))										 // checkUriPos comprueba si la URI es correcta
// // // //				 {
// // // //					 _error_code = 400;										  // si no es correcta el error es 400					
// // // //					 std::cout << "Request URI ERROR: goes before root !!" << std::endl;	 // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 if (charRead != 'H')										   //  si el caracter no es H
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (get_Version)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _fillStatus = Request_Line_HT;									   // si el caracter es H se pasa al siguiente estado Request_Line_HT
// // // //				 break ;														 // se sale del switch			   
// // //			 }
// // //			 // case Request_Line_HT:											   //  si _fillStatus es Request_Line_HT se rellena la versión
// // //			 // {
// // // //				 if (charRead != 'T')										   // si el caracter no es T
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Request_Line_HT)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _fillStatus = Request_Line_HTT;									  // si el caracter es T se pasa al siguiente estado Request_Line_HTT
// // // //				 break ;														 // se sale del switch
// // // //			 }
// // // //			 case Request_Line_HTT:											  // si _fillStatus es Request_Line_HTT se rellena la versión
// // // //			 {
// // // //				 if (charRead != 'T')										   // si el caracter no es T
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Request_Line_HTT)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _fillStatus = Request_Line_HTTP;									 // si el caracter es T se pasa al siguiente estado Request_Line_HTTP
// // // //				 break ;														 // se sale del switch
// // // //			 }
// // // //			 case Request_Line_HTTP:											 // si _fillStatus es Request_Line_HTTP se rellena la versión
// // // //			 {
// // // //				 if (charRead != 'P')										   // si el caracter no es P
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Request_Line_HTTP)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _fillStatus = Request_Line_HTTP_Slash;							   // si el caracter es P se pasa al siguiente estado Request_Line_HTTP_Slash
// // // //				 break ;														 // se sale del switch
// // // //			 }	
// // // //			 case Request_Line_HTTP_Slash:									   // si _fillStatus es Request_Line_HTTP_Slash se rellena la versión
// // // //			 {
// // // //				 if (charRead != '/')										   // si el caracter no es /
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Request_Line_HTTP_Slash)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _fillStatus = Request_Line_Major;									// si el caracter es / se pasa al siguiente estado Request_Line_Major
// // // //				 break ;														 // se sale del switch
// // // //			 }
// // //			 case get_first_version:											//	
// // //			 {
// // // //				 if (!isdigit(charRead))										// si el caracter no es un dígito ver si merece la pena orzarlo a que sea un 1
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Request_Line_Major)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _ver_major = charRead;										 // se asigna el caracter a _ver_major
// // // //				 _fillStatus = Request_Line_Dot;									  // cambiamos el estado a Request_Line_Dot
// // // //				 break ;														 // salimos del switch	
// // //			 }
// // //			 case get_Dot:														   // buscamos el punto
// // //			 {
// // // //				 if (charRead != '.')										   // si el caracter no es un punto
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Request_Line_Dot)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _fillStatus = Request_Line_Minor;									// si el caracter es un punto cambiamos el estado a Request_Line_Minor
// // // //				 break ;														 // salimos del switch											
// // //			 }
// // //			 case get_second_version:											// buscamos el punto
// // //			 {
// // // //				 if (!isdigit(charRead))										// si el caracter no es un dígito ver si merece la pena orzarlo a que sea un 1
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Request_Line_Minor)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _ver_minor = charRead;										 // se asigna el caracter a _ver_minor
// // // //				 _fillStatus = Request_Line_CR;									   // cambiamos el estado a Request_Line_CR
// // // //				 break ;														 // salimos del switch
// // //			 }
// // //			 case get_CR:											   // buscamos el retorno de carro
// // //			 {
// // // //				 if (charRead != '\r')										  // si el caracter no es un retorno de carro
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Request_Line_CR)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _fillStatus = Request_Line_LF;									   // si el caracter es un retorno de carro cambiamos el estado a Request_Line_LF
// // // //				 break ;														 // salimos del switch
// // //			 }
// // //			 case get_LF:											   // buscamos el salto de línea
// // //			 {
// // // //				 if (charRead != '\n')										  // si el caracter no es un salto de línea
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Request_Line_LF)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _fillStatus = Field_Name_Start;									  // si el caracter es un salto de línea cambiamos el estado a Field_Name_Start
// // // //				 _temp.clear();											   // limpiamos storage
// // // //				 continue ;													  // saltamos a la siguiente iteración del for
// // //			 }
// // //			 case header_Name_Start:											  // buscamos el nombre del campo
// // //			 {
// // // //				 if (charRead == '\r')										  // si el caracter es un retorno de carro
// // // //					 _fillStatus = headers_End;										// cambiamos el estado a Fields_End ya que se han encontrado juntos \r\n\r	
// // // //				 else if (isToken(charRead))									// si el caracter es un token válido 
// // // //					 _fillStatus = header_Name;										// cambiamos el estado a Field_Name
// // // //				 else															// si no es un token
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Field_Name_Start)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 break ;														 // salimos del switch
// // //			 }
// // //			 case headers_End:													// sive para comprobar si se han acabado los campos	
// // //			 {
// // // //				 if (charRead == '\n')										  // si el caracter es un salto de línea
// // // //				 {
// // // //					 _temp.clear();										   // limpiamos storage
// // // //					 _fields_done_flag = true;								   // el flag de campos se pone a true ya que se han encontrado juntos \r\n
// // // //					 _handle_headers();										  // se llama a la función _handle_headers
// // // //					 //if no body then parsing is completed.
// // // //					 if (_body_flag == 1)										// si el flag de body es 1 
// // // //					 {
// // // //						 if (_chunked_flag == true)							  // si el flag de chunked es true				
// // // //							 _fillStatus = Chunked_Length_Begin;					  // cambiamos el estado a Chunked_Length_Begin
// // // //						 else													// si no			
// // // //						 {   
// // // //							 _fillStatus = Message_Body;							  // cambiamos el estado a Message_Body
// // // //						 }
// // // //					 }				   
// // //					 // else														// si no
// // //					 // {
// // //					 //	 _fillStatus = Parsed;									   // cambiamos el estado a Parsing_Done
// // //					 // }
// // //					 // continue ;												  // saltamos a la siguiente iteración del for
// // //					 // }
// // // //				 else															// si el caracter no es un salto de línea   
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Fields_End)" << std::endl;	 // mensaje de error distinto	
// // // //					 return ;													// salimos de la función		
// // // //				 }																   
// // // //				 break ;														 // salimos del switch							
// // //			 }
// // //			 case header_Name:													// buscamos el nombre del campo para los headers		
// // //			 {
// // // //				 if (charRead == ':')										   // si el caracter es :
// // // //				 {
// // // //					 _key_temp = _temp;									// almacenamos el nombre del campo en _key_temp
// // // //					 _temp.clear();										   // limpiamos storage
// // // //					 _fillStatus = header_Value;									   // cambiamos el estado a Field_Value
// // // //					 continue ;												  // saltamos a la siguiente iteración del for	
// // // //				 }
// // // //				 else if (!isToken(charRead))								   // si el caracter no es un token
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400		
// // // //					 std::cout << "Bad charRead (Field_Name)" << std::endl;	 // mensaje de error distinto	  
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 break ;														 // salimos del switch
// // // //				 //if (!charRead allowed)
// // // //				 // error;
// // //			 }
// // //			 case header_Value:												   // en este estado guardamos el contenido de los valroes de los headers
// // //			 {
// // // //				 if (charRead == '\r')										  // si el caracter es un retorno de carro
// // // //				 {
// // // //					 setHeader(_key_temp, _temp);						  // se llama a la función setHeader con los valores de _key_temp y _temp
// // // //					 _key_temp.clear();									   // se limpia _key_temp
// // // //					 _temp.clear();										   // se limpia _temp
// // // //					 _fillStatus = header_Value_End;								   // cambiamos el estado a Field_Value_End
// // // //					 continue ;												  // saltamos a la siguiente iteración del for
// // // //				 }
// // // //				 break ;														 // salimos del switch
// // //			 }
// // //			 case header_Value_End:											   // en este estado se comprueba si se han acabado los valores de los headers
// // //			 {
// // // //				 if (charRead == '\n')										  // si el caracter es un salto de línea
// // // //				 {
// // // //					 _fillStatus = header_Name_Start;								  // cambiamos el estado a Field_Name_Start
// // // //					 continue ;												  // saltamos a la siguiente iteración del for
// // // //				 }
// // // //				 else															// si el caracter no es un salto de línea
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (header_Value_End)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 break ;														 // salimos del switch
// // //			}
// // //			case Chunk_Length_Begin:										  // en este estado se busca el inicio de la longitud de los chunks
// // //			 {
// // // //				 if (isxdigit(charRead) == 0)								   // si el caracter no es un dígito hexadecimal
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Chunk_Length_Begin)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 s.str("");													  // iniciamos s como una cadena vacía 
// // // //				 s.clear();													  // limpiamos s
// // // //				 s << charRead;												 // añadimos el caracter a s
// // // //				 s >> std::hex >> _chunk_length;								 // convertimos el valor de s a hexadecimal y lo almacenamos en _chunk_length
// // // //				 if (_chunk_length == 0)										 // si la longitud del chunk es 0
// // // //					 _fillStatus = Chunk_Length_CR;								 // cambiamos el estado a Chunked_Length_CR
// // // //				 else															// si no
// // // //					 _fillStatus = Chunk_Length;									// cambiamos el estado a Chunked_Length
// // // //				 continue ;													  // saltamos a la siguiente iteración del for
// // //			 } 
// // //			case Chunk_Length:												// en caso de Chunked_lenght	
// // //			 {
// // // //				 if (isxdigit(charRead) != 0)								   // si el caracter es un dígito hexadecimal
// // // //				 {
// // // //					 int temp_len = 0;										   // inicializamos temp_len a 0
// // // //					 s.str("");												  // iniciamos s como una cadena vacía
// // // //					 s.clear();												  // limpiamos s
// // // //					 s << charRead;											 // añadimos el caracter a s
// // // //					 s >> std::hex >> temp_len;								  // convertimos el valor de s a hexadecimal y lo almacenamos en temp_len
// // // //					 _chunk_length *= 16;										// multiplicamos _chunk_length por 16
// // // //					 _chunk_length += temp_len;								  // sumamos temp_len a _chunk_length
// // // //				 }
// // // //				 else if (charRead == '\r')									 // si el caracter es un retorno de carro
// // // //					 _fillStatus = Chunked_Length_LF;								 // cambiamos el estado a Chunked_Length_LF
// // // //				 else															// si no
// // // //					 _fillStatus = Chunked_Ignore;									// cambiamos el estado a Chunked_Ignore
// // // //				 continue ;													  // saltamos a la siguiente iteración del for						
// // //			 }
// // //			case Chunk_Length_CR:											 // en caso de Chunked_Length_CR
// // //			 {
// // // //				 if (charRead == '\r')										  // si el caracter es un retorno de carro
// // // //					 _fillStatus = Chunked_Length_LF;								 // cambiamos el estado a Chunked_Length_LF
// // // //				 else															// si no
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Chunked_Length_CR)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 continue ;													  // saltamos a la siguiente iteración del for
// // //			 }
// // //			case Chunk_Length_LF:											 // en caso de Chunked_Length_LF
// // //			 {
// // // //				 if (charRead == '\n')										  // si el caracter es un salto de línea
// // // //				 {
// // // //					 if (_chunk_length == 0)									 // si la longitud del chunk es 0
// // // //						 _fillStatus = Chunked_End_CR;								// cambiamos el estado a Chunked_End_CR
// // // //					 else														// si no
// // // //						 _fillStatus = Chunked_dt;								   // cambiamos el estado a Chunked_dt
// // // //				 }
// // // //				 else															// si el caracter no es un salto de línea
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Chunk_Length_LF)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 continue ;													  // saltamos a la siguiente iteración del for
// // //			 }
// // //			case Chunk_Ignore:											  // en caso de Chunked_Ignore				   
// // //			 {
// // // //				 if (charRead == '\r')										  // si el caracter es un retorno de carro
// // // //					 _fillStatus = Chunk_Length_LF;								 // cambiamos el estado a Chunked_Length_LF
// // // //				 continue ;													  // saltamos a la siguiente iteración del for
// // //			 }
// // //			case Chunk_body:												  // en caso de Chunk_body
// // //			 {
// // // //				 _body.push_back(charRead);									 // añadimos el caracter a _body
// // // //				 --_chunk_length;												// decrementamos _chunk_length
// // // //				 if (_chunk_length == 0)										 // si _chunk_length es 0
// // // //					 _fillStatus = Chunk_body_CR;								   // cambiamos el estado a Chunk_body_CR esperamos un retorno de carro
// // // //				 continue ;													  // saltamos a la siguiente iteración del for
// // //			 }
// // //			case Chunk_body_CR:											   // en caso de Chunk_body_CR
// // //			 {
// // // //				 if ( charRead == '\r')										 // si el caracter es un retorno de carro
// // // //					 _fillStatus = Chunk_body_LF;								   // cambiamos el estado a Chunk_body_LF, esperamos un salto de línea
// // // //				 else															// si no
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Chunk_body_CR)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 continue ;													  // saltamos a la siguiente iteración del for
// // //			 }
// // //			case Chunk_body_LF:											   //  en caso de Chunk_body_LF
// // //			 {
// // // //				 if ( charRead == '\n')										 // si el caracter es un salto de línea
// // // //					 _fillStatus = Chunked_Length_Begin;							  // cambiamos el estado a Chunked_Length_Begin   esperamos un nuevo chunk
// // // //				 else															// si no
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400
// // // //					 std::cout << "Bad charRead (Chunk_body_LF)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 continue ;													  // saltamos a la siguiente iteración del for
// // //			 }
// // //			case Chunk_End_CR:												// en caso de Chunked_End_CR
// // //			 {
// // // //				 if (charRead != '\r')										  // si el caracter no es un retorno de carro
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400	  
// // // //					 std::cout << "Bad charRead (Chunk_End_CR)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _fillStatus = Chunk_End_LF;										// si el caracter es un retorno de carro cambiamos el estado a Chunked_End_LF		
// // // //				 continue ;													  // saltamos a la siguiente iteración del for						  

// // //			 }
// // //			 case Chunk_End_LF:												// en caso de Chunked_End_LF
// // //			 {
// // // //				 if (charRead != '\n')										  // si el caracter no es un salto de línea	 
// // // //				 {
// // // //					 _error_code = 400;										  // el error es 400				
// // // //					 std::cout << "Bad charRead (Chunked_End_LF)" << std::endl; // mensaje de error distinto
// // // //					 return ;													// salimos de la función
// // // //				 }
// // // //				 _body_done_flag = true;										 // el flag de body_done se pone a true
// // // //				 _fillStatus = Parsing_Done;										  // cambiamos el estado a Parsing_Done 
// // // //				 continue ;													  // saltamos a la siguiente iteración del for		
// // //			 }
// // //			 case Message_Body:												  // en caso de Message_Body
// // //			 {
// // // //				 if (_body.size() < _body_length )							   // si el tamaño de _body es menor que _body_length
// // // //					 _body.push_back(charRead);								 // añadimos el caracter a _body
// // // //				 if (_body.size() == _body_length )							  // si el tamaño de _body es igual a _body_length
// // // //				 {
// // // //					 _body_done_flag = true;									 // el flag de body_done se pone a true
// // // //					 _fillStatus = Parsing_Done;									  // cambiamos el estado a Parsing_Done
// // // //				 }
// // // //				 break ;														 // salimos del switch
// // //			 }		
// // //			 case Parsed:												  // en caso de Parsing_Done
// // //			 {
// // //				 return ;														// salimos de la función
// // //			 }
// //			 }//end of swtich
// //		 // _temp += charRead;									  // se añade el caracter al string storage
// //	 }
// // //	 if( _fillStatus == parsed)									 // si el estado es Parsing_Done
// // //	 {
// // //		 _body_str.append((char*)_body.dt(), _body.size());		// 
// // //	 }
// }


void	Request::reset(){
	// _path.clear();
	_error_code = 0;
	// _query.clear();
	// _fragment.clear();
	_method = NONE;
	_method_ix = 1;
	//_http_ix = 0;
	_fillStatus = get_First;

	// _body_length = 0;
	// _chunk_length = 0x0;
	// _temp.clear();
	// _body_str = "";
	// _key_storage.clear();m
	// _request_headers.clear();
	// _server_name.clear();
	// _body.clear();
	// _boundary.clear();
	// _fields_done_flag = false;
	// _body_flag = false;
	// _body_done_flag = false;
	// _complete_flag = false;
	// _chunked_flag = false;
	// _multiform_flag = false;
}
