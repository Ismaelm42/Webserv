#include "../includes/lib.hpp"
#include "../includes/common.hpp"

void Response::buildErrorMap()
{
    httpStatusMap.clear();
    httpStatusMap.insert(std::make_pair(100, std::make_pair("Continue", "El cliente debe continuar con su solicitud.")));
    httpStatusMap.insert(std::make_pair(101, std::make_pair("Switching Protocols", "El servidor ha aceptado cambiar a un protocolo diferente.")));
    httpStatusMap.insert(std::make_pair(200, std::make_pair("OK", "La solicitud ha sido procesada con éxito.")));
    httpStatusMap.insert(std::make_pair(201, std::make_pair("Created", "La solicitud se ha cumplido y se ha creado un nuevo recurso.")));
    httpStatusMap.insert(std::make_pair(202, std::make_pair("Accepted", "La solicitud ha sido aceptada para procesamiento, pero no se ha completado.")));
    httpStatusMap.insert(std::make_pair(203, std::make_pair("Non-Authoritative Information", "La información devuelta no es definitiva.")));
    httpStatusMap.insert(std::make_pair(204, std::make_pair("No Content", "La solicitud fue exitosa, pero no hay contenido que devolver.")));
    httpStatusMap.insert(std::make_pair(205, std::make_pair("Reset Content", "La solicitud fue exitosa, por favor restablezca el formulario.")));
    httpStatusMap.insert(std::make_pair(206, std::make_pair("Partial Content", "El servidor está devolviendo solo una parte de los datos solicitados.")));
    httpStatusMap.insert(std::make_pair(300, std::make_pair("Multiple Choices", "Hay varias opciones para la solicitud.")));
    httpStatusMap.insert(std::make_pair(301, std::make_pair("Moved Permanently", "El recurso solicitado ha sido movido de forma permanente.")));
    httpStatusMap.insert(std::make_pair(302, std::make_pair("Found", "El recurso solicitado ha sido encontrado temporalmente en otra ubicación.")));
    httpStatusMap.insert(std::make_pair(303, std::make_pair("See Other", "El cliente debe hacer una solicitud a otra URL para obtener el recurso.")));
    httpStatusMap.insert(std::make_pair(304, std::make_pair("Not Modified", "El recurso no ha sido modificado desde la última solicitud.")));
    httpStatusMap.insert(std::make_pair(307, std::make_pair("Temporary Redirect", "El recurso está temporalmente disponible en otra ubicación.")));
    httpStatusMap.insert(std::make_pair(308, std::make_pair("Permanent Redirect", "El recurso está permanentemente disponible en otra ubicación.")));
    httpStatusMap.insert(std::make_pair(400, std::make_pair("Bad Request", "El servidor no puede procesar la solicitud debido a un error de cliente.")));
    httpStatusMap.insert(std::make_pair(401, std::make_pair("Unauthorized", "Se requiere autenticación para acceder al recurso.")));
    httpStatusMap.insert(std::make_pair(403, std::make_pair("Forbidden", "No tienes permiso para acceder a este recurso.")));
    httpStatusMap.insert(std::make_pair(404, std::make_pair("Not Found", "El recurso solicitado no fue encontrado en el servidor.")));
    httpStatusMap.insert(std::make_pair(405, std::make_pair("Method Not Allowed", "El método de solicitud no está permitido para el recurso solicitado.")));
    httpStatusMap.insert(std::make_pair(406, std::make_pair("Not Acceptable", "El servidor no puede generar una respuesta aceptable según las cabeceras 'Accept' enviadas por el cliente.")));
    httpStatusMap.insert(std::make_pair(407, std::make_pair("Proxy Authentication Required", "Es necesario autenticarse en un proxy.")));
    httpStatusMap.insert(std::make_pair(408, std::make_pair("Request Timeout", "El servidor agotó el tiempo de espera para la solicitud.")));
    httpStatusMap.insert(std::make_pair(409, std::make_pair("Conflict", "La solicitud no puede procesarse debido a un conflicto con el estado actual del recurso.")));
    httpStatusMap.insert(std::make_pair(410, std::make_pair("Gone", "El recurso solicitado ya no está disponible y no se conoce ninguna dirección de reenvío.")));
    httpStatusMap.insert(std::make_pair(411, std::make_pair("Length Required", "Se requiere la longitud del contenido para la solicitud.")));
    httpStatusMap.insert(std::make_pair(412, std::make_pair("Precondition Failed", "Una precondición en las cabeceras de la solicitud ha fallado.")));
    httpStatusMap.insert(std::make_pair(413, std::make_pair("Payload Too Large", "La carga útil de la solicitud es demasiado grande para ser procesada por el servidor.")));
    httpStatusMap.insert(std::make_pair(414, std::make_pair("URI Too Long", "El URI solicitado es demasiado largo para ser procesado por el servidor.")));
    httpStatusMap.insert(std::make_pair(415, std::make_pair("Unsupported Media Type", "El tipo de medio de la solicitud no es compatible con el servidor.")));
    httpStatusMap.insert(std::make_pair(416, std::make_pair("Range Not Satisfiable", "El rango especificado no es válido para el recurso solicitado.")));
    httpStatusMap.insert(std::make_pair(417, std::make_pair("Expectation Failed", "El servidor no puede cumplir con las expectativas dadas en la cabecera 'Expect'.")));
    httpStatusMap.insert(std::make_pair(418, std::make_pair("I'm a teapot", "Soy una tetera. Este código es una broma del Protocolo HTCPCP.")));
    httpStatusMap.insert(std::make_pair(421, std::make_pair("Misdirected Request", "La solicitud fue dirigida a un servidor que no puede producir una respuesta.")));
    httpStatusMap.insert(std::make_pair(425, std::make_pair("Too Early", "El servidor no está dispuesto a procesar una solicitud que puede ser repetida.")));
    httpStatusMap.insert(std::make_pair(426, std::make_pair("Upgrade Required", "El cliente debe cambiar a un protocolo diferente.")));
    httpStatusMap.insert(std::make_pair(428, std::make_pair("Precondition Required", "El servidor requiere que la solicitud sea condicional.")));
    httpStatusMap.insert(std::make_pair(429, std::make_pair("Too Many Requests", "El cliente ha enviado demasiadas solicitudes en un tiempo determinado.")));
    httpStatusMap.insert(std::make_pair(431, std::make_pair("Request Header Fields Too Large", "Las cabeceras de la solicitud son demasiado grandes.")));
    httpStatusMap.insert(std::make_pair(451, std::make_pair("Unavailable For Legal Reasons", "El recurso no está disponible por razones legales.")));
    httpStatusMap.insert(std::make_pair(500, std::make_pair("Internal Server Error", "El servidor encontró un error interno y no pudo completar la solicitud.")));
    httpStatusMap.insert(std::make_pair(501, std::make_pair("Not Implemented", "El servidor no tiene la funcionalidad requerida para cumplir con la solicitud.")));
    httpStatusMap.insert(std::make_pair(502, std::make_pair("Bad Gateway", "El servidor actuando como pasarela recibió una respuesta inválida.")));
    httpStatusMap.insert(std::make_pair(503, std::make_pair("Service Unavailable", "El servidor no está disponible para manejar la solicitud en este momento.")));
    httpStatusMap.insert(std::make_pair(504, std::make_pair("Gateway Timeout", "El servidor actuando como pasarela no recibió una respuesta a tiempo.")));
    httpStatusMap.insert(std::make_pair(505, std::make_pair("HTTP Version Not Supported", "La versión HTTP utilizada en la solicitud no es compatible con el servidor.")));
    httpStatusMap.insert(std::make_pair(506, std::make_pair("Variant Also Negotiates", "La negociación de contenido transparente para este recurso causó una referencia circular.")));
    httpStatusMap.insert(std::make_pair(507, std::make_pair("Insufficient Storage", "El servidor no puede almacenar la representación necesaria para completar la solicitud.")));
    httpStatusMap.insert(std::make_pair(510, std::make_pair("Not Extended", "El servidor requiere más extensiones para cumplir con la solicitud.")));
    httpStatusMap.insert(std::make_pair(511, std::make_pair("Network Authentication Required", "Es necesaria la autenticación de red para acceder al recurso.")));
}

