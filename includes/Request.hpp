#pragma once

#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "./common.hpp"

class Client; // Declaración adelantada de Client válida ya que solo llama al  puntero
// Esto se debería colocar en la cabecera de un archivo que se pueda sobreescrbir por el parseo
#define URI_MAX_LENGTH 8192		// nginx default 8k puede cambiarse con las directivas: http { client_header_buffer_size 16k; large_client_header_buffers 4 16k;}

enum Methods   // incluir tanto aquí como en _initMethodStr los métodos permitidos
{
	GET,
	POST,
	DELETE,
	NONE
};

/**
 * @brief definir estado del parseo.
 * 
 * Pendiente de revisar para ver si es posible dividir en más estado y 
 * pendiente de corregir la función de error para ver si es necesario incluir los mensajes
 * o si lo dejamos como un debugger.
 * 
 *    ----- Pendiente de revisar la forma de lectura de las partes que se hacen comparando con cadenas
 *    ----- Sobre todo el protocolo ya que he dejado por defecto el 1.1 como único protocolo aceptado
 * 
 */
enum fillStatusEnum				
{
	get_First,
	get_Method,
	get_First_Space,
	get_First_Slash,
	get_URI_Path,
	get_URI_Query,
	get_URI_Fragment,
	get_Protocol,
	get_CR,
	get_LF,
	header_Name_Start,
	headers_End,
	header_Name,
	header_Value,
	header_Value_End,
	Chunk_Length_Begin,
	Chunk_Length,
	Chunk_Ignore,
	Chunk_Length_CR,
	Chunk_Length_LF,
	Chunk_body,
	Chunk_body_CR,
	Chunk_body_LF,
	Chunk_End_CR,
	Chunk_End_LF,
	get_Body,
	Parsed
};

class Request
{
	public:
		Request();
		~Request();
		Methods										&getMethod();									// se usa para obtener el método de la solicitud, valor de la enumeración 
		std::string									getMethodStr();									// se usa para obtener el método en formato string
		std::string									&getPath();										// se usa para obtener la ruta del recurso solicitado
		std::string									&getQuery();									// se usa para obtener la query de la solicitud, habrá que parsear los parámetros en el response o en el CGI si corresponde
		std::string									getHeader(std::string const &);					// se usa para obtener un header en concreto
		const std::map<std::string, std::string>	&getHeaders() const;							// se usa para obtener todos los headers de la solicitud
		std::string									getServerName();								// se usa para obtener el nombre del servidor
		std::string									&getFragment();									// se usa para obtener el fragmento de la solicitud(suelen corresponder con parted del mismo documento pero depende del mime/type del documento)					
		std::string									&getBody();										// se usa para obtener el cuerpo de la solicitud en formato string	
		std::string									&getBoundary();									// se usa para obtener el boundary en caso de que se esté recibiendo un formulario multiform
		bool										getMultiformFlag();								// ver documentación sobre formularios html al final de este documento
		int 										getErrorCode();									// se usa para obtener el código de error, mi intención es usarlos para mostrar la página de error adecuada		
		void										saveHeader(std::string &, std::string &);       // se usa para almacenar los datos limpios sin espacios al final y al principio y con los nombres en minúsculas
		void										setClientMaxBodySize(size_t);					// se usa para establecer el tamaño máximo del cuerpo de la solicitud No tengo claro si tambienlimita el tamaño de los chunks o del total de los chunked
		void										fillRequest(char *data, size_t size);			// se usa para alimentar el parser con los datos recibidos
		bool										isParsed();										// para comprobar desde fuera si esta parseado y se puede enviar la respuesta
		bool										isKeepAlive();									// ver si es necesario para el funcionamiento del servidor
		void										reset();										// llamado de momento desde el response
		void								 		printParsed();
		void 										setClient(Client* client);
		Client*										_client;							// cliente
		
		// ver si hacen falta los settters de los métodos, body, etc

	private: 
	
		Methods								_method;				// se usa para almacenar el método a comparar con el metodo recibido basado en la primera letra
		std::map<u_int8_t, std::string>		_methods_str;			// se usa para comparar el metodo recibido alacendo strings  de los métodos recibidos 
		std::string							_path;				    // se usa para almacenar la ruta del recurso solicitado
		std::string							_query;					// se usa para almacenar la query de la solicitud /ver como gestionar los params
		std::string							_fragment;
		std::map<std::string, std::string> 	_headers;
		std::vector<u_int8_t>               _body;
		std::string							_boundary;
		fillStatusEnum						_fillStatus;			// se usa para almacenar el estado actual del parser y lo que buscamos o esperamos en cada momento
		size_t								_body_size;		   		// se usa para almacenar la longitud del cuerpo de la solicitud aportado por el heade content-length
		size_t								_client_max_body_size;
		size_t							  	_chunk_size;
		size_t							  	_uri_size;
		int									_error_code;
		std::string							_server_name;
		std::string							_body_str;
		/*				temporales				*/
		std::string							_temp;			   // se usa para almacenar los datos recibidos limpios
		std::string							_header_name_temp;		   // se usa para almacenar el nombre del header antes de pasarlo a setHeader
		/*				indices para el parseo				*/
		int									 _ix;		  		// se usa para avanzar sobre el nombre del metodo, el tipo HTTP y la version con el string recibido
		/* 				flags				*/
		bool								_headers_parsed;	  // se usa para indicar que se han recibido todos los campos de la solicitud
		bool								_get_body_flag;			 // se usa para indicar que se está recibiendo el cuerpo de la solicitud ya sea con content-length o chunked
		bool								_body_parsed;		// se usa para indicar que se ha recibido todo el cuerpo de la solicitud
		bool								_chunked_body_flag;		  // se usa para indicar que se está recibiendo datos en formato chunked
		bool								_multiform_flag;
		void		_initMethodStr(); 
		void		_returnErr(int err, std::string msg,uint8_t charRead);
		void		_handle_headers();

};

#endif

/*

Content-Type in HTML forms

En una solicitud POST , que resulta del envio de un formulario html, el Content-Type de la solicitud es especificado como un atributo enctype del elemento <form> .
html

<form action="/" method="post" enctype="multipart/form-data">
  <input type="text" name="description" value="some text" />
  <input type="file" name="myFile" />
  <button type="submit">Submit</button>
</form>

La solicitud se visualiza algo como esto (si tienes poco interes en los headers omite esto)

POST /foo HTTP/1.1
Content-Length: 68137
Content-Type: multipart/form-data; boundary=---------------------------974767299852498929531610575

---------------------------974767299852498929531610575
Content-Disposition: form-data; name="description"

some text
---------------------------974767299852498929531610575
Content-Disposition: form-data; name="myFile"; filename="foo.txt"
Content-Type: text/plain

(content of the uploaded file foo.txt)
---------------------------974767299852498929531610575


*/
