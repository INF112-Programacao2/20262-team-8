#ifndef SERVICO_RESERVAS_HPP
#define SERVICO_RESERVAS_HPP

#include <string>
#include "modelos/reserva.hpp"
#include "sistema/consultaManutencao.hpp"
#include "sistema/mensagens.hpp"
#include "sistema/relogio.hpp"
#include "sistema/servicoClientes.hpp"
#include "sistema/servicoQuadras.hpp"
#include "util/colecao.hpp"

// Regras das reservas: criar, cancelar, concluir e registrar falta. E dono dos objetos Reserva.
class ServicoReservas {
public:
    // As listas dentro de Reserva e de Cliente ainda sao arrays fixos, entao as reservas
    // continuam com limite (as outras classes do sistema crescem sem limite).
    static const int MAX_RESERVAS = Reserva::MAX_ATIVAS;

    ServicoReservas(Mensagens& mensagens, const Relogio& relogio, ServicoClientes& clientes,
                    ServicoQuadras& quadras);

    // Quem responde se a quadra esta em manutencao (pode ficar sem, e ai nunca bloqueia).
    void definirConsultaManutencao(const ConsultaManutencao* consulta);

    // Sobrecarga: sem duracao, a reserva dura 1 hora (padrao). Devolve o id ou -1.
    int criar(int clienteId, int quadraId, const std::string& data, int horarioInicio);
    int criar(int clienteId, int quadraId, const std::string& data, int horarioInicio,
              int duracao);
    // Mesma coisa, recebendo os objetos. Devolve nullptr se der erro.
    Reserva* criar(Cliente& cliente, Quadra& quadra, const std::string& data, int horarioInicio,
                   int duracaoHoras);

    // Devolve o valor reembolsado (ou -1 se deu erro).
    double cancelar(int reservaId);
    bool cancelar(Reserva& reserva);
    bool concluir(int reservaId);
    bool registrarFalta(int reservaId);

    // Cancela (com reembolso total e cupom para o cliente) as reservas confirmadas da quadra
    // entre as duas datas. Devolve quantas foram canceladas. Usado quando ha manutencao.
    int cancelarNoPeriodo(int quadraId, const std::string& dataInicio, const std::string& dataFim,
                          int cupomPercentual);

    bool verificarDisponibilidade(const Quadra& quadra, const std::string& data,
                                  int horarioInicio, int duracaoHoras) const;

    Reserva* achar(int id) const;  // nullptr se nao existir
    int quantidade() const;
    Reserva* obter(int indice) const;  // nullptr se a posicao for invalida

private:
    ServicoReservas(const ServicoReservas&);
    ServicoReservas& operator=(const ServicoReservas&);

    // A reserva ja comecou? (dia anterior a hoje, ou hoje com a hora de inicio ja passada)
    bool jaComecou(const Reserva& reserva) const;
    std::string motivoNaoComecou(const Reserva& reserva) const;

    Mensagens& mensagens_;
    const Relogio& relogio_;
    ServicoClientes& clientes_;
    ServicoQuadras& quadras_;
    const ConsultaManutencao* manutencao_;
    Colecao<Reserva> reservas_;
};

#endif
