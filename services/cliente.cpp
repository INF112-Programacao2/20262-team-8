#include "../models/cliente.hpp"
#include "../models/reserva.hpp"
#include <string>
#include <vector>

static int gerarId() {
    static int id = 0;
    return ++id;
}

Cliente::Cliente(const std::string& nome, const std::string& email)
    : id(gerarId()), nome(nome), email(email) {}

Cliente::~Cliente() {
    for (Reserva* reserva : reservas) {
        delete reserva;
    }
}

std::vector<Reserva*> Cliente::getReservas() const {
    return reservas;
}

void Cliente::adicionarReserva(Reserva* reserva) {
    if (reserva != nullptr) {
        reservas.push_back(reserva);
    }
}

int Cliente::getId() const {
    return id;
}