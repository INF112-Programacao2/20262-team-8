#include "models/reserva.hpp"

#include <stdexcept>

Reserva::Reserva(Cliente* cli, Quadra* q, const std::string& dt, int hrInicio, int horas)
    : cliente(cli), quadra(q), dataReserva(dt), horarioInicio(hrInicio), valorTotal(0.0),
      valorReembolso(0.0), duracao(horas), status(StatusReserva::Confirmada) {
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

bool Reserva::cancelarReserva(double reembolso) {
    if (!estaAtiva()) {
        return false;
    }
    valorReembolso = reembolso;
    status = StatusReserva::Cancelada;
    return true;
}

bool Reserva::concluir() {
    if (!estaAtiva()) {
        return false;
    }
    status = StatusReserva::Concluida;
    return true;
}

bool Reserva::registrarFalta() {
    if (!estaAtiva()) {
        return false;
    }
    status = StatusReserva::Falta;
    return true;
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

int Reserva::getHorarioFim() const {
    return horarioInicio + duracao;
}

StatusReserva Reserva::getStatus() const {
    return status;
}

std::string Reserva::getStatusTexto() const {
    switch (status) {
        case StatusReserva::Confirmada: return "Confirmada";
        case StatusReserva::Concluida: return "Concluida";
        case StatusReserva::Cancelada: return "Cancelada";
        case StatusReserva::Falta: return "Falta";
    }
    return "Desconhecido";
}

double Reserva::getValorTotal() const {
    return valorTotal;
}

double Reserva::getValorReembolso() const {
    return valorReembolso;
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
    return status == StatusReserva::Confirmada;
}