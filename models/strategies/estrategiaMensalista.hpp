#ifndef ESTRATEGIA_MENSALISTA_HPP
#define ESTRATEGIA_MENSALISTA_HPP
#include "estrategiaPreço.hpp"
// EstrategiaMensalista.hpp
class EstrategiaMensalista : public EstrategiaPreco {
public:
    double calcularPreco(double precoBaseHora, int duracao, int horarioInicio) const override {
        return (precoBaseHora * duracao) * 0.85; // 15% de desconto
    }
};
#endif