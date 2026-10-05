#ifndef RESERVA_HPP
#define RESERVA_HPP

#include <string>
#include "cliente.hpp"
#include "quadra.hpp"

class Reserva {
    private:
        Cliente* cliente;
        Quadra* quadra;
        std::string dataReserva;
        int horarioInicio;
        double valorTotal;
        int duracao;

        static std::vector<Reserva*> reservasAtivas;
    public:
        Reserva(Cliente* cli, Quadra* q, const std::string& dt, const std::string& hrInicio, int horas);
        ~Reserva();

        void validarHorarios();
        void solicitarCalculoValor();
        void cancelarReserva();

        static std::vector<Reserva*> listarReservasAtivas();
        bool temConflitoComOutraReserva(const Reserva& outra) const;
        int getDuracaoHoras() const;
        double getValorTotal() const;
};

#endif