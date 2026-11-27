#ifndef RESERVA_HPP
#define RESERVA_HPP

#include "enums/StatusReserva.hpp"

#include <string>
#include "cliente.hpp"
#include "quadra.hpp"

class Reserva {
    private:
        int id;
        Cliente* cliente;
        Quadra* quadra;
        std::string dataReserva;
        int horarioInicio;
        double valorTotal;
        double valorReembolso;
        int duracao;
        int descontoPercentual;
        StatusReserva status;
        bool avaliada;
    public:
        Reserva(Cliente* cli, Quadra* q, const std::string& dt, int hrInicio, int horas,
                int descontoPercentual = 0);
        ~Reserva();
        Reserva(const Reserva&) = delete;
        Reserva& operator=(const Reserva&) = delete;

        int getId() const;
        Cliente* getCliente() const;
        Quadra* getQuadra() const;
        std::string getDataReserva() const;
        int getHorarioInicio() const;
        double getValorTotal() const;
        double getValorReembolso() const;
        int getDescontoPercentual() const;
        int getDuracaoHoras() const;
        int getHorarioFim() const;
        StatusReserva getStatus() const;
        std::string getStatusTexto() const;
        bool estaAtiva() const;
        bool foiAvaliada() const;
        void marcarAvaliada();

        void validarHorarios();
        void solicitarCalculoValor();
        bool cancelarReserva(double reembolso = 0.0);
        bool concluir();
        bool registrarFalta();
        bool temConflitoComOutraReserva(const Reserva& outra) const;
};

#endif