#ifndef ESTRATEGIA_PICO_HPP
#define ESTRATEGIA_PICO_HPP

#include "estrategiaPreço.hpp"

class EstrategiaPico : public EstrategiaPreco {
public:
    double calcularPreco(double precoBaseHora, int duracao, int horarioInicio) const override {
        double valorTotal = 0.0;
        for (int h = 0; h < duracao; ++h) {
            int horaAtual = horarioInicio + h;
            // Aplica 20% de acréscimo entre as 18h e as 22h
            if (horaAtual >= 18 && horaAtual < 22) {
                valorTotal += precoBaseHora * 1.20;
            } else {
                valorTotal += precoBaseHora;
            }
        }
        return valorTotal;
    }
};

#endif