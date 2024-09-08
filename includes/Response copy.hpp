#pragma once

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "Webserv.hpp"
# include "Request.hpp"


/*
    Crear y alamacenar la respuesta. almacena el valor para ser enviado al cliente.
	con la funcion getRes() se obtiene la respuesta.
*/
{
    public:
        static     Mime _mime;											// objeto mime para obtener el tipo de contenido
        Response();														// constructor		
        Response(Request&);												// constructor con request	
        ~Response();													// destructor								

        std::string     getRes();										// devuelve la respuesta
        size_t      getLen() const;										// devuelve la longitud de la respuesta	
        int         getCode() const;									// devuelve el código de respuesta

        void        setRequest(Request &);                              // asigna el request al objeto response    
        void        setServer(ServerConfig &);                          // asigna el server al objeto response

        void        buildResponse();									// construye la respuesta	
        void        clear();											// limpia la respuesta
        void        handleCgi(Request&);								// maneja el cgi
        void        cutRes(size_t);										// corta la respuesta
        int         getCgiState();										// devuelve el estado del cgi
        void        setCgiState(int);									// asigna el estado del cgi
        void        setErrorResponse(short code);						// asigna la respuesta de error

		CgiHandler		_cgi_obj;										// objeto cgi	

        std::string removeBoundary(std::string &body, std::string &boundary);	// elimina el boundary del cuerpo de la solicitud
        std::string     _response_content;								// contenido de la respuesta

        Request     request;                                            // copia del request al que tiene que responder 
    private:
        ServerConfig    _server;                                        // server al que se tiene que responder
        std::string     _target_file;                                   // archivo al que se tiene que responder	  
        std::vector<uint8_t> _body;										// cuerpo de la respuesta
        size_t          _body_length;									// longitud del cuerpo de la respuesta
        std::string     _response_body;
        std::string     _location;
        short           _code;
        char            *_res;
		int				_cgi;
		int				_cgi_fd[2];
		size_t			_cgi_response_length;
        bool            _auto_index;

        int     buildBody();
        size_t  file_size();
        void    setStatusLine();
        void    setHeaders();
        void    setServerDefaultErrorPages();
        int     readFile();
        void    contentType();
        void    contentLength();
        void    connection();
        void    server();
        void    location();
        void    date();
        int     handleTarget();
        void    buildErrorBody();
        bool    reqError();
        int     handleCgi(std::string &);
        int     handleCgiTemp(std::string &);
};

#endif // RESPONSE_HPP