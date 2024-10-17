#pragma once
#include <arpa/inet.h>
#include <cerrno>
#include <csignal>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <set>
#include <signal.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/epoll.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <dirent.h> 

#define	Black	"\e[0;30m"
#define	Red		"\e[0;31m"
#define	Green	"\e[0;32m"
#define	Yellow	"\e[0;33m"
#define	Blue	"\e[0;34m"
#define	Purple	"\e[0;35m"
#define	Cyan	"\e[0;36m"
#define	White	"\e[0;37m"

#define High_Black	"\e[0;90m"
#define	High_Red	"\e[0;91m"
#define	High_Green	"\e[0;92m"
#define	High_Yellow	"\e[0;93m"
#define	High_Blue	"\e[0;94m"
#define	High_Purple	"\e[0;95m"
#define	High_Cyan	"\e[0;96m"
#define	High_White	"\e[0;97m"
#define Reset		"\e[0m"
#define Log			"\e[0;90m[Webserv] \e[0m"

extern int globalSigint;

struct Location_config
{
	bool autoindex;
	size_t body_size;
	std::string location;
	std::string root;
	std::set<std::string> methods;
	std::vector<std::string> index;
	std::pair<int, std::string> redir;
	std::vector<std::pair<std::string, std::string> > cgi;
	std::string auth_basic;
	std::string auth_basic_user_file;
	Location_config() : autoindex(false) , body_size(0) {};
};

struct Server_config
{
	size_t body_size;
	std::string root;
	std::vector<std::string> index;
	std::vector<std::string> server_names;
	std::map<int, std::string> error_pages;
	std::vector<Location_config> locations;
	std::vector<std::pair<std::string, int> > host_port;
	Server_config() : body_size(1000000) {};
};

struct Epoll_events
{
	int epfd;
	std::vector<struct epoll_event> log;
	std::map<int, struct epoll_event> added;
};

template <typename T>
std::string toStr(const T toConvert)
{
    std::stringstream stream;
	stream << toConvert;
    return stream.str();
}

inline int checkFileOrDirectory(std::string &path, const std::string type)
{
	struct stat stat_buffer;
    if (stat(path.c_str(), &stat_buffer) < 0)
    {
		if (errno == ENOENT)
		{
			std::cout << Log << Red << "Error: " << path << " not found" << Reset << std::endl;
			return 404;
		}
		else if (errno == EACCES || !(stat_buffer.st_mode & S_IRUSR))
		{
			std::cout << Log << Red << "Error line 106: " << path << " permission denied" << Reset << std::endl;
			return 403;
		}
		else
			return 500;
    }
	if ((type == "dir" && !S_ISDIR(stat_buffer.st_mode)) || (type == "file" && S_ISDIR(stat_buffer.st_mode)))
		return 400;
	return 0;
}

const std::string base64_chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

static inline bool is_base64(unsigned char c){
    return (isalnum(c) || (c == '+') || (c == '/'));
}

enum Methods
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

