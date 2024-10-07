#pragma once
#include "./common.hpp"
#include "./Client.hpp"
#define URI_MAX_LENGTH 8192

class Client;

class Request
{
	public:
		Request(Client *client, Server_config *config);
		Methods												&getMethod();
		std::string											getMethodStr();
		std::string											&getPath();
		std::string											&getQuery();
		std::string											getHeader(std::string const &);
		const std::map<std::string, std::string>			&getHeaders() const;
		std::string											getServerName();
		int													getPort();
		std::string											&getFragment();
		std::string											&getBody();
		std::string											&getBoundary();
		bool												getMultiformFlag();
		int 												getErrorCode();
		void												saveHeader(std::string &, std::string &);
		void												fillRequest(char *data, size_t size);
		bool												isParsed();
		void												reset();
		void 												setClient(Client* client);
	 	void 												setBodyStr(std::string body);		

	private: 

		Client 												*_client;
		Server_config 										*_config;
		Methods												_method;
		std::map<u_int8_t, std::string>						_methods_str;
		std::string											_path;
		std::string											_query;
		std::string											_fragment;
		std::map<std::string, std::string> 					_headers;
		std::vector<u_int8_t>               				_body;
		std::string											_boundary;
		fillStatusEnum										_fillStatus;
		size_t												_body_size;
		size_t							  					_chunk_size;
		size_t							  					_uri_size;
		int													_error_code;
		std::string											_server_name;
		std::string											_port;
		std::string											_body_str;
		std::string											_temp;
		std::string											_header_name_temp;
		int													 _ix;
		bool												_headers_parsed;
		bool												_get_body_flag;
		bool												_body_parsed;
		bool												_chunked_body_flag;
		bool												_multiform_flag;
		void												_initMethodStr(); 
		void												_returnErr(int err, std::string msg,uint8_t charRead);
		void												_handle_headers();
		bool 												isValidUri();
};
