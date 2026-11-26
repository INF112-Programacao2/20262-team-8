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
        bool ativa;
    public:
        Reserva(Cliente* cli, Quadra* q, const std::string& dt, int hrInicio, int horas);
        ~Reserva();
        Reserva(const Reserva&) = delete;
        Reserva& operator=(const Reserva&) = delete;

        Cliente* getCliente() const;
        Quadra* getQuadra() const;
        std::string getDataReserva() const;
        int getHorarioInicio() const;
        double getValorTotal() const;
        int getDuracaoHoras() const;
        bool estaAtiva() const;

        void validarHorarios();
        void solicitarCalculoValor();
        void cancelarReserva();
        bool temConflitoComOutraReserva(const Reserva& outra) const;
};

#endif