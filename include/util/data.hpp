#ifndef DATA_HPP
#define DATA_HPP

#include <string>

// Funcoes de apoio para datas no formato AAAA-MM-DD (texto).
class Data {
public:
    static bool valida(const std::string& data);  // formato e dia/mes existentes
    // Quantos dias de "de" ate "ate" (negativo se "ate" for antes). Datas devem ser validas.
    static int diferencaEmDias(const std::string& de, const std::string& ate);
};

#endif
