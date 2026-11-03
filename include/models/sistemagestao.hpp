#ifndef SISTEMA_GESTAO_HPP
#define SISTEMA_GESTAO_HPP

#include "../models/cliente.hpp"
#include "../models/quadra.hpp"
#include "../models/reserva.hpp"

#include <string>
#include <vector>

class SistemaGestao {
public:
    Reserva* criarReserva(Cliente& cliente, Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras);
    bool cancelarReserva(Reserva& reserva);
    std::vector<Reserva*> listarReservasAtivas() const;
    bool verificarDisponibilidade(const Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras) const;
private:
    void validarDadosReserva(const std::string& data, int horarioInicio, int duracaoHoras) const;
    static bool ehAnoBissexto(int ano);
    static bool dataValida(const std::string& data);
};

#endif
