#include "models/reserva.hpp"

#include <stdexcept>

Reserva::Reserva(Cliente* cli, Quadra* q, const std::string& dt, int hrInicio, int horas)
    : cliente(cli), quadra(q), dataReserva(dt), horarioInicio(hrInicio), duracao(horas),
      ativa(true) {
    validarHorarios();
    solicitarCalculoValor();
    if (cliente != nullptr) {
        cliente->adicionarReserva(this);
    }
}

Reserva::~Reserva() {
    if (cliente != nullptr) {
        cliente->removerReserva(this);
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
        valorTotal = quadra->calcularValorReserva(duracao, horarioInicio);
    } else {
        valorTotal = 0.0;
    }
}

void Reserva::cancelarReserva() {
    ativa = false;
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

std::string Reserva::getDataReserva() const {
    return dataReserva;
}

int Reserva::getHorarioInicio() const {
    return horarioInicio;
}

bool Reserva::estaAtiva() const {
    return ativa;
}