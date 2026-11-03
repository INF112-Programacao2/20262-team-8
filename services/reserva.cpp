#include "../models/reserva.hpp"
#include "../models/cliente.hpp"
#include "../models/quadra.hpp"
#include <string>
#include <algorithm>
#include <stdexcept>

std::vector<Reserva*> Reserva::reservasAtivas;

Reserva::Reserva(Cliente* cli, Quadra* q, const std::string& dt, int hrInicio, int horas)
    : cliente(cli), quadra(q), dataReserva(dt), horarioInicio(hrInicio), duracao(horas) {
    validarHorarios();
    solicitarCalculoValor();
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

void Reserva::validarHorarios() {
    if (horarioInicio < 0 || horarioInicio > 23) {
        throw std::invalid_argument("Horário de início inválido.");
    }
    
    if (duracao <= 0 || duracao > 24) {
        throw std::invalid_argument("Duração inválida.");
    }

    if (horarioInicio + duracao > 24) {
        throw std::invalid_argument("A reserva ultrapassa o horário permitido (24 horas).");
    }
}

void Reserva::solicitarCalculoValor() {
    if (quadra != nullptr) {
        valorTotal = quadra->calcularValorReserva(duracao);
    } else {
        valorTotal = 0.0;
    }
}

void Reserva::cancelarReserva() {
    auto it = std::find(reservasAtivas.begin(), reservasAtivas.end(), this);
    if (it != reservasAtivas.end()) {
        reservasAtivas.erase(it);
    }
}

bool Reserva::temConflitoComOutraReserva(const Reserva& outra) const {
    if (dataReserva != outra.dataReserva) {
        return false;
    }

    int fimAtual = horarioInicio + duracao;
    int fimOutra = outra.horarioInicio + outra.duracao;

    return !(fimAtual <= outra.horarioInicio || fimOutra <= horarioInicio);
}

int Reserva::getDuracaoHoras() const {
    return duracao;
}

double Reserva::getValorTotal() const {
    return valorTotal;
}

Cliente* Reserva::getCliente() const {
    return cliente;
}

Quadra* Reserva::getQuadra() const {
    return quadra;
}

std::vector<Reserva*> Reserva::listarReservasAtivas() {
    return reservasAtivas;
}

std::string Reserva::getDataReserva() const {
    return dataReserva;
}

int Reserva::getHorarioInicio() const {
    return horarioInicio;
}

void Reserva::adicionarReservaAtiva(Reserva* reserva) {
    reservasAtivas.push_back(reserva);
}