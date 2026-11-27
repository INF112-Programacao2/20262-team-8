#include "models/cliente.hpp"

#include <algorithm>
#include <stdexcept>
#include <string>
#include <vector>

static int gerarId() {
    static int id = 0;
    return ++id;
}

Cliente::Cliente(const std::string& nome, const std::string& email)
    : id(gerarId()), nome(nome), email(email), faltas(0), descontoProximaReserva(0) {}

std::vector<Reserva*> Cliente::getReservas() const {
    return reservas;
}

void Cliente::adicionarReserva(Reserva* reserva) {
    if (reserva != nullptr) {
        reservas.push_back(reserva);
    }
}

void Cliente::removerReserva(const Reserva* reserva) {
    reservas.erase(std::remove(reservas.begin(), reservas.end(), reserva), reservas.end());
}

int Cliente::getFaltas() const {
    return faltas;
}

bool Cliente::estaBloqueado() const {
    return faltas >= limiteFaltas;
}

void Cliente::registrarFalta() {
    ++faltas;
}

void Cliente::zerarFaltas() {
    faltas = 0;
}

int Cliente::getDescontoProximaReserva() const {
    return descontoProximaReserva;
}

void Cliente::setDescontoProximaReserva(int percentual) {
    if (percentual < 0 || percentual > 100) {
        throw std::invalid_argument("O desconto deve estar entre 0 e 100%.");
    }
    descontoProximaReserva = percentual;
}

void Cliente::receberCupom(int percentual) {
    if (percentual < 0 || percentual > 100) {
        throw std::invalid_argument("O desconto deve estar entre 0 e 100%.");
    }
    if (percentual > descontoProximaReserva) {
        descontoProximaReserva = percentual;
    }
}

int Cliente::getId() const {
    return id;
}

std::string Cliente::getNome() const {
    return nome;
}

std::string Cliente::getEmail() const {
    return email;
}
