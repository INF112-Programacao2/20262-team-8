#ifndef CONSULTA_MANUTENCAO_HPP
#define CONSULTA_MANUTENCAO_HPP

#include <string>

// Interface (classe abstrata) que responde: "a quadra esta em manutencao nessa data?".
// O ServicoReservas so conhece esta interface, nao o ServicoManutencoes. Assim os dois
// servicos nao dependem um do outro (inversao de dependencia).
class ConsultaManutencao {
public:
    virtual ~ConsultaManutencao() {}
    virtual bool quadraBloqueadaNaData(int quadraId, const std::string& data) const = 0;
};

#endif
