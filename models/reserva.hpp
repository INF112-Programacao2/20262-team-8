#ifndef RESERVA_HPP
#define RESERVA_HPP

#include <string>
#include <vector>
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
        Reserva(Cliente* cli, Quadra* q, const std::string& dt, int hrInicio, int horas);
        ~Reserva();

        Cliente* getCliente() const;
        Quadra* getQuadra() const;
        std::string getDataReserva() const;
        int getHorarioInicio() const;
        double getValorTotal() const;
        int getDuracaoHoras() const;

        void validarHorarios();
        void solicitarCalculoValor();
        void cancelarReserva();
        bool temConflitoComOutraReserva(const Reserva& outra) const;

        static std::vector<Reserva*> listarReservasAtivas();
        static void adicionarReservaAtiva(Reserva* reserva);
};

#endif