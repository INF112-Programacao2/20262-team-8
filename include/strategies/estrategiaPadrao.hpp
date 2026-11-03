#ifndef ESTRATEGIA_PADRAO_HPP
#define ESTRATEGIA_PADRAO_HPP
#include "estrategiaPreço.hpp"
// EstrategiaPadrao.hpp
class EstrategiaPadrao : public EstrategiaPreco {
public:
    double calcularPreco(double precoBaseHora, int duracao, int horarioInicio) const override {
        return precoBaseHora * duracao;
    }
};
#endif