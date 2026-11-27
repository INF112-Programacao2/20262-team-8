#ifndef RESERVA_HPP
#define RESERVA_HPP

#include "enums/StatusReserva.hpp"

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
        StatusReserva status;
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
        int getHorarioFim() const;
        StatusReserva getStatus() const;
        std::string getStatusTexto() const;
        bool estaAtiva() const;

        void validarHorarios();
        void solicitarCalculoValor();
        bool cancelarReserva();
        bool concluir();
        bool registrarFalta();
        bool temConflitoComOutraReserva(const Reserva& outra) const;
};

#endif