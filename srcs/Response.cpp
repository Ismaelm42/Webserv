#include "../includes/lib.hpp"
#include "../includes/common.hpp"

void Response::buildErrorMap()
{
    httpStatusMap.clear();
	httpStatusMap.insert(std::make_pair(100, std::make_pair("Continue", "The client should continue with its request.")));
	httpStatusMap.insert(std::make_pair(101, std::make_pair("Switching Protocols", "The server has agreed to switch to a different protocol.")));
	httpStatusMap.insert(std::make_pair(200, std::make_pair("OK", "The request has been successfully processed.")));
	httpStatusMap.insert(std::make_pair(201, std::make_pair("Created", "The request has been fulfilled, and a new resource has been created.")));
	httpStatusMap.insert(std::make_pair(202, std::make_pair("Accepted", "The request has been accepted for processing, but it has not been completed.")));
	httpStatusMap.insert(std::make_pair(203, std::make_pair("Non-Authoritative Information", "The returned information is not definitive.")));
	httpStatusMap.insert(std::make_pair(204, std::make_pair("No Content", "The request was successful, but there is no content to return.")));
	httpStatusMap.insert(std::make_pair(205, std::make_pair("Reset Content", "The request was successful, please reset the form.")));
	httpStatusMap.insert(std::make_pair(206, std::make_pair("Partial Content", "The server is returning only part of the requested data.")));
	httpStatusMap.insert(std::make_pair(300, std::make_pair("Multiple Choices", "There are multiple options for the request.")));
	httpStatusMap.insert(std::make_pair(301, std::make_pair("Moved Permanently", "The requested resource has been permanently moved.")));
	httpStatusMap.insert(std::make_pair(302, std::make_pair("Found", "The requested resource has been temporarily found at another location.")));
	httpStatusMap.insert(std::make_pair(303, std::make_pair("See Other", "The client should request the resource at another URL.")));
	httpStatusMap.insert(std::make_pair(304, std::make_pair("Not Modified", "The resource has not been modified since the last request.")));
	httpStatusMap.insert(std::make_pair(307, std::make_pair("Temporary Redirect", "The resource is temporarily available at another location.")));
	httpStatusMap.insert(std::make_pair(308, std::make_pair("Permanent Redirect", "The resource is permanently available at another location.")));
	httpStatusMap.insert(std::make_pair(400, std::make_pair("Bad Request", "The server cannot process the request due to client error.")));
	httpStatusMap.insert(std::make_pair(401, std::make_pair("Unauthorized", "Authentication is required to access the resource.")));
	httpStatusMap.insert(std::make_pair(403, std::make_pair("Forbidden", "You do not have permission to access this resource.")));
	httpStatusMap.insert(std::make_pair(404, std::make_pair("Not Found", "The requested resource was not found on the server.")));
	httpStatusMap.insert(std::make_pair(405, std::make_pair("Method Not Allowed", "The request method is not allowed for the requested resource.")));
	httpStatusMap.insert(std::make_pair(406, std::make_pair("Not Acceptable", "The server cannot generate a response acceptable to the client's 'Accept' headers.")));
	httpStatusMap.insert(std::make_pair(407, std::make_pair("Proxy Authentication Required", "Authentication is required to use a proxy.")));
	httpStatusMap.insert(std::make_pair(408, std::make_pair("Request Timeout", "The server timed out waiting for the request.")));
	httpStatusMap.insert(std::make_pair(409, std::make_pair("Conflict", "The request cannot be processed due to a conflict with the current state of the resource.")));
	httpStatusMap.insert(std::make_pair(410, std::make_pair("Gone", "The requested resource is no longer available, and no forwarding address is known.")));
	httpStatusMap.insert(std::make_pair(411, std::make_pair("Length Required", "The content length is required for the request.")));
	httpStatusMap.insert(std::make_pair(412, std::make_pair("Precondition Failed", "A precondition in the request headers has failed.")));
	httpStatusMap.insert(std::make_pair(413, std::make_pair("Payload Too Large", "The request payload is too large for the server to process.")));
	httpStatusMap.insert(std::make_pair(414, std::make_pair("URI Too Long", "The requested URI is too long for the server to process.")));
	httpStatusMap.insert(std::make_pair(415, std::make_pair("Unsupported Media Type", "The request's media type is not supported by the server.")));
	httpStatusMap.insert(std::make_pair(416, std::make_pair("Range Not Satisfiable", "The range specified is not valid for the requested resource.")));
	httpStatusMap.insert(std::make_pair(417, std::make_pair("Expectation Failed", "The server cannot meet the expectations given in the 'Expect' header.")));
	httpStatusMap.insert(std::make_pair(418, std::make_pair("I'm a teapot", "I'm a teapot. This code is a joke from the HTCPCP protocol.")));
	httpStatusMap.insert(std::make_pair(421, std::make_pair("Misdirected Request", "The request was directed to a server that cannot produce a response.")));
	httpStatusMap.insert(std::make_pair(425, std::make_pair("Too Early", "The server is unwilling to process a request that might be replayed.")));
	httpStatusMap.insert(std::make_pair(426, std::make_pair("Upgrade Required", "The client should switch to a different protocol.")));
	httpStatusMap.insert(std::make_pair(428, std::make_pair("Precondition Required", "The server requires the request to be conditional.")));
	httpStatusMap.insert(std::make_pair(429, std::make_pair("Too Many Requests", "The client has sent too many requests in a given amount of time.")));
	httpStatusMap.insert(std::make_pair(431, std::make_pair("Request Header Fields Too Large", "The request headers are too large.")));
	httpStatusMap.insert(std::make_pair(451, std::make_pair("Unavailable For Legal Reasons", "The resource is unavailable due to legal reasons.")));
	httpStatusMap.insert(std::make_pair(500, std::make_pair("Internal Server Error", "The server encountered an internal error and was unable to complete the request.")));
	httpStatusMap.insert(std::make_pair(501, std::make_pair("Not Implemented", "The server does not support the functionality required to fulfill the request.")));
	httpStatusMap.insert(std::make_pair(502, std::make_pair("Bad Gateway", "The server, while acting as a gateway, received an invalid response.")));
	httpStatusMap.insert(std::make_pair(503, std::make_pair("Service Unavailable", "The server is currently unavailable to handle the request.")));
	httpStatusMap.insert(std::make_pair(504, std::make_pair("Gateway Timeout", "The server, while acting as a gateway, did not receive a timely response.")));
	httpStatusMap.insert(std::make_pair(505, std::make_pair("HTTP Version Not Supported", "The HTTP version used in the request is not supported by the server.")));
	httpStatusMap.insert(std::make_pair(506, std::make_pair("Variant Also Negotiates", "Transparent content negotiation for this resource resulted in a circular reference.")));
	httpStatusMap.insert(std::make_pair(507, std::make_pair("Insufficient Storage", "The server is unable to store the representation needed to complete the request.")));
	httpStatusMap.insert(std::make_pair(510, std::make_pair("Not Extended", "Further extensions are required for the server to fulfill the request.")));
	httpStatusMap.insert(std::make_pair(511, std::make_pair("Network Authentication Required", "Network authentication is required to access the resource.")));
}

