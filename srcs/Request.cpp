#include "../includes/lib.hpp"

Request::Request(Client *client, Server_config *config)
:_client(client), _config(config)
{
	_initMethodStr();
	_path = "";
	_query = "";
	_fragment = "";
	_body_str = "";
	_error_code = 0;
	_uri_size = 0;
	_chunk_size = 0;
	_method = NONE;
	_ix = 0;
	_fillStatus = get_First;
	_headers_parsed = false;
	_get_body_flag = false;
	_body_parsed = false;
	_chunked_body_flag = false;
	_body_size = 0;
	_temp = "";
	_header_name_temp = "";
	_multiform_flag = false;
	_boundary = "";
}

void	Request::_initMethodStr()
{
	_methods_str[::GET] = "GET";
	_methods_str[::POST] = "POST";
	_methods_str[::DELETE] = "DELETE";
}

void Request::setClient(Client* client)
{
    _client = client;
}

void Request::setBodyStr(std::string body)
{
	_body_str = body;
}

int		Request::getErrorCode()
{
    return _error_code;
}

Methods  &Request::getMethod()
{
    return _method;
}

std::string &Request::getPath()
{
    return _path;
}

std::string &Request::getQuery()
{
    return _query;
}

std::string &Request::getFragment()
{
    return _fragment;
}

std::string Request::getHeader(std::string const &name)
{
    return _headers[name];
}

const std::map<std::string, std::string> &Request::getHeaders() const
{
	return _headers;
}

std::string Request::getMethodStr()
{
	return _methods_str[_method];
}

std::string &Request::getBody()
{
    return _body_str;
}

std::string     Request::getServerName()
{
    return _server_name;
}

int	Request::getPort()
{
	return _client->_port;
}

bool    Request::getMultiformFlag()
{
    return _multiform_flag;
}

std::string     &Request::getBoundary()
{
    return _boundary;
}

void	Request::saveHeader(std::string &name, std::string &value)
{
	static const char* spaces = " \t";
	value.erase(0, value.find_first_not_of(spaces));
	value.erase(value.find_last_not_of(spaces) + 1);
	for (size_t i = 0; i < name.length(); ++i)		
		name[i] = std::tolower(name[i]);
	_headers[name] = value;
}

void Request::_returnErr(int err, std::string msg, uint8_t charRead = 0)
{
	_error_code = err;
	std::cout << "Error = " << err <<": "<< msg << ": " << charRead << std::endl;
}

bool	allowedURIChar(uint8_t ch)
{
	if ((ch >= '#' && ch <= ';') || 
		(ch >= '?' && ch <= '[') || 
		(ch >= 'a' && ch <= 'z') ||
	  ch == '!' || ch == '=' || 
	  ch == ']' || ch == '_' || 
	  ch == '~')
		return (true);
	return (false);
}

bool	checkPath(std::string path)
{
	std::string tmp(path);
	char *res = strtok((char*)tmp.c_str(), "/");
	int pos = 0;
	while (res != NULL)
	{
		if (!strcmp(res, ".."))
			pos--;
		else
			pos++;
		if (pos < 0)
			return (1);
		res = strtok(NULL, "/");
	}
	return (0);
}

bool	isValidChar(uint8_t ch)
{
	if (ch == '!' || (ch >= '#' && ch <= '\'') || 
		ch == '*'|| ch == '+' ||
		ch == '-'  || ch == '.' ||
		(ch >= '0' && ch <= '9') || 
		(ch >= 'A' && ch <= 'Z') || 
		(ch >= 'a' && ch <= 'z') ||
		(ch >= '^' && ch <= '`') ||
		ch == '|')
		return (true);
	return (false);
}

void	Request::_handle_headers(){
	
	if (_headers.count("host"))
	{
		size_t pos = _headers["host"].find_first_of(':');
		_server_name = _headers["host"].substr(0, pos);
	}
	if (_headers.count("content-length"))
	{
		_get_body_flag = true;
		std::istringstream iss(_headers["content-length"]);
		iss >> _body_size;
		if (_body_size > _config->body_size)
			return (_returnErr(413, "Request Entity Too Large", 0));
	}
	if (_headers.count("transfer-encoding"))
	{
		_get_body_flag = true;
		if (_headers["transfer-encoding"].find("chunked") != std::string::npos)
			_chunked_body_flag = true;
	}
	if (_headers.count("content-type") && _headers["content-type"].find("multipart/form-data") != std::string::npos)
	{
		size_t pos = _headers["content-type"].find("boundary=");
		if (pos != std::string::npos)
			this->_boundary = _headers["content-type"].substr(pos + 9);
		this->_multiform_flag = true;
		std::cout << "_multiform_flag: " << _multiform_flag << std::endl;
		std::cout << "Boundary: " << _boundary << std::endl;
	}
}

bool Request::isValidUri()
{	
	std::stringstream ss;
	ss << _client->_port;
	std::string portStr = ss.str();
	std::string scheme = "http://";
	std::string uriTotal = scheme + _client->_host  + ":" + portStr + _path + _query + _fragment;
	if (uriTotal.length() > URI_MAX_LENGTH)
		return false;
	return true;
}

