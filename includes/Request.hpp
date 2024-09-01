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
	get_Version,
	// // get_HT,
	// // get_HTT,
	// // get_HTTP,
	// // get_HTTP_Slash,
	// get_first_version,
	// get_Dot,
	// get_second_version,
	// get_CR,
	// get_LF,
	// header_Name_Start,
	// headers_End,
	// header_Name,
	// header_Value,
	// header_Value_End,
	// Chunk_Length_Begin,
	// Chunk_Length,
	// Chunk_Ignore,
	// Chunk_Length_CR,
	// Chunk_Length_LF,
	// Chunk_body,
	// Chunk_body_CR,
	// Chunk_body_LF,
	// Chunk_End_CR,
	// Chunk_End_LF,
	// Message_Body,
	Parsed
};

class Request
{
	public:
		Request();
		~Request();
	//	Methods									  &getMethod();
	//	std::string								 &getPath();
	//	std::string								 &getQuery();
	//	std::string								 &getFragment();
	//	std::string								 getHeader(std::string const &);
	//	const std::map<std::string, std::string>	&getHeaders() const;
	// 	std::string								 getMethodStr();
	//	std::string								 &getBody();
	//	std::string								 getServerName();
	//	std::string								 &getBoundary();
	//	bool										getMultiformFlag();
		
	//	 void		setMethod(HttpMethod &);
	//	 void		setHeader(std::string &, std::string &);
	//	 void		setMaxBodySize(size_t);
	//	 void		setBody(std::string name);
		void		fillRequest(char *data, size_t size);
	//	 bool		parsingCompleted();
	//	 void		printMessage();
		void		reset();
	//	 short	   errorCode();
	//	 bool		keepAlive();
	//	 void		cutReqBody(int bytes);

	private:
		void	_initMethodStr(); 
		void	returnErr(int err, std::string msg,uint8_t charRead);
	
		std::string							_path;				  // se usa para almacenar la ruta del recurso solicitado
	//	 std::string						 _query;
	//	 std::string						 _fragment;
	//	 std::map<std::string, std::string>  _request_headers;
	//	 std::vector<u_int8_t>			   _body;
	//	 std::string						 _boundary;
		Methods								 _method;				// se usa para almacenar el método a comparar con el metodo recibido basado en la primera letra
		std::map<u_int8_t, std::string>		_methods_str;			// se usa para comparar el metodo recibido alacendo strings  de los métodos recibidos 
		fillStatusEnum						 _fillStatus;				 // se usa para almacenar el estado actual del parser y lo que buscamos o esperamos en cada momento
	//	 size_t							  _max_body_size;
	//	 size_t							  _body_length;		   // se usa para almacenar la longitud del cuerpo de la solicitud aportado por el heade content-length
		int									_error_code;
	//	size_t							  _chunk_length;
		std::string							_temp;			   // se usa para almacenar los datos recibidos limpios
	//	std::string						  _key_storage;		   // se usa para almacenar el nombre del header antes de pasarlo a setHeader
		int									 _method_ix;		  // se usa para avanzar sobre el nombre del metodo y comparar con el metodo recibido
	//	int									 _http_ix;
	//	 u_int8_t							_ver_major;			 // se usa para almacenar la versión mayor del protocolo http
	//	 u_int8_t							_ver_minor;			 // se usa para almacenar la versión menor del protocolo http
	//	 std::string						 _server_name;
	//	 std::string						 _body_str;
	//	 /* flags */
	//	 bool								_fields_done_flag;	  // se usa para indicar que se han recibido todos los campos de la solicitud
	//	 bool								_body_flag;			 // se usa para indicar que se está recibiendo el cuerpo de la solicitud ya sea con content-length o chunked
	//	 bool								_body_done_flag;		// se usa para indicar que se ha recibido todo el cuerpo de la solicitud
	//	 bool								_complete_flag;		 // se usa para indicar que se ha recibido toda la solicitud
	//	 bool								_chunked_flag;		  // se usa para indicar que se está recibiendo datos en formato chunked
	//	 bool								_multiform_flag;

	//	 void			_handle_headers();

};

#endif