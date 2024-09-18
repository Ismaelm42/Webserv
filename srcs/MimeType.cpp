#include "../includes/lib.hpp"

MimeType::MimeType()
{
_mime_types["avi"] = "video/x-msvideo";
_mime_types["bmp"] = "image/bmp";
_mime_types["css"] = "text/css";
_mime_types["doc"] = "application/msword";
_mime_types["gif"] = "image/gif";
_mime_types["gz"] = "application/x-gzip";
_mime_types["htm"] = "text/html";
_mime_types["html"] = "text/html";
_mime_types["ico"] = "image/x-icon";
_mime_types["jpeg"] = "image/jpeg";
_mime_types["jpg"] = "image/jpeg";
_mime_types["js"] = "application/javascript";
_mime_types["json"] = "application/json";
_mime_types["mp3"] = "audio/mp3";
_mime_types["mp4"] = "video/mp4";
_mime_types["pdf"] = "application/pdf";
_mime_types["png"] = "image/png";
_mime_types["svg"] = "image/svg+xml";
_mime_types["txt"] = "text/plain";
_mime_types["xml"] = "application/xml";
}

/**
 * @brief chequeuea si en el mapa Mime hay algun valor cuya key 
 * sea igual a la extension pasada como parametro, si la encuentra
 * retorna el valor de la key, si no encuentra la key retorna el valor
 * por defecto que es el .html = "text/html"
 * 
 * @param ext
 * @return std::string 
 */
std::string MimeType::getMimeType(std::string ext)
{
    if (_mime_types.count(ext))
        return (_mime_types[ext]);
     return (_mime_types["html"]);
}


/*
Editado para no tener los puntos en las claves de las extensiones
*/
/**
 * @brief esteblece el valor de Content-type, incluyendo el título "Content-Type: " 
 * y buscando el valor del mime type según la extensión del archivo target si el código es 200
 * se busca el utlimo punto en el target file y se obtiene la extension del archivo con substr 
 * si encuentra la extension en el mapa de mime types se agrega el valor de la key al response content
 * si no se encuentra la extension se agrega el valor por defecto = "text/html" 
 * si el archivo no tuviera estensión se agrega el valor por defecto = "text/html"
 */

/*
void   Response::contentType()									// setea el content type de la respuesta
{
    _response_content.append("Content-Type: ");					// agrega el string: "Content-type: "a la respuesta
    
	if(_target_file.rfind(".", std::string::npos) != std::string::npos && _code == 200)	// si el target file tiene un punto, empezando por el final lo identifica como extension 
        _response_content.append(_mime.getMimeType(_target_file.substr(_target_file.rfind(".", std::string::npos) + 1)) ); // obtiene el mime type de la extension del archivo
    else
        _response_content.append(_mime.getMimeType("html"));
    _response_content.append("\r\n");
}

// Mi versión //

void   Response::contentType()									// setea el content type de la respuesta
{
    _response_content.append("Content-Type: ");					// agrega el string: "Content-type: "a la respuesta
    std::string mime = "text/html";
	if(_target_file.rfind(".", std::string::npos) != std::string::npos && _code == 200)	// si el target file tiene un punto, empezando por el final lo identifica como extension 
        mime = _mime.getMimeType(_target_file.substr(_target_file.rfind(".", std::string::npos) + 1)); // obtiene el mime type de la extension del archivo
    _response_content.append(mime);   // Corregido: remover el paréntesis extra
    _response_content.append("\r\n");
}

*/


