#include "models/quadra.hpp"
#include "models/cliente.hpp"
#include "models/reserva.hpp"
#include "enums/TipoQuadra.hpp"

#include <iostream>
#include <string>
#include <vector>

int main() {
    Cliente cliente("Rhyan Lelis", "rhyanlelis@ufv.br");
    Cliente cliente2("João Silva", "joao.silva@ufv.br");
    Cliente cliente3("Maria Souza", "maria.souza@ufv.br");

    std::string nomeQuadra = "Quadra Poliesportiva";
    std::string localizacaoQuadra = "Campus UFV";
    std::vector<TipoQuadra> modalidades = {FUTEBOL, BASQUETE, VOLEI};
    Quadra quadra(nomeQuadra, localizacaoQuadra, modalidades, 80.0);

    std::string dataReserva = "2026-10-07";
    Reserva* reserva1 = new Reserva(&cliente, &quadra, dataReserva, 9, 2);
    Reserva* reserva2 = new Reserva(&cliente2, &quadra, dataReserva, 11, 1);
    Reserva* reserva3 = new Reserva(&cliente3, &quadra, dataReserva, 14, 2);

    auto reservas = Reserva::listarReservasAtivas();

    std::cout << "Reservas Ativas:\n";
    for (const auto& reserva : reservas) {
        std::cout << "Cliente: " << reserva->getCliente()->getNome() << ", Quadra: " << reserva->getQuadra()->getNome()
                  << ", Data: " << reserva->getDataReserva() << ", Horário Início: " << reserva->getHorarioInicio()
                  << "h, Duração: " << reserva->getDuracaoHoras() << "h, Valor Total: R$" << reserva->getValorTotal() << "\n";
    }

    return 0;
}