Response::Response(Client *client, Server_config *config, Request *request)
:_client(client), _config(config), _request(request), _cgiFlag(false), _auto_index_flag(0)
{
	buildErrorMap();
	_responseString = "";
		_code = _request->getErrorCode();
	_hasIndexFlag = 0;
	_locationMatch = "";
}

void Response::reset()
{ 
	_responseString = ""; 
	_code = _request->getErrorCode();
	_cgiFlag= false;
	_response_str = "";
	_target = "";
	_response_body_str = "";
	_mime =  "";
	_auto_index_flag = 0;
	_hasIndexFlag = 0;
	_locationMatch = "";
}	

int Response::getCode() const
{
	return _code;
}

int Response::setReturnCode(int code)
{
	_code = code;
	return (_code);
}

int Response::setCode(int code)
{
	_code = code;
	return(0);
}

void Response::setClient(Client* client)
{
	_client = client;
}

std::string Response::getStatusDescription(int statusCode)
{
    std::map<int, std::pair<std::string, std::string> >::const_iterator it = httpStatusMap.find(statusCode);
    if (it != httpStatusMap.end())
		return it->second.second;
	else
		return "No description available";
}

std::string Response::statusString(int statusCode)
{
	std::map<int, std::pair<std::string, std::string> >::const_iterator it = httpStatusMap.find(statusCode);
	if (it != httpStatusMap.end())
		return it->second.first;
	else
		return "Undefined";
}

