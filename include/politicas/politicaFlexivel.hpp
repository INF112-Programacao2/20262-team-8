#ifndef POLITICA_FLEXIVEL_HPP
#define POLITICA_FLEXIVEL_HPP

#include "politicas/politicaCancelamento.hpp"

// Flexivel: 100% ate 1 dia antes; 50% no proprio dia.
class PoliticaFlexivel : public PoliticaCancelamento {
public:
    int percentualReembolso(int diasAntes) const override {
        if (diasAntes >= 1) {
            return 100;
        }
        return 50;
    }
    std::string getNome() const override { return "Flexível"; }
    std::string getDescricao() const override { return "100% até 1 dia antes"; }
};

#endif
