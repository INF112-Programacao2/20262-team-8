#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>
#include <vector>

class Reserva;

class Cliente {
    private:
        static const int limiteFaltas = 3;
        int id;
        std::string nome;
        std::string email;
        int faltas;
        // Referencias nao proprietarias; ServicoReservas e dono das reservas.
        std::vector<Reserva*> reservas;
    public:
        Cliente(const std::string& nome, const std::string& email);
        ~Cliente() = default;

        int getId() const;
        std::string getNome() const;
        std::string getEmail() const;
        std::vector<Reserva*> getReservas() const;
        
        void adicionarReserva(Reserva* reserva);
        void removerReserva(const Reserva* reserva);
        int getFaltas() const;
        bool estaBloqueado() const;
        void registrarFalta();
        void zerarFaltas();
};

#endif