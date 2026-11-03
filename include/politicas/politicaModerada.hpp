#ifndef POLITICA_MODERADA_HPP
#define POLITICA_MODERADA_HPP

#include "politicas/politicaCancelamento.hpp"

// Moderada: 100% com 3 dias ou mais; 50% com 1 a 2 dias; 0% no proprio dia.
class PoliticaModerada : public PoliticaCancelamento {
public:
    int percentualReembolso(int diasAntes) const override {
        if (diasAntes >= 3) {
            return 100;
        }
        if (diasAntes >= 1) {
            return 50;
        }
        return 0;
    }
    std::string getNome() const override { return "Moderada"; }
    std::string getDescricao() const override { return "100% até 3 dias antes"; }
};

#endif