void		Response::setStatusline()
{
	std::ostringstream temp;
	if (_code == 0)
		_code = 200;
	temp << _code;
	std::string codeStr = temp.str();
	_response_str.append("HTTP/1.1 " + codeStr + " ");
	_response_str.append(statusString(_code));
	_response_str.append("\r\n");
}

bool Response::isErrorCode()
{
	if(_request->getErrorCode())
	{
		_code = _request->getErrorCode();
		return (1);
	}
	return (0);
}

std::string concatenatePaths(std::string str1, std::string str2, std::string str3)
{
	std::string res;
	int		 len1;
	int		 len2;

	len1 = str1.length();
	len2 = str2.length();
	if (str1[len1 - 1] == '/' && !str2.empty() && str2[0] == '/')
		str2.erase(0, 1);
	if (str1[len1 - 1] != '/' && !str2.empty() && str2[0] != '/')
		str1.append(1, '/');
	if (str2[len2 - 1] == '/' && !str3.empty() && str3[0] == '/')
		str3.erase(0, 1);
	if (str2[len2 - 1] != '/' && !str3.empty() && str3[0] != '/')
		str2.append(1, '/');
	res = str1 + str2 + str3;
	return (res);
}

bool isReadableDirectory(const std::string& path)
{
	struct stat info;
	if (stat(path.c_str(), &info) != 0)
	{
		std::cout << "path en isReadableDirectory: " << path << std::endl;
		std::cerr << "&info stat() error" << std::endl;
		return false;
	}
	else if (!(info.st_mode & S_IFDIR))
	{
		std::cerr << "S_IFDIR stat() error" << std::endl;
		return false;
	}
	if (access(path.c_str(), R_OK) != 0){
			std::cerr << "Access R_OK stat() error" << std::endl;
		return false;
	}
	return true;
}

int checkDirectoriesPath(const std::string& filepath)
{
    std::string trimmedFilepath = filepath;
	if (trimmedFilepath.substr(0, 2) == "./")
		trimmedFilepath.erase(0, 2);
    std::vector<std::string> parts;
    std::stringstream ss(trimmedFilepath);
    std::string item;
    while (std::getline(ss, item, '/'))
		if (!item.empty())
			parts.push_back(item);
    std::string currentPath = "";
    for (size_t i = 0; i < parts.size() - 1; ++i)
	{
        currentPath += parts[i] + "/";
        if (!isReadableDirectory(currentPath))
			return 1;
    }
    return 0;
}