Response::Response(Client *client, Server_config *config, Request *request)
:_client(client), _config(config), _request(request), _cgiFlag(false), _auto_index_flag(0)
{
	buildErrorMap();
	_responseString = "";
		_code = _request->getErrorCode();
	_hasIndexFlag = 0;
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
		return false;
	else if (!(info.st_mode & S_IFDIR))
		return false;
	if (access(path.c_str(), R_OK) != 0)
		return false;
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
    if (checkDirectoriesPath(_target.c_str()))
	    return (setReturnCode(403));				
	if (stat(_target.c_str(), &buffer) != 0)
		return (setReturnCode(404));
    if (!(buffer.st_mode & S_IRUSR))
		return (setReturnCode(403));
    std::ifstream file(_target.c_str());
    if (!file.is_open())
        return (setReturnCode(404));
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
	_response_str.append("\r\n");
}

std::string Response::getMatch(std::string path, std::vector<Location_config> locations)
{
	std::string locationMatch = "";
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
		locationMatch = longestMatch;
	return locationMatch;
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

int Response::getTarget()
{
	_location_config = NULL;
	std::string locationMatch = "";
	locationMatch = getMatch(_request->getPath(), _config->locations);
	if (!locationMatch.empty())
	{
		_location_config = find_location(*_config,locationMatch);
		std::string body = _request->getBody();
		if (_request->getBody().length() > _location_config->body_size)
			return (setReturnCode(413));
		if (!_location_config->methods.empty())
			if (isNotValidMethod())
				return (501);
		if (setIndex())
			return (413);
		else if (_target.size() == 0)
			_target= concatenatePaths(_config->root, _request->getPath(), "");
		if (_location_config->redir.first)
 		{
			_location = _location_config->redir.second;
			_response_body_str = "";
			_target = "";
			return (setReturnCode(_location_config->redir.first));
		}
		if (!_location_config->cgi.empty() && hasValidExtension(_request->getPath(), _location_config->cgi))
			return(launchCgi());
		if (!_hasIndexFlag && _location_config->autoindex && isReadableDirectory(_target))
		{
			_auto_index_flag = 1;
			return (0);
		}
	}
	else
		_target= concatenatePaths(_config->root, _request->getPath(), "");
	return 0;
}

void Response::getDefaultErrorBody(int code)
{
	std::cout << "en getDefaultErrorBody________________-" << std::endl;
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
    struct dirent *structDirent;
    DIR *dir;
    _response_body_str = "";
    dir = opendir(_target.c_str());
    if (dir == NULL)
    {
        std::cerr << "Error in opendir" << std::endl;
        return (1);
    }
    _response_body_str.append("<!DOCTYPE html>\n<html lang=\"es\">\n<head>\n<meta charset='UTF-8'>\n<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n");
    _response_body_str.append(
        "    <style>\n"
        "        .footer {\n"
        "            text-align: center;\n"
        "            font-size: 0.8rem;\n"
        "            margin-top: 60px;\n"
        "            color: #940f0f;\n"
        "            text-shadow: #e8fd81ad 1px 0px 2px;\n"
        "        }\n"
        "        .delete-button {\n"
        "            background-color: red;\n"
        "            color: white;\n"
        "            padding: 10px;\n"
        "            border: none;\n"
        "            border-radius: 5px;\n"
        "            cursor: pointer;\n"
        "            font-size: 16px;\n"
        "            margin-top: 20px;\n"
        "            display: none;\n"
        "            float: right;\n"
        "            margin-right: 10%;\n"
        "        }\n"
        "        .selected-row {\n"
        "            background-color: rgba(255, 255, 255, 0.1); /* Aclarar el fondo */\n"
        "            border: 1px solid #e8fd81;\n"
        "        }\n"
        "    </style>\n"
    );
    _response_body_str.append("<title>Index of ");
    _response_body_str.append(_target);
    _response_body_str.append("</title>\n</head>\n<body style=\"background-color: black; color: red; font-family: Arial, sans-serif;\">\n");
	_response_body_str.append("<h1 style=\"text-align: center;\">Index of " + _target + "</h1>\n");
    _response_body_str.append("<table style=\"width:80%; margin: auto; border-collapse: collapse; font-size: 15px;\">\n");
    _response_body_str.append("<hr style=\"border: 1px solid red; width:90%;margin-bottom: 0px;\">\n");
    _response_body_str.append("<hr style=\"border: 1px solid #e8fd81; width:90%; margin-top:0px;\">\n");
    _response_body_str.append("<thead style=\"color: #e8fd81;\">\n<tr>\n<th style=\"text-align:left; padding: 10px; border-bottom: 1px solid red;\">File Name</th>\n");
    _response_body_str.append("<th style=\"text-align:left; padding: 10px; border-bottom: 1px solid red;\">Last Modification</th>\n");
    _response_body_str.append("<th style=\"text-align:left; padding: 10px; border-bottom: 1px solid red;\">File Size</th>\n");
    _response_body_str.append("</tr>\n</thead>\n<tbody>\n");

    struct stat file_stat;
    std::string file_path;
    while ((structDirent = readdir(dir)) != NULL)
    {
        if (strcmp(structDirent->d_name, ".") == 0)
            continue;
        file_path = concatenatePaths(_target, structDirent->d_name, "");
        stat(file_path.c_str(), &file_stat);
        file_path = removeRoot(file_path, root);
        bool isFile = !S_ISDIR(file_stat.st_mode);
        _response_body_str.append("<tr id=\"");
        _response_body_str.append(structDirent->d_name);
        _response_body_str.append("\" onclick=\"selectFile('" + file_path + "', " + (isFile ? "true" : "false") + ")\">\n");
        _response_body_str.append("<td style=\"padding: 10px; border-bottom: 1px solid red;\">\n<a href=\"");
        _response_body_str.append(file_path);
        if (S_ISDIR(file_stat.st_mode))
            _response_body_str.append("/");
        _response_body_str.append("\" style=\"color: red; text-decoration: none;\">");
        _response_body_str.append(structDirent->d_name);
        if (S_ISDIR(file_stat.st_mode))
            _response_body_str.append("/");
        _response_body_str.append("</a></td>\n");
        _response_body_str.append("<td style=\"padding: 10px; border-bottom: 1px solid red;\">");
        _response_body_str.append(ctime(&file_stat.st_mtime));
        _response_body_str.append("</td>\n");
        _response_body_str.append("<td style=\"padding: 10px; border-bottom: 1px solid red;\">");
        if (!S_ISDIR(file_stat.st_mode))
            _response_body_str.append(toStr(file_stat.st_size));
        _response_body_str.append("</td>\n");
        _response_body_str.append("</tr>\n");
    }
    _response_body_str.append("</tbody>\n</table>\n");
    _response_body_str.append("<div><button class='delete-button' id='delete-button' onclick='deleteSelectedFile()'>Delete</button></div>\n");
    _response_body_str.append(
        "<script>\n"
        "    let selectedFile = '';\n"
        "    let isFile = false;\n"
        "    let previousSelectedRow = null;\n"
        "    function selectFile(filePath, file) {\n"
        "        if (previousSelectedRow) {\n"
        "            previousSelectedRow.classList.remove('selected-row');\n"
        "        }\n"
        "        let currentRow = document.getElementById(filePath.split('/').pop());\n"
        "        currentRow.classList.add('selected-row');\n"
        "        previousSelectedRow = currentRow;\n"
        "        selectedFile = filePath;\n"
        "        isFile = file;\n"
        "        if (isFile) {\n"
        "            document.getElementById('delete-button').style.display = 'block';\n"
        "        } else {\n"
        "            document.getElementById('delete-button').style.display = 'none';\n"
        "        }\n"
        "    }\n"
        "    function deleteSelectedFile() {\n"
        "        if (selectedFile === '') {\n"
        "            alert('No file selected!');\n"
        "            return;\n"
        "        }\n"
        "        fetch(selectedFile, { method: 'DELETE' })\n"
        "            .then(response => {\n"
        "                if (response.ok) {\n"
        "                    alert('File deleted successfully');\n"
        "                    window.location.reload();\n"
        "                } else {\n"
        "                    alert('Error deleting file');\n"
        "                }\n"
        "            })\n"
        "            .catch(error => {\n"
        "                console.error('Error:', error);\n"
        "                alert('Error deleting file');\n"
        "            });\n"
        "    }\n"
        "</script>\n"
    );
    _response_body_str.append("<div class='footer'>\n<p>Powered by Imoro-sa & Alfofern C++98 WebServer</p>\n</div>\n");
    _response_body_str.append("</body>\n</html>\n");
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
		buildErrorPage(_code);
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
	else if (_auto_index_flag)
    {
		if (!isReadableDirectory(_target))
			buildErrorPage(403);
		else if (buildDirHtml())
			buildErrorPage(500);
        else
            _code = 200;
	}
	else if (isReadableDirectory(_target) && !_auto_index_flag)
			buildErrorPage(403);
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
