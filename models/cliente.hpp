#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>
#include <vector>

class Reserva;

class Cliente {
    private:
        int id;
        std::string nome;
        std::string email;
        std::vector<Reserva*> reservas;
    public:
        Cliente(const std::string& nome, const std::string& email);
        ~Cliente();

        int getId() const;
        std::string getNome() const;
        std::string getEmail() const;
        std::vector<Reserva*> getReservas() const;
        
        void adicionarReserva(Reserva* reserva);
};

#endif