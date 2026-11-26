#include "models/sistemagestao.hpp"

Reserva* SistemaGestao::criarReserva(Cliente& cliente, Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras) {
    return servicoReservas.criarReserva(cliente, quadra, data, horarioInicio, duracaoHoras);
}

bool SistemaGestao::cancelarReserva(Reserva& reserva) {
    return servicoReservas.cancelarReserva(reserva);
}

std::vector<Reserva*> SistemaGestao::listarReservasAtivas() const {
    return servicoReservas.listarReservasAtivas();
}

bool SistemaGestao::verificarDisponibilidade(const Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras) const {
    return servicoReservas.verificarDisponibilidade(
        quadra, data, horarioInicio, duracaoHoras);
}
