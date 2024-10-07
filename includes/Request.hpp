#pragma once
#include "./common.hpp"
#include "./Client.hpp"

#define URI_MAX_LENGTH 8192		// nginx default 8k puede cambiarse con las directivas: http { client_header_buffer_size 16k; large_client_header_buffers 4 16k;}

class Client;

class Request
{
	public:
		Request(Client *client, Server_config *config);
		~Request();
		Methods										&getMethod();									// se usa para obtener el método de la solicitud, valor de la enumeración 
		std::string									getMethodStr();									// se usa para obtener el método en formato string
		std::string									&getPath();										// se usa para obtener la ruta del recurso solicitado
		std::string									&getQuery();									// se usa para obtener la query de la solicitud, habrá que parsear los parámetros en el response o en el CGI si corresponde
		std::string									getHeader(std::string const &);					// se usa para obtener un header en concreto
		const std::map<std::string, std::string>	&getHeaders() const;							// se usa para obtener todos los headers de la solicitud
		std::string									getServerName();								// se usa para obtener el nombre del servidor
		int											getPort();										// se usa para obtener el puerto
		std::string									&getFragment();									// se usa para obtener el fragmento de la solicitud(suelen corresponder con parted del mismo documento pero depende del mime/type del documento)					
		std::string									&getBody();										// se usa para obtener el cuerpo de la solicitud en formato string	
		std::string									&getBoundary();									// se usa para obtener el boundary en caso de que se esté recibiendo un formulario multiform
		bool										getMultiformFlag();								// ver documentación sobre formularios html al final de este documento
		int 										getErrorCode();									// se usa para obtener el código de error, mi intención es usarlos para mostrar la página de error adecuada		
		std::string									getcgiString();									// Retorna el output del cgi
		void										saveHeader(std::string &, std::string &);       // se usa para almacenar los datos limpios sin espacios al final y al principio y con los nombres en minúsculas
		void										fillRequest(char *data, size_t size);			// se usa para alimentar el parser con los datos recibidos
		bool										isParsed();										// para comprobar desde fuera si esta parseado y se puede enviar la respuesta
		bool										isKeepAlive();									// ver si es necesario para el funcionamiento del servidor
		void										reset();										// llamado de momento desde el response
		void								 		printParsed();
		void 										setClient(Client* client);
	 	void 										setBodyStr(std::string body);		

	private: 

		Client 								*_client;				// Puntero al Client de este request
		Server_config 						*_config;				// Puntero a la estructura Config
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
		size_t							  	_chunk_size;
		size_t							  	_uri_size;
		int									_error_code;
		std::string							_server_name;
		std::string							_port;
		std::string							_body_str;
		/*				temporales				*/
		std::string							_temp;			   		// se usa para almacenar los datos recibidos limpios
		std::string							_header_name_temp;		// se usa para almacenar el nombre del header antes de pasarlo a setHeader
		/*				indices para el parseo				*/
		int									 _ix;		  			// se usa para avanzar sobre el nombre del metodo, el tipo HTTP y la version con el string recibido
		/* 				flags				*/
		bool								_headers_parsed;	  	// se usa para indicar que se han recibido todos los campos de la solicitud
		bool								_get_body_flag;			// se usa para indicar que se está recibiendo el cuerpo de la solicitud ya sea con content-length o chunked
		bool								_body_parsed;			// se usa para indicar que se ha recibido todo el cuerpo de la solicitud
		bool								_chunked_body_flag;		// se usa para indicar que se está recibiendo datos en formato chunked
		bool								_multiform_flag;
		void		_initMethodStr(); 
		void		_returnErr(int err, std::string msg,uint8_t charRead);
		void		_handle_headers();
		bool 		isValidUri();
};

