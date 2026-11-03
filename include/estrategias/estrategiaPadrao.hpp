#ifndef ESTRATEGIA_PADRAO_HPP
#define ESTRATEGIA_PADRAO_HPP

#include "estrategias/estrategiaPreco.hpp"

// Padrao: preco/hora x duracao, sem ajuste.
class EstrategiaPadrao : public EstrategiaPreco {
public:
    double calcularPreco(double precoBaseHora, int duracao, int /*horarioInicio*/) const override {
        return precoBaseHora * duracao;
    }
    std::string getNome() const override { return "Padrão"; }
    std::string getDescricao() const override { return "preço por hora, sem ajuste"; }
};

#endif
