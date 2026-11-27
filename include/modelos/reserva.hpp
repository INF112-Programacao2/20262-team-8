#ifndef RESERVA_HPP
#define RESERVA_HPP

#include <string>
#include "enums/StatusReserva.hpp"
#include "modelos/cliente.hpp"
#include "modelos/quadra.hpp"

class Reserva {
public:
    static const int MAX_ATIVAS = 500;

    // O id e gerado automaticamente. O valor e calculado na criacao (com o cupom, se houver).
    // A reserva entra na lista de reservas ativas e na lista do cliente.
    Reserva(Cliente* cliente, Quadra* quadra, const std::string& dataReserva, int horarioInicio,
            int duracao, int descontoPercentual = 0);
    ~Reserva();

    int getId() const;
    Cliente* getCliente() const;
    Quadra* getQuadra() const;
    std::string getDataReserva() const;
    int getHorarioInicio() const;
    int getDuracaoHoras() const;
    int getHorarioFim() const;
    double getValorTotal() const;
    int getDesconto() const;

    // Estados: Confirmada -> Concluida, Cancelada ou Falta.
    // Sair de "Confirmada" tira a reserva da lista de ativas.
    StatusReserva getStatus() const;
    std::string getStatusTexto() const;
    bool estaAtiva() const;
    void concluir();
    void registrarFalta();
    void cancelarReserva(double reembolso);
    double getReembolso() const;

    // Cada reserva concluida pode receber uma unica avaliacao.
    bool foiAvaliada() const;
    void marcarAvaliada();

    // Conflito de horario com outra reserva (mesma data e horarios que se sobrepoem).
    // Compara so data e horario: nao olha a quadra nem o status das reservas.
    bool temConflitoComOutraReserva(const Reserva& outra) const;
    // Conflito com um pedido de reserva (so reservas confirmadas ocupam o horario).
    bool conflitaCom(int quadraId, const std::string& data, int inicio, int duracao) const;

    // Lista estatica das reservas ativas (confirmadas) de todo o sistema.
    static int contarReservasAtivas();
    static Reserva* getReservaAtiva(int indice);  // nullptr se o indice for invalido

private:
    // A reserva e criada com new e guardada em varios lugares: nao pode ser copiada.
    Reserva(const Reserva&);
    Reserva& operator=(const Reserva&);

    // Calcula o valor (so o construtor precisa chamar).
    void solicitarCalculoValor();

    static void adicionarReservaAtiva(Reserva* reserva);
    static void removerReservaAtiva(Reserva* reserva);

    int id_;
    Cliente* cliente_;
    Quadra* quadra_;
    std::string dataReserva_;
    int horarioInicio_;
    int duracao_;
    int desconto_;      // cupom (%) usado nesta reserva
    double valorTotal_; // calculado na criacao, via polimorfismo
    double reembolso_;  // quanto foi devolvido, se foi cancelada
    StatusReserva status_;
    bool avaliada_;

    static Reserva* reservasAtivas_[MAX_ATIVAS];
    static int qtdAtivas_;
};

#endif