int Response::getFile()
{
    struct stat buffer;
	std::cout << "target en getfile: " << _target << std::endl;
		if (!_target.empty() && _target[0] == '/')
		_target.erase(0, 1);
	// if (!_target.empty() && _target[0] == '/')
	// 	_target = '.' + _target;
	// if (!_target.empty() && _target[(_target.size() - 1)] != '/')
	// 	_target += '/';
	std::cout << "target en getfile: " << _target << std::endl;
	if (checkDirectoriesPath(_target.c_str()))
	{
		std::cout << "en checkDirectoriesPath linea 221" << std::endl;
		return (setReturnCode(403));
	}
	if (stat(_target.c_str(), &buffer) != 0)
	{
		std::cout << "en checkDirectoriesPath linea 226" << std::endl;
		return (setReturnCode(404));
	}
	if (!(buffer.st_mode & S_IRUSR))
	{
		std::cout << "en checkDirectoriesPath linea 231" << std::endl;
		return (setReturnCode(403));
	}
	std::ifstream file(_target.c_str());
    if (!file.is_open())
    {
		std::cout << "en checkDirectoriesPath linea 237" << std::endl;
	    return (setReturnCode(404));
	}
	std::ostringstream ss;
    ss << file.rdbuf();
    if (file.rdstate() & std::ifstream::failbit || file.rdstate() & std::ifstream::badbit)
        return (setReturnCode(500));
    _response_body_str = ss.str();
    return (0);
}

void	Response::setHeaders()
{	
	MimeType mime;
	_response_str.append("Content-Type: ");
	_mime = "text/html";
	if(_target.rfind(".", std::string::npos) != std::string::npos && _code == 200)
		_mime = mime.getMimeType(_target.substr(_target.rfind(".", std::string::npos) + 1));
	_response_str.append(_mime);
	_response_str.append("\r\n");
	if (_location.length())
	{
		if(_location[_location.length() - 1] != '/')
			_location.append("/");
		_response_str.append("Location: "+ _location +"\r\n");
	}
	std::stringstream ss;
	ss << _response_body_str.length();
	_response_str.append("Content-Length: " + ss.str() + "\r\n");
	if (_request->getHeader("authorization") != "")
	{
		_response_str.append("WWW-Authenticate: Basic realm=\"");
		_response_str.append(_config->locations.back().auth_basic);
		_response_str.append("\"\r\n");
	}	
	_response_str.append("\r\n");
}

std::string Response::getMatch(std::string path, std::vector<Location_config> locations)
{
	std::string longestMatch = "";
	size_t longestMatchLength = 0;
	std::string combinedPath = concatenatePaths(_config->root, path, "");
	for (std::vector<Location_config>::iterator it = locations.begin(); it != locations.end(); it++)
	{
		std::string locationToFind = it->location;
		if (combinedPath.find(locationToFind) == 0 && locationToFind.length() > longestMatchLength)
		{
			longestMatch = locationToFind;
			longestMatchLength = locationToFind.length();
		}
	}
	if (longestMatchLength > 0)
		_locationMatch = longestMatch;
	std::cout <<Red<< "locationMatch: " << _locationMatch << Reset << std::endl;
	return _locationMatch;
}

static Location_config* find_location(Server_config &server, const std::string &location_value)
{
	for (size_t i = 0; i < server.locations.size(); ++i)
		if (server.locations[i].location == location_value)
			return &server.locations[i];
	return NULL;
}

int Response::isValidTargetFile()
{
	std::ifstream file(_target.c_str());
	return (file.good());
}

int Response::isNotValidMethod()
{
	for (std::set<std::string>::const_iterator it = _location_config->methods.begin(); it != _location_config->methods.end(); ++it)
		if (*it == _request->getMethodStr())
			return (0);
	return (setReturnCode(405));
}

int Response:: setIndex()
{
	std::vector<std::string> index;
	if (_location_config->index.size() > 0)
		index = _location_config->index;
	else if (_config->index.size() > 0)
		index = _config->index;
	if (index.size() > 0 && (_request->getPath() == "/" || _request->getPath() == ""))
	{
		for (size_t i = 0; i < index.size(); i++)
		{
			_target = index[i];
			if (isValidTargetFile())
			{
				_hasIndexFlag = 1;
				return (0);
			}
		}
		_target = concatenatePaths(_config->root, _request->getPath(), "");
		if (!isReadableDirectory(_target) || !_location_config->autoindex)
			return (setReturnCode(404));
	}
	return (0);
}

