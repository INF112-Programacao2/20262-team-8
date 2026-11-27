#ifndef POLITICA_RIGIDA_HPP
#define POLITICA_RIGIDA_HPP

#include "politicas/politicaCancelamento.hpp"

// Rigida: 50% com 7 dias ou mais; senao, nada.
class PoliticaRigida : public PoliticaCancelamento {
public:
    int percentualReembolso(int diasAntes) const override {
        if (diasAntes >= 7) {
            return 50;
        }
        return 0;
    }
    std::string getNome() const override { return "Rígida"; }
    std::string getDescricao() const override { return "50% até 7 dias antes"; }
};

#endif
