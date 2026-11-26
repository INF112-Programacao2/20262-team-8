#include "services/servicoReservas.hpp"

#include <stdexcept>
#include <utility>

bool ServicoReservas::ehAnoBissexto(int ano) {
    return ano % 400 == 0 || (ano % 4 == 0 && ano % 100 != 0);
}

bool ServicoReservas::dataValida(const std::string& data) {
    if (data.size() != 10 || data[4] != '-' || data[7] != '-') {
        return false;
    }

    for (std::size_t i = 0; i < data.size(); ++i) {
        if (i != 4 && i != 7 && (data[i] < '0' || data[i] > '9')) {
            return false;
        }
    }

    const int ano = std::stoi(data.substr(0, 4));
    const int mes = std::stoi(data.substr(5, 2));
    const int dia = std::stoi(data.substr(8, 2));

    if (ano < 1 || mes < 1 || mes > 12) {
        return false;
    }

    const int diasPorMes[] = {
        31, ehAnoBissexto(ano) ? 29 : 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };
    return dia >= 1 && dia <= diasPorMes[mes - 1];
}

Reserva* ServicoReservas::criarReserva(Cliente& cliente, Quadra& quadra,
                                       const std::string& data, int horarioInicio,
                                       int duracaoHoras) {
    validarDadosReserva(data, horarioInicio, duracaoHoras);
    if (!verificarDisponibilidade(quadra, data, horarioInicio, duracaoHoras)) {
        throw std::runtime_error("A quadra ja possui uma reserva nesse horario.");
    }

    reservas.reserve(reservas.size() + 1);
    auto reserva = std::make_unique<Reserva>(&cliente, &quadra, data, horarioInicio,
                                             duracaoHoras);
    Reserva* reservaCriada = reserva.get();
    reservas.push_back(std::move(reserva));
    return reservaCriada;
}

bool ServicoReservas::cancelarReserva(Reserva& reserva) {
    for (const auto& reservaGerenciada : reservas) {
        if (reservaGerenciada.get() == &reserva && reservaGerenciada->estaAtiva()) {
            reserva.cancelarReserva();
            return true;
        }
    }
    return false;
}

std::vector<Reserva*> ServicoReservas::listarReservasAtivas() const {
    std::vector<Reserva*> reservasAtivas;
    for (const auto& reserva : reservas) {
        if (reserva->estaAtiva()) {
            reservasAtivas.push_back(reserva.get());
        }
    }
    return reservasAtivas;
}

bool ServicoReservas::verificarDisponibilidade(const Quadra& quadra,
                                                const std::string& data,
                                                int horarioInicio,
                                                int duracaoHoras) const {
    validarDadosReserva(data, horarioInicio, duracaoHoras);

    const int fimNovoHorario = horarioInicio + duracaoHoras;
    for (const auto& reserva : reservas) {
        if (reserva->estaAtiva() &&
            reserva->getQuadra() == &quadra &&
            reserva->getDataReserva() == data &&
            horarioInicio < reserva->getHorarioInicio() + reserva->getDuracaoHoras() &&
            reserva->getHorarioInicio() < fimNovoHorario) {
            return false;
        }
    }
    return true;
}

void ServicoReservas::validarDadosReserva(const std::string& data, int horarioInicio,
                                          int duracaoHoras) const {
    if (!dataValida(data)) {
        throw std::invalid_argument("Data invalida. Use o formato AAAA-MM-DD.");
    }
    if (horarioInicio < 0 || horarioInicio > 23) {
        throw std::invalid_argument("Horario de inicio invalido.");
    }
    if (duracaoHoras <= 0 || duracaoHoras > 24 ||
        horarioInicio + duracaoHoras > 24) {
        throw std::invalid_argument("Duracao invalida para o horario informado.");
    }
}
