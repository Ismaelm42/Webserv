#ifndef REQUEST_HPP
#define REQUEST_HPP

#include "./common.hpp"

// Esto se debería colocar en la cabecera de un archivo que se pueda sobreescrbir por el parseo
#define URI_MAX_LENGTH 8192		// nginx default 8k puede cambiarse con las directivas: http { client_header_buffer_size 16k; large_client_header_buffers 4 16k;}

enum Methods   // incluir tanto aquí como en _initMethodStr los métodos permitidos
{
	GET,
	POST,
	DELETE,
	NONE
};

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
	//	Methods									 &getMethod();
	//	std::string								 &getPath();
	//	std::string								 &getQuery();
	//	std::string								 &getFragment();
	//	std::string								 getHeader(std::string const &);
	//	const std::map<std::string, std::string> &getHeaders() const;
	// 	std::string								 getMethodStr();
	//	std::string								 &getBody();
	//	std::string								 getServerName();
	//	std::string								 &getBoundary();
	//	bool									 getMultiformFlag();
		
	//	 void		setMethod(HttpMethod &);
		void		saveHeader(std::string &, std::string &);
	//	 void		setMaxBodySize(size_t);
	//	 void		setBody(std::string name);
		void		fillRequest(char *data, size_t size);
	//	 bool		parsingCompleted();
	//	 void		printMessage();
		void		reset();
	//	 short	    errorCode();
	//	 bool		keepAlive();
	//	 void		cutReqBody(int bytes);

	private:
		void		_initMethodStr(); 
		void		returnErr(int err, std::string msg,uint8_t charRead);
		void		_handle_headers();
	
		Methods								_method;				// se usa para almacenar el método a comparar con el metodo recibido basado en la primera letra
		std::string							_path;				    // se usa para almacenar la ruta del recurso solicitado
		std::string							_query;					// se usa para almacenar la query de la solicitud /ver como gestionar los params
		std::string							_fragment;
		std::map<std::string, std::string> 	_headers;
		std::vector<u_int8_t>			   	_body;
		std::string							_boundary;
		std::map<u_int8_t, std::string>		_methods_str;			// se usa para comparar el metodo recibido alacendo strings  de los métodos recibidos 
		fillStatusEnum						_fillStatus;			// se usa para almacenar el estado actual del parser y lo que buscamos o esperamos en cada momento
		size_t								_body_size;		   		// se usa para almacenar la longitud del cuerpo de la solicitud aportado por el heade content-length
		//	 size_t							  _max_body_size;

		size_t							  	_chunk_size;
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
	//	 bool								_complete_flag;		 // se usa para indicar que se ha recibido toda la solicitud
		bool								_chunked_body_flag;		  // se usa para indicar que se está recibiendo datos en formato chunked
		bool								_multiform_flag;


};

#endif