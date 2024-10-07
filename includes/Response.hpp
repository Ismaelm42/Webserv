#pragma once
#include "./common.hpp"
#include "./MimeType.hpp"

class Client;
class Request;

class Response
{
    public:
		Response(Client *client, Server_config *config, Request *_request);
	  
		std::string 												_response_str;
		Client*														_client;
		Server_config* 												_config;
		Request*													_request;
		bool 														_cgiFlag;
		std::map<int, std::pair<std::string, std::string> >			httpStatusMap;
		bool 														isErrorCode();
		void 														buildResponse();
		int 														buildBody();
		int 														getTarget();
		int 														isValidTargetFile();
		int 														isNotValidMethod();
		int 														getCode() const;
		int 														setCode(int code);
		int 														setReturnCode(int code);
		void 														setClient(Client* client);
		void 														setStatusline();
		int 														getFile();
		std::string 												getResString();
		void    													setHeaders();
		void 														reset();
		std::string 												getStatusDescription(int statusCode);
		std::string 												statusString(int statusCode);

	private:
		int 														_code;
        std::string 												_mime;
		std::string 												_responseString;
		bool 														_hasIndexFlag;
		int 														_auto_index_flag;
		std::string 												_target;
		std::string 												_response_body_str;
		std::string 												_location;
		Location_config 											*_location_config;
		std::string													getMatch(std::string path, std::vector<Location_config> locations);
		int 														setIndex();
		int 														launchCgi();
		int 														buildDirHtml();
		int 														buildErrorPage(int code);
		void														buildErrorMap();
		void 														getDefaultErrorBody(int code);
};
