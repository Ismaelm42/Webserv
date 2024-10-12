#include <iostream>
#include <string>
#include <vector>

// Tabla de caracteres Base64
const std::string base64_chars = 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

// Función para codificar una cadena en Base64
std::string base64_encode(const std::string &input) {
    std::string encoded;
    int val = 0;
    int valb = -6;
    for (unsigned char c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            encoded.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) 
        encoded.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (encoded.size() % 4) 
        encoded.push_back('=');
    return encoded;
}

// Función para decodificar una cadena Base64
static inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_decode(const std::string &input) {
    std::vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    std::string decoded;
    int val = 0;
    int valb = -8;
    for (unsigned char c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            decoded.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return decoded;
}

// Función main para probar las funciones
int main() {
    std::string input = "Hola, Mundo!";
    std::cout << "Cadena original: " << input << std::endl;

    // Codificar en Base64
    std::string encoded = base64_encode(input);
    std::cout << "Codificado en Base64: " << encoded << std::endl;

    // Decodificar Base64
    std::string decoded = base64_decode(encoded);
    std::cout << "Decodificado: " << decoded << std::endl;

    return 0;
}
