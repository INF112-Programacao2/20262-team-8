#ifndef CONSULTA_MANUTENCAO_HPP
#define CONSULTA_MANUTENCAO_HPP

#include <string>

class ConsultaManutencao {
public:
    virtual ~ConsultaManutencao() = default;
    virtual bool quadraBloqueadaNaData(int quadraId, const std::string& data) const = 0;
};

#endif