static bool hasValidExtension(const std::string& path, const std::vector<std::pair<std::string, std::string> >& config)
{
	std::string::size_type dotPos = path.rfind('.');
	if (dotPos == std::string::npos)
		return false;
	std::string extension = path.substr(dotPos);
	for (std::vector<std::pair<std::string, std::string> >::const_iterator it = config.begin(); it != config.end(); ++it)
		if (it->first == extension)
			return true;
	return false;
}

int Response::launchCgi()
{
	_cgiFlag = true;
	_response_body_str = "";
	_client->initCgi(&_code, _response_body_str);
	if (_code != 0 && _code != 200)
		return 1;
	return 0;
}

bool endsWith(const std::string& str, const std::string& suffix)
{
    if (str.size() < suffix.size()) return false;
    return str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string removelocationMatch(std::string path, std::string locationMatch)
{
	std::cout <<Yellow << path << std::endl;
	std::cout << "locationMatch: " << locationMatch << Reset << std::endl;
	std::string::size_type pos = path.find(locationMatch);
	if (pos != std::string::npos)
		path.erase(pos, locationMatch.length());
	return path;
}

int Response::getTarget()
{
	_location_config = NULL;
	_locationMatch = getMatch(_request->getPath(), _config->locations);
	if (!_locationMatch.empty())
	{ 
		_location_config = find_location(*_config,_locationMatch);
		std::cout << Green << "_locationMatch: " << _locationMatch << Reset << std::endl;
		if (_location_config->root.size() > 0)
		{
			std::string finalPath = removelocationMatch(concatenatePaths(_config->root, _request->getPath(), ""), _locationMatch);
			_target= concatenatePaths(_location_config->root, finalPath, "");
		}
		std::cout << "concatenatePath (_target): " << _target << std::endl;
		std::string body = _request->getBody();
		if (_request->getBody().length() > _location_config->body_size)
			return (setReturnCode(413));
		if (!_location_config->methods.empty())
		{	
			if (isNotValidMethod())
				return (501);
		}
		_request->set_basic(_location_config->auth_basic);
		_request->set_basic_path(_location_config->auth_basic_user_file);
		if (_request->getPath().find("login.html") != std::string::npos ||
    		_request->getPath().find("login.py") != std::string::npos ||
    		_request->getPath().find("register.html") != std::string::npos ||
    		_request->getPath().find("register.py") != std::string::npos) 
			{
				std::cout << "en login o register" << std::endl;
				std::cout << "userStatus: " << _request->getUserStatus() << std::endl;
			}
 		else if (_location_config->auth_basic.length() > 0 && _request->getUserStatus() == 0
			&& _location_config->auth_basic != "off")
		{
			if (DEBUG)
				std::cout << "auth_basic: " << _location_config->auth_basic << std::endl;
			return (setReturnCode(401));
		}
		if (setIndex())							
			return (413);				
		else if(_target.size() == 0 )
			_target= concatenatePaths(_config->root, _request->getPath(), "");
		if(_location_config->redir.first)
 		{
			_location = _location_config->redir.second;
			_response_body_str = "";
			_target = "";
			return (setReturnCode(_location_config->redir.first));
		}
		if (!_location_config->cgi.empty() && hasValidExtension(_request->getPath(), _location_config->cgi))
			return(launchCgi());
//		Se podríanpermitir en algunas locations como upload, pero por segurida mejor no permitir que suban scipts y los lancen
//		En caso de escalar el proyecto sería recomendable incluir un mapa de extensiones permitidas por location o restringidas fuera de cgi.
//		if ((endsWith(_target, ".py") || endsWith(_target, ".sh")) && (!endsWith(_locationMatch, "upload"))) {
		if ((endsWith(_target, ".py") || endsWith(_target, ".sh"))) {
			return (setReturnCode(403));
		}
		std::cout << "en getTarget línea 413" << std::endl;
		// VEr si es necesario incluir un apartado para tratar a los directorios
		// incluido para evitar problemas con la primera barra
		if (!_target.empty() && _target[0] == '/')
			_target.erase(0, 1);
		///////////////////////////////	
		if (!_hasIndexFlag && _location_config->autoindex && isReadableDirectory(_target))
		{
			_auto_index_flag = 1;
			std::cout << "en autoindex _auto_index_flag= " << _auto_index_flag << std::endl;

			return (0);
		}
	}
	else
		_target= concatenatePaths(_config->root, _request->getPath(), "");
	return 0;
}

void Response::getDefaultErrorBody(int code)
{
	_response_body_str = err_first_sec_title 
					+ toStr(code) + " " + statusString(code)
					+ err_sec_sec_error + toStr(code)
					+ err_th_sec_errmsg + statusString(code)
					+ err_fth_sec_errdesc + getStatusDescription(code) + err_last_sec_end;
}

int Response::buildErrorPage(int code)
{
	if(_config->error_pages.find(code)==_config->error_pages.end()|| _config->error_pages.at(code).empty())
	{
		getDefaultErrorBody(code);
		return (0);
	}
	for (std::map<int, std::string>::iterator itm = _config->error_pages.begin(); itm != _config->error_pages.end(); itm++)
	{
		if (itm->first == code)
			_target = itm->second;
	}
	if (getFile())
		getDefaultErrorBody(setReturnCode(code));
	return (0);
}

int Response::buildBody()
{
	if (_request->getBody().length() > _config->body_size)
		return (setReturnCode(413));
	if (getTarget())
		return (1);
	if (_cgiFlag || _auto_index_flag || _code)
		return (0);
	if (_request->getMethod() == GET)
	{
		if (getFile())
			return (1);
	}
	else if (_request->getMethod() == DELETE)
	{
		if (!isValidTargetFile())
			return (setReturnCode(404));
		if (remove( _target.c_str() ) != 0 )
			return (setReturnCode(500));
		return (setCode(204));
	}
	_code = 200;
	std::cout << "en BUILDBODY linea 497" << std::endl;
	return (0);
}

std::string removeRoot(std::string target, std::string root)
{
	std::string::size_type pos = target.find(root);
	if (pos != std::string::npos)
		target.erase(pos, root.length());
	return target;
}

int Response::buildDirHtml()
{
    std::string root = _config->root;
	if (_location_config->root.size() > 0)
		root = _location_config->root;
	std::string cleanLocation = removeRoot(_locationMatch, _config->root);
	if(DEBUG)
	{
		std::cout << Green << "En buildDirhtml _target: " << _target << std::endl;
		std::cout << Green << "En buildDirhtml _locationMatch: " << _locationMatch << std::endl;
		std::cout << Green << "En buildDirhtml _location_config->root: " << _location_config->root<< std::endl;
		std::cout << Green << "En buildDirhtml _config->root: " << _config->root << std::endl;
		std::cout << Green << "En buildDirhtml cleanLocation: " << cleanLocation << std::endl;
	}
    struct dirent *structDirent;
    DIR *dir;
    _response_body_str = "";
	std::cout << "target en buildDirHtml línea 509: " << _target << std::endl;

	if (root.size() > 0 && root[0]== '/')
		root.erase(0, 1);
	if(_target[0] == '/')
		_target.erase(0, 1);
    dir = opendir(_target.c_str());
    if (dir == NULL)
    {
        std::cerr << "Error in opendir" << std::endl;
        return (1);
    }
    _response_body_str.append(HTML_ST);
    struct stat file_stat;
    std::string file_path;
    while ((structDirent = readdir(dir)) != NULL)
    {
        if (strcmp(structDirent->d_name, ".") == 0)
            continue;
		std::cout << Red << "_target en buildDirHtml línea 528: " << _target << Reset << std::endl;
		std::cout << Red << "structDirent->d_name en buildDirHtml línea 529: " << structDirent->d_name << Reset << std::endl;
        file_path = concatenatePaths(_target, structDirent->d_name, "");
		std::cout << Red << "file_path en buildDirHtml línea 531: " << file_path << Reset << std::endl;
        stat(file_path.c_str(), &file_stat);
		std::cout << Green << "root en 533: " << root << Reset << std::endl;
        file_path = removeRoot(file_path, root);
		if (_location_config->root.size() > 0)
			file_path = concatenatePaths(cleanLocation, file_path, "");
		std::cout << Yellow << "file_path en buildDirHtml línea 534: " << file_path << Reset << std::endl;
        bool isFile = !S_ISDIR(file_stat.st_mode);
        _response_body_str.append("<tr id=\"");
        _response_body_str.append(structDirent->d_name);
        _response_body_str.append("\" onclick=\"selectFile('" + file_path + "', " + (isFile ? "true" : "false") + ")\">\n");
        _response_body_str.append("<td style=\"padding: 10px;\">\n<a href=\"");
        _response_body_str.append(file_path);
        if (S_ISDIR(file_stat.st_mode))
            _response_body_str.append("/");
        _response_body_str.append("\" style=\"color: yellow; text-decoration: none;\">");
        _response_body_str.append(structDirent->d_name);
        if (S_ISDIR(file_stat.st_mode))
            _response_body_str.append("/");
        _response_body_str.append("</a></td>\n");
        _response_body_str.append("<td style=\"padding: 10px;\">");
        _response_body_str.append(ctime(&file_stat.st_mtime));
        _response_body_str.append("</td>\n");
        _response_body_str.append("<td style=\"padding: 10px;\">");
        if (!S_ISDIR(file_stat.st_mode))
            _response_body_str.append(toStr(file_stat.st_size));
        _response_body_str.append("</td>\n");
        _response_body_str.append("</tr>\n");
    }
    _response_body_str.append(HTML_EN);
	closedir(dir);
    return (0);
}

int countCharsAfterEmptyLines(const std::string& _response_body_str)
{
    std::size_t pos = _response_body_str.find("\n\n");
	if (pos != std::string::npos)
		return _response_body_str.size() - pos - 2;
    return -1;
}

void Response::buildResponse()
{	
	if (isErrorCode() || buildBody())
	{
		std::cout << "en buildResponse línea 576" << std::endl;
		buildErrorPage(_code);
	}
	if	(_cgiFlag)
	{
		if (_code == 0 || _code == 200)
		{
			_code = 200;
			setStatusline();
			std::stringstream ss;
			ss << countCharsAfterEmptyLines(_response_body_str);
			_response_str.append("Content-Length: " + ss.str() + "\r\n");
		}
		else
		{
			setStatusline();
			setHeaders();
		}
		_response_str.append(_response_body_str);
		_responseString = _response_str;
		return ;
	}
	else if (!checkFileOrDirectory(_target, "dir") && _auto_index_flag)
    {
		if (!isReadableDirectory(_target))
		{	
			std::cout << "en buildREsponse línea 600" << std::endl;
			buildErrorPage(403);
		}
		else if (buildDirHtml()){
			std::cout << Yellow << "en buildDirHtml línea 588"  << Reset << std::endl;
			buildErrorPage(500);
		}
        else
            _code = 200;
	}
	else if (!checkFileOrDirectory(_target, "dir") && isReadableDirectory(_target) && !_auto_index_flag)
	{
		std::cout << "en buildResponse línea 611" << std::endl;	
		buildErrorPage(403);
	}
	setStatusline();
	setHeaders();
	if(_request->getMethod() == GET || _code != 200)
		_response_str.append(_response_body_str);
	_responseString = _response_str;
}

std::string Response::getResString()
{
	return _responseString;
}