#define HTML_ST "<!DOCTYPE html>\n" \
"<html>\n" \
"	<head>\n" \
"		<meta charset=\"UTF-8\">\n" \
"		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n" \
"		<title>WeBSeRV</title>\n" \
"		<link rel=\"icon\" href=\"../favicon.ico\" type=\"image/x-icon\">\n" \
"		<style>\n" \
"	        body {\n" \
"	            font-family: 'Arial', sans-serif;\n" \
"	            background-color: #1a1a1a;\n" \
"	            color: #e0e0e0;\n" \
"	            margin: 0;\n" \
"	            padding: 0;\n" \
"	        }\n" \
"            .delete-button {\n" \
"                background-color: red;\n" \
"                color: white;\n" \
"                padding: 10px;\n" \
"                border: none;\n" \
"                border-radius: 5px;\n" \
"                cursor: pointer;\n" \
"                font-size: 16px;\n" \
"                margin-top: 20px;\n" \
"                display: none;\n" \
"                float: right;\n" \
"                margin-right: 10%;\n" \
"            }\n" \
"            .selected-row {\n" \
"                background-color: rgba(255, 255, 255, 0.1);\n" \
"                border: 1px solid #e8fd81;\n" \
"            }\n" \
"	        header {\n" \
"	            background-color: #000000;\n" \
"	            border-bottom: 5px solid #ff0000;\n" \
"	            padding: 20px 0;\n" \
"	            text-align: center;\n" \
"	        }\n" \
"	        header h1 {\n" \
"	            color: #ff0000;\n" \
"	            font-size: 2.5em;\n" \
"	            font-weight: bold;\n" \
"	            text-transform: uppercase;\n" \
"	            letter-spacing: 5px;\n" \
"	            margin: 0;\n" \
"				line-height: 1.5em;\n" \
"	        }\n" \
"			h1 span {\n" \
"				font-family: Impact, Haettenschweiler, 'Arial Narrow Bold', sans-serif;\n" \
"				font-size: 1.2em;\n" \
"				color: red;\n" \
"				font-weight: bold;\n" \
"			}\n" \
"	        nav ul {\n" \
"	            list-style: none;\n" \
"	            padding: 0;\n" \
"	            display: flex;\n" \
"	            justify-content: center;\n" \
"	            background-color: #ff0000;\n" \
"	            margin: 0;\n" \
"				line-height: 2.5em;\n" \
"	        }\n" \
"	        nav ul li {\n" \
"	            position: relative;\n" \
"	            margin: 0 15px;\n" \
"	        }\n" \
"	        nav ul li a {\n" \
"	            color: #1a1a1a;\n" \
"	            text-decoration: none;\n" \
"	            font-size: 1.2em;\n" \
"	            text-transform: uppercase;\n" \
"	            font-weight: bold;\n" \
"	            letter-spacing: 2px;\n" \
"	        }\n" \
"	        nav ul li a:hover {\n" \
"	            color: #ffffff;\n" \
"	        }\n" \
"			footer {\n" \
"	            position: fixed;\n" \
"	            width: 100%;\n" \
"	            font-size: 14px;\n" \
"	            color: #666;\n" \
"	            text-align: center;\n" \
"	        }\n" \
"	        .footer-text {\n" \
"	            margin: 0;\n" \
"	        }\n" \
"	        nav ul li ul {\n" \
"	            display: none;\n" \
"	            position: absolute;\n" \
"	            top: 100%;\n" \
"	            left: 0;\n" \
"	            background-color: #ff0000;\n" \
"	            padding: 0;\n" \
"	            list-style: none;\n" \
"	            margin: 0;\n" \
"	        }\n" \
"	        nav ul li ul li {\n" \
"	            width: 150px;\n" \
"	            margin: 0;\n" \
"	        }\n" \
"	        nav ul li ul li a {\n" \
"	            color: #1a1a1a;\n" \
"	            text-transform: none;\n" \
"	            font-size: 1em;\n" \
"	            letter-spacing: 1px;\n" \
"	            padding: 10px;\n" \
"	            display: block;\n" \
"	        }\n" \
"	        nav ul li ul li a:hover {\n" \
"	            color: #ffffff;\n" \
"	        }\n" \
"	        nav ul li:hover ul {\n" \
"	            display: block;\n" \
"	        }\n" \
"	        nav ul li ul li a {\n" \
"	            transition: color 0.3s ease;\n" \
"	        }\n" \
"			nav {\n" \
"	            border-bottom: 5px solid #ff0000;\n" \
"			}\n" \
"	        #main-content {\n" \
"				position: absolute;\n" \
"	            top: 0;\n" \
"	            left: 0;\n" \
"	            z-index: 5;\n" \
"				width: 100%;\n" \
"	            height: 100%;\n" \
"	        }\n" \
"            table {\n" \
"                width: 80%;\n" \
"                margin: 20px auto;\n" \
"                border-collapse: collapse;\n" \
"                font-size: 15px;\n" \
"            }\n" \
"            table tr:hover {\n" \
"                background-color: rgba(255, 0, 0, 0.8); /* Cambia este valor según el color deseado */\n" \
"            }\n" \
"            th, td {\n" \
"                padding: 10px;\n" \
"                text-align: left;\n" \
"                border: 1px solid #e8fd81;\n" \
"            }\n" \
"            th {\n" \
"                background-color: #333;\n" \
"                color: #e8fd81;\n" \
"            }\n" \
"            tr:nth-child(even) {\n" \
"                background-color: rgba(255, 255, 255, 0.1);\n" \
"            }\n" \
"		</style>\n" \
"	</head>\n" \
"	<body>\n" \
"		<div id=\"main-content\">\n" \
"		    <header>\n" \
"				<h1>- <span>42 - </span>WEBSERVER<span> - 42</span> -</h1>\n" \
"			</header>\n" \

