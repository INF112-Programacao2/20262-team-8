#ifndef SISTEMA_GESTAO_HPP
#define SISTEMA_GESTAO_HPP

#include "../models/cliente.hpp"
#include "../models/quadra.hpp"
#include "../models/reserva.hpp"
#include "services/servicoClientes.hpp"
#include "services/servicoReservas.hpp"

#include <string>
#include <vector>

class SistemaGestao {
public:
    int cadastrarCliente(const std::string& nome, const std::string& email);
    int getQtdClientes() const;
    Cliente* getCliente(int indice) const;

    Reserva* criarReserva(Cliente& cliente, Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras);
    bool cancelarReserva(Reserva& reserva);
    std::vector<Reserva*> listarReservasAtivas() const;
    bool verificarDisponibilidade(const Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras) const;
private:
    ServicoClientes servicoClientes;
    ServicoReservas servicoReservas;
};

#endif
