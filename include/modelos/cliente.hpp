#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include <string>

class Reserva;

class Cliente {
public:
    // Com este numero de faltas sem aviso o cliente fica bloqueado.
    static const int LIMITE_FALTAS = 3;
    static const int MAX_RESERVAS = 500;

    // O id e gerado automaticamente.
    Cliente(const std::string& nome, const std::string& email);

    int getId() const;
    std::string getNome() const;
    std::string getEmail() const;

    // Reservas do cliente (so guarda referencias: quem libera a memoria
    // das reservas e o SistemaGestao).
    int getQtdReservas() const;
    Reserva* getReserva(int indice) const;  // nullptr se o indice for invalido
    void adicionarReserva(Reserva* reserva);

    // Faltas sem aviso
    int getFaltas() const;
    void registrarFalta();
    void zerarFaltas();
    bool estaBloqueado() const;

    // Cupom de desconto (em %) que vale para a proxima reserva.
    int getDescontoProximaReserva() const;
    void setDescontoProximaReserva(int percentual);
    // So troca o cupom se o novo for maior que o que o cliente ja tem.
    void receberCupom(int percentual);

private:
    int id_;
    std::string nome_;
    std::string email_;
    Reserva* reservas_[MAX_RESERVAS];
    int qtdReservas_;
    int faltas_;
    int desconto_;
};

#endif
