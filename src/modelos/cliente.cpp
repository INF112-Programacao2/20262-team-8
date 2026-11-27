#include "modelos/cliente.hpp"
#include "util/geradorId.hpp"

Cliente::Cliente(const std::string& nome, const std::string& email) {
    id_ = proximoId<Cliente>();
    nome_ = nome;
    email_ = email;
    qtdReservas_ = 0;
    faltas_ = 0;
    desconto_ = 0;
}

int Cliente::getId() const {
    return id_;
}

std::string Cliente::getNome() const {
    return nome_;
}

std::string Cliente::getEmail() const {
    return email_;
}

int Cliente::getQtdReservas() const {
    return qtdReservas_;
}

Reserva* Cliente::getReserva(int indice) const {
    if (indice < 0 || indice >= qtdReservas_) {
        return nullptr;
    }
    return reservas_[indice];
}

void Cliente::adicionarReserva(Reserva* reserva) {
    if (reserva != nullptr && qtdReservas_ < MAX_RESERVAS) {
        reservas_[qtdReservas_] = reserva;
        qtdReservas_++;
    }
}

int Cliente::getFaltas() const {
    return faltas_;
}

void Cliente::registrarFalta() {
    faltas_++;
}

void Cliente::zerarFaltas() {
    faltas_ = 0;
}

bool Cliente::estaBloqueado() const {
    return faltas_ >= LIMITE_FALTAS;
}

int Cliente::getDescontoProximaReserva() const {
    return desconto_;
}

void Cliente::setDescontoProximaReserva(int percentual) {
    desconto_ = percentual;
}

void Cliente::receberCupom(int percentual) {
    if (percentual > desconto_) {
        desconto_ = percentual;
    }
}
