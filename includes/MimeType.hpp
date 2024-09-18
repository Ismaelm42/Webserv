#pragma once
#include "common.hpp"

class MimeType
{
    public:
        MimeType();
        std::string getMimeType(std::string ext);			
    private:
        std::map<std::string, std::string> _mime_types;
        
};