#define HTML_EN "                </tbody>\n" \
"            </table>\n" \
"			<div><button class='delete-button' id='delete-button' onclick='deleteSelectedFile()'>Delete</button></div>\n" \
"			<script>\n" \
"			    let selectedFile = '';\n" \
"			    let isFile = false;\n" \
"			    let previousSelectedRow = null;\n" \
"			    function selectFile(filePath, file) {\n" \
"			        if (previousSelectedRow) {\n" \
"			            previousSelectedRow.classList.remove('selected-row');\n" \
"			        }\n" \
"			        let currentRow = document.getElementById(filePath.split('/').pop());\n" \
"			        currentRow.classList.add('selected-row');\n" \
"			        previousSelectedRow = currentRow;\n" \
"			        selectedFile = filePath;\n" \
"			        isFile = file;\n" \
"			        if (isFile) {\n" \
"			            document.getElementById('delete-button').style.display = 'block';\n" \
"			        } else {\n" \
"			            document.getElementById('delete-button').style.display = 'none';\n" \
"			        }\n" \
"			    }\n" \
"			    function deleteSelectedFile() {\n" \
"			        if (selectedFile === '') {\n" \
"			            alert('No file selected!');\n" \
"			            return;\n" \
"			        }\n" \
"			        fetch(selectedFile, { method: 'DELETE' })\n" \
"			            .then(response => {\n" \
"			                if (response.ok) {\n" \
"			                    alert(selectedFile.split('/').pop() + ' deleted successfully');\n" \
"			                    window.location.reload();\n" \
"			                } else {\n" \
"			                    alert(selectedFile.split('/').pop() + 'Error deleting file');\n" \
"			                }\n" \
"			            })\n" \
"			            .catch(error => {\n" \
"			                console.error('Error:', error);\n" \
"			                alert('Error deleting' + selectedFile.split('/').pop());\n" \
"			            });\n" \
"			    }\n" \
"			</script>\n" \
"			<footer>\n" \
"				<p class=\"footer-text\">2024 Imoro-sa & Alfofern Webserv. All Rights Reserved.</p>\n" \
"			</footer>\n" \
"		</div>\n" \
"	</body>\n" \
"    <script>" \
"       document.addEventListener(\"DOMContentLoaded\", function() { \n" \
"            const currentUrl = window.location.href; \n" \
"            const baseUrl = currentUrl.match(/^(.*:\\d+)/)[0];\n" \
"            const navLinks = document.querySelectorAll(\"nav a\");\n" \
"            navLinks.forEach(link => {\n" \
"                const relativeHref = link.getAttribute(\"href\");\n" \
"                if (relativeHref && !relativeHref.startsWith(\"http\")) {\n" \
"                    link.setAttribute(\"href\", baseUrl + relativeHref);\n" \
"                }\n" \
"            });\n" \
"        });\n" \
"    </script>\n" \
"</html>\n"
