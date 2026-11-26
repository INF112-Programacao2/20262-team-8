#ifndef SERVICO_RESERVAS_HPP
#define SERVICO_RESERVAS_HPP

#include "models/cliente.hpp"
#include "models/quadra.hpp"
#include "models/reserva.hpp"

#include <memory>
#include <string>
#include <vector>

class ServicoReservas {
public:
    Reserva* criarReserva(Cliente& cliente, Quadra& quadra, const std::string& data,
                          int horarioInicio, int duracaoHoras);
    bool cancelarReserva(Reserva& reserva);
    std::vector<Reserva*> listarReservasAtivas() const;
    bool verificarDisponibilidade(const Quadra& quadra, const std::string& data,
                                  int horarioInicio, int duracaoHoras) const;

private:
    void validarDadosReserva(const std::string& data, int horarioInicio,
                             int duracaoHoras) const;
    static bool ehAnoBissexto(int ano);
    static bool dataValida(const std::string& data);

    std::vector<std::unique_ptr<Reserva>> reservas;
};

#endif
