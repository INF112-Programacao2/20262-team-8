#include "../models/reserva.hpp"
#include "../models/cliente.hpp"
#include "../models/quadra.hpp"
#include <string>
#include <algorithm>
#include <stdexcept>

Reserva::Reserva(Cliente* cli, Quadra* q, std::string& dt, int hrInicio, int horas)
    : cliente(cli), quadra(q), dataReserva(dt), horarioInicio(hrInicio), duracao(horas) {
    valorTotal = 0.0;
    reservasAtivas.push_back(this);
    if (cliente != nullptr) {
        cliente->adicionarReserva(this);
    }
}

Reserva::~Reserva() {
    auto it = std::find(reservasAtivas.begin(), reservasAtivas.end(), this);
    if (it != reservasAtivas.end()) {
        reservasAtivas.erase(it);
    }
}
