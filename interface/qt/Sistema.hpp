#ifndef SISTEMA_HPP
#define SISTEMA_HPP

// Fachada sem dependência de Qt. Concentra as regras que o main.cpp (CLI) faz
// "inline" (validação, conflito de horário, posse dos objetos) para que a
// interface gráfica só precise chamar métodos e exibir o resultado.

#include "../../models/cliente.hpp"
#include "../../models/quadra.hpp"
#include "../../models/reserva.hpp"

#include <memory>
#include <string>
#include <vector>

class Sistema {
    public:
        // Lançam std::invalid_argument com mensagem pronta para exibição.
        Cliente* cadastrarCliente(const std::string& nome, const std::string& email);
        Quadra* cadastrarQuadra(const std::string& nome, const std::string& localizacao,
                                const std::vector<TipoQuadra>& tipos, double precoHora);

        // Lança std::invalid_argument (dados inválidos) ou std::runtime_error (conflito).
        Reserva* criarReserva(Cliente* cliente, Quadra* quadra, const std::string& data,
                              int inicio, int duracao);
        void cancelarReserva(Reserva* reserva);

        const std::vector<std::unique_ptr<Cliente>>& clientes() const { return clientes_; }
        const std::vector<std::unique_ptr<Quadra>>& quadras() const { return quadras_; }
        std::vector<Reserva*> reservasAtivas() const { return Reserva::listarReservasAtivas(); }

        // Reserva ativa que ocupa a quadra na data/hora cheia informada (ou nullptr).
        Reserva* reservaEm(const Quadra* quadra, const std::string& data, int hora) const;
        bool temConflito(const Quadra* quadra, const std::string& data,
                         int inicio, int duracao) const;

        static std::string nomeTipo(TipoQuadra tipo);
        static const std::vector<TipoQuadra>& todosOsTipos();

    private:
        std::vector<std::unique_ptr<Cliente>> clientes_;
        std::vector<std::unique_ptr<Quadra>> quadras_;
};

#endif
