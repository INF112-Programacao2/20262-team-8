#ifndef ESTRATEGIA_PRECO_HPP
#define ESTRATEGIA_PRECO_HPP

#include <string>
#include "enums/TipoEstrategia.hpp"

// Estrategia de preco (padrao Strategy): cada subclasse decide como calcular
// o valor base de uma reserva. A quadra usa uma estrategia e pode troca-la.
class EstrategiaPreco {
public:
    virtual ~EstrategiaPreco() {}

    // Recebe o preco base/hora, a duracao e o horario de inicio.
    virtual double calcularPreco(double precoBaseHora, int duracao, int horarioInicio) const = 0;
    virtual std::string getNome() const = 0;
    virtual std::string getDescricao() const = 0;
};

// Cria (com new) a estrategia do tipo pedido. Quem recebe o ponteiro precisa dar delete
// (a Quadra faz isso sozinha). Definida em src/estrategias/estrategiaPreco.cpp.
EstrategiaPreco* criarEstrategia(TipoEstrategia tipo);

#endif