std::string decode(const std::string& value)
{
    std::string decoded;
    for (size_t i = 0; i < value.length(); ++i)
	{
        if (value[i] == '%')
		{
            if (i + 2 < value.length())
			{
                std::string hex = value.substr(i + 1, 2);
                char c = (char)strtol(hex.c_str(), NULL, 16);
                decoded += c;
                i += 2;
            }
        }
		else
            decoded += value[i];
    }
    return decoded;
}

void	Request::fillRequest(char *dt, size_t bytesRead)
{
	u_int8_t charRead;
	std::string protocol = "HTTP/1.1";
	static std::stringstream s;
	for (size_t i = 0; i < bytesRead; ++i)
	{
		charRead = dt[i];
		switch (_fillStatus)
		{
			case get_First:
			{
				if (charRead == 'G')
				  _method = GET;
				else if (charRead == 'P')
					_method = POST;
				else if (charRead == 'D')
					_method = DELETE;
				else
					return (_returnErr(501, "Method not Implemented, detected in char", charRead));
				_ix++;
				_fillStatus = get_Method;
				break ;	 
			}
			case get_Method:
			{
				if (charRead == _methods_str[_method][_ix])
					_ix++;
				else
					return (_returnErr(501, "Method Error, charRead is = ", charRead));
				if (size_t(_ix) == _methods_str[_method].length())
					_fillStatus = get_First_Space;
				break ;
			}
			case get_First_Space:
			{
				_ix = 0;
				if (charRead != ' ')
					return (_returnErr(400, "Bad charRead", charRead));
 				_fillStatus = get_First_Slash;
				continue ;
			}   
			case get_First_Slash:
			{
				if (charRead == '/')
				{
					_fillStatus = get_URI_Path;
					_temp.clear();
				}
				else
					return (_returnErr(400, "Bad charRead", charRead));
				break ;
			}
			case get_URI_Path:
			{
				if (charRead == ' ')
				{
					_fillStatus = get_Protocol;
					_path.append(_temp);
					_path = decode(_path);
					_temp.clear();
					if ( !isValidUri())
						return (_returnErr(414, "URI Too Long", charRead));
					if (checkPath(_path))
						return (_returnErr(400, "wrong path address", charRead));
					continue ;
				}
				else if (charRead == '?')
				{
					_fillStatus = get_URI_Query;
					_path.append(_temp);
					if (checkPath(_path))
						return (_returnErr(400, "wrong path address", charRead));
					_temp.clear();
					continue ;	
				}
				else if (charRead == '#')
				{
					_fillStatus = get_URI_Fragment;
					_path.append(_temp);
					if (checkPath(_path))
						return (_returnErr(400, "wrong path address", charRead));
					_temp.clear();
					continue ;
				}
				else if (!allowedURIChar(charRead))
					return (_returnErr(400, "character not allowed", charRead));
				else if ( i > URI_MAX_LENGTH)
					return (_returnErr(414, "URI Too Long", charRead));
				break ;
			}
			case get_URI_Query:
			{
				if (charRead == ' ')
				{
				_fillStatus = get_Protocol;
				_query.append(_temp);
				_temp.clear();
				if ( !isValidUri())
					return (_returnErr(414, "URI Too Long", charRead));
				continue ;
				}
				else if (charRead == '#')
				{
				_fillStatus = get_URI_Fragment;
				_query.append(_temp);
				_temp.clear();
				continue ;
				}
				else if (!allowedURIChar(charRead))
					return (_returnErr(400, "character not allowed", charRead));
				else if ( i > URI_MAX_LENGTH)
					return (_returnErr(414, "URI Too Long", charRead));
				break ;
			}
			case get_URI_Fragment:
			{
				if (charRead == ' ')
				{
					_fragment.append(_temp);
					_temp.clear();
					_fillStatus = get_Protocol;
					if ( !isValidUri())
						return (_returnErr(414, "URI Too Long", charRead));
					continue ;
				}
				else if (!allowedURIChar(charRead))
					return (_returnErr(400, "character not allowed", charRead));
				else if ( i > URI_MAX_LENGTH)
					return (_returnErr(414, "URI Too Long", charRead));
				break ;
			}
			case get_Protocol:
			{
				if (charRead != protocol[_ix])
					return (_returnErr(400, "wrong protocol or protocol not admitted", charRead));
				_ix++;
				if (size_t(_ix) == protocol.length())
					_fillStatus = get_CR;
				break ;
			}
			case get_CR:
			{
				_ix= 0;
				if (charRead != '\r')
					return (_returnErr(400, "wrong CR", charRead));
				_fillStatus = get_LF;
				break ;
			}
			case get_LF:
			{
				if (charRead != '\n')
					return (_returnErr(400, "wrong CR", charRead));
				_fillStatus = header_Name_Start;
				_temp.clear();
				continue ;
			}
			case header_Name_Start:
			{
				if (charRead == '\r')
					_fillStatus = headers_End;
				else if (isValidChar(charRead))
					_fillStatus = header_Name;
				else
					return (_returnErr(400, "wrong char in header name", charRead));	
				break ;
			}	
			case headers_End:
			{
				if (charRead == '\n')
				{
					_temp.clear();
					_headers_parsed = true;
					_handle_headers();
					if (_get_body_flag == 1)
					{
						_fillStatus = get_Body;
						if (_chunked_body_flag == true)
							_fillStatus = Chunk_Length_Begin;
						else
							_fillStatus = get_Body;
					}
					else
						_fillStatus = Parsed;
					continue ;
				}
				else
					return (_returnErr(400, "wrong char at the end of headers", charRead));	
				break ;
			}
			case header_Name:
			{
				if (charRead == ':')
 				{
					_fillStatus = header_Value;
					_header_name_temp = _temp;
					_temp.clear();
					continue ;
				}
				else if (!isValidChar(charRead))
					return (_returnErr(400, "wrong char in header name", charRead));
				break ;
			}
			case header_Value:
			{
				if (charRead == '\r')
				{
					saveHeader(_header_name_temp, _temp);
					_temp.clear();
					_header_name_temp.clear();
					_fillStatus = header_Value_End;
					continue ;
				}
				break ;
			}
			case header_Value_End:
			{
				if (charRead == '\n')
				{
					_fillStatus = header_Name_Start;
					continue ;
				}
				else
					return (_returnErr(400, "wrong char after header value", charRead));
				break ;
			}
			case get_Body:
			{
				if (_body.size() < _body_size)
					_body.push_back(charRead);
				if (_body.size() == _body_size)
				{
					_body_parsed = true;
					_fillStatus = Parsed;
				}
				break ;
			}	
			case Chunk_Length_Begin:
			{
				if (!isxdigit(charRead))
					return (_returnErr(400, "wrong chunk size", charRead));
				s.str("");
				s.clear();
				s << charRead;
				s >> std::hex >> _chunk_size;
				if (_chunk_size == 0)
					_fillStatus = Chunk_Length_CR;
				else
					_fillStatus = Chunk_Length;
				continue ;
			} 
			case Chunk_Length:
			{
				if (isxdigit(charRead) != 0)
				{
					int temp_len = 0;
					s.str("");
					s.clear();
					s << charRead;
					s >> std::hex >> temp_len;
					_chunk_size *= 16;
					_chunk_size += temp_len;
				}
				else if (charRead == '\r')
					_fillStatus = Chunk_Length_LF;
				else
					_fillStatus = Chunk_Ignore;
				continue ;
			}
			case Chunk_Length_CR:
			{
				if (charRead == '\r')
					_fillStatus = Chunk_Length_LF;
				else
					return (_returnErr(400, "wrong chunk size", charRead));
				continue ;
			}
			case Chunk_Length_LF:
			{
				if (charRead == '\n')
				{
					if (_chunk_size == 0)
						_fillStatus = Chunk_End_CR;
					else
						_fillStatus = Chunk_body;
				}
				else
					return (_returnErr(400, "wrong chunk LF", charRead));
				continue ;
			}
			case Chunk_Ignore:
			{
				if (charRead == '\r')
					_fillStatus = Chunk_Length_LF;
				continue ;
			}
			case Chunk_body:
			{
				_body.push_back(charRead);
				--_chunk_size;
				if (_chunk_size == 0)
					_fillStatus = Chunk_body_CR;
				continue ;
			}
			case Chunk_body_CR:
			{
				if ( charRead == '\r')
					_fillStatus = Chunk_body_LF;
				else
					return (_returnErr(400, "wrong chunk CR", charRead));
				continue ;
			}
			case Chunk_body_LF:
			{
				if ( charRead == '\n')
					_fillStatus = Chunk_Length_Begin;
				else
					return (_returnErr(400, "wrong chunk LF", charRead));
				continue ;
			}
			case Chunk_End_CR:
			{
				if (charRead != '\r')
					return (_returnErr(400, "wrong chunk CR", charRead));
				_fillStatus = Chunk_End_LF;
				continue ;
			}
			case Chunk_End_LF:
			{
				if (charRead != '\n')
				{
					_error_code = 400;
					std::cout << "Bad charRead (Chunk_End_LF)" << std::endl;
					return ;
				}
				_body_parsed = true;
				_fillStatus = Parsed;
				continue ;
			}
			case Parsed:
				return ;
		}
		_temp += charRead;
	}
	if( _fillStatus == Parsed)
		_body_str.append((char*)_body.data(), _body.size());
}

void	Request::reset()
{
	_path.clear();
	_error_code = 0;
	_query.clear();
	_fragment.clear();
	_method = NONE;
	_ix = 0;
	_fillStatus = get_First;
	_uri_size = 0;
	_body_size = 0;
	_chunk_size = 0x0;
	_temp.clear();
	_body_str = "";
	_header_name_temp.clear();
	_headers.clear();
	_server_name.clear();
	_body.clear();
	_boundary.clear();
	_headers_parsed = false;
	_get_body_flag = false;
	_body_parsed = false;
	_chunked_body_flag = false;
	_multiform_flag = false;
}

bool    Request::isParsed()
{
	if (_fillStatus == 	Parsed)
	    return (true);
	return (false);
}
