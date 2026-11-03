#ifndef ESTRATEGIA_PICO_HPP
#define ESTRATEGIA_PICO_HPP

#include "estrategias/estrategiaPreco.hpp"

// Horario de pico: +20% nas horas entre 18h e 22h.
class EstrategiaPico : public EstrategiaPreco {
public:
    double calcularPreco(double precoBaseHora, int duracao, int horarioInicio) const override {
        double valorTotal = 0.0;
        for (int h = 0; h < duracao; h++) {
            int horaAtual = horarioInicio + h;
            // 20% de acrescimo entre as 18h e as 22h
            if (horaAtual >= 18 && horaAtual < 22) {
                valorTotal += precoBaseHora * 1.20;
            } else {
                valorTotal += precoBaseHora;
            }
        }
        return valorTotal;
    }
    std::string getNome() const override { return "Horário de pico"; }
    std::string getDescricao() const override { return "+20% entre 18h e 22h"; }
};

#endif
