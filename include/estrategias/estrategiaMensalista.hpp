#ifndef ESTRATEGIA_MENSALISTA_HPP
#define ESTRATEGIA_MENSALISTA_HPP

#include "estrategias/estrategiaPreco.hpp"

// Mensalista: 15% de desconto no valor base.
class EstrategiaMensalista : public EstrategiaPreco {
public:
    double calcularPreco(double precoBaseHora, int duracao, int /*horarioInicio*/) const override {
        return (precoBaseHora * duracao) * 0.85;  // 15% de desconto
    }
    std::string getNome() const override { return "Mensalista"; }
    std::string getDescricao() const override { return "15% de desconto"; }
};

#endif
