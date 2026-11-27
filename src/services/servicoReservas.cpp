#include "services/servicoReservas.hpp"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <utility>

ServicoReservas::ServicoReservas() {
    const std::time_t instante = std::time(nullptr);
    const std::tm* horarioLocal = std::localtime(&instante);
    if (horarioLocal == nullptr) {
        throw std::runtime_error("Nao foi possivel obter a data e hora locais.");
    }

    std::ostringstream dataFormatada;
    dataFormatada << std::put_time(horarioLocal, "%Y-%m-%d");
    dataHoje = dataFormatada.str();
    horaAgora = horarioLocal->tm_hour;
}

bool ServicoReservas::definirDataHoje(const std::string& data) {
    if (!dataValida(data)) {
        return false;
    }
    dataHoje = data;
    return true;
}

std::string ServicoReservas::getDataHoje() const {
    return dataHoje;
}

bool ServicoReservas::definirHoraAgora(int hora) {
    if (hora < 0 || hora > 23) {
        return false;
    }
    horaAgora = hora;
    return true;
}

int ServicoReservas::getHoraAgora() const {
    return horaAgora;
}

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
    if (data < dataHoje) {
        throw std::invalid_argument("Nao e possivel criar uma reserva em uma data passada.");
    }
    if (cliente.estaBloqueado()) {
        throw std::runtime_error("Cliente bloqueado por faltas sem aviso.");
    }
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
        if (reservaGerenciada.get() == &reserva) {
            return reserva.estaAtiva() && reserva.getDataReserva() >= dataHoje &&
                   reserva.cancelarReserva();
        }
    }
    return false;
}

bool ServicoReservas::concluirReserva(Reserva& reserva) {
    if (!pertenceAoServico(reserva) || !reserva.estaAtiva() || !jaComecou(reserva)) {
        return false;
    }
    return reserva.concluir();
}

bool ServicoReservas::registrarFalta(Reserva& reserva) {
    if (!pertenceAoServico(reserva) || !reserva.estaAtiva() || !jaComecou(reserva)) {
        return false;
    }
    if (!reserva.registrarFalta()) {
        return false;
    }
    reserva.getCliente()->registrarFalta();
    return true;
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

bool ServicoReservas::pertenceAoServico(const Reserva& reserva) const {
    for (const auto& reservaGerenciada : reservas) {
        if (reservaGerenciada.get() == &reserva) {
            return true;
        }
    }
    return false;
}

bool ServicoReservas::jaComecou(const Reserva& reserva) const {
    return reserva.getDataReserva() < dataHoje ||
           (reserva.getDataReserva() == dataHoje &&
            reserva.getHorarioInicio() <= horaAgora);
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
