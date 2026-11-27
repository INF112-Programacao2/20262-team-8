#ifndef SERVICO_MANUTENCOES_HPP
#define SERVICO_MANUTENCOES_HPP

#include <string>
#include "modelos/manutencao.hpp"
#include "sistema/consultaManutencao.hpp"
#include "sistema/mensagens.hpp"
#include "sistema/relogio.hpp"
#include "sistema/servicoQuadras.hpp"
#include "sistema/servicoReservas.hpp"
#include "util/colecao.hpp"

// Manutencoes das quadras e o que elas causam nas reservas. E dono dos objetos Manutencao.
// Implementa ConsultaManutencao, que e o que o ServicoReservas usa para bloquear datas.
class ServicoManutencoes : public ConsultaManutencao {
public:
    static const int CUPOM_CANCELAMENTO = 10;  // % de desconto para quem teve reserva cancelada

    ServicoManutencoes(Mensagens& mensagens, const Relogio& relogio, ServicoQuadras& quadras,
                       ServicoReservas& reservas);

    // Devolve o id da manutencao, ou -1 se der erro.
    int registrar(int quadraId, const std::string& dataInicio, const std::string& dataFim,
                  const std::string& motivo);
    bool encerrar(int manutencaoId);
    bool quadraEmManutencao(int quadraId) const;  // ha manutencao aberta que ainda nao terminou

    // ConsultaManutencao: alguma manutencao cobre essa data nessa quadra?
    bool quadraBloqueadaNaData(int quadraId, const std::string& data) const override;

    int quantidade() const;
    Manutencao* obter(int indice) const;  // nullptr se a posicao for invalida

private:
    ServicoManutencoes(const ServicoManutencoes&);
    ServicoManutencoes& operator=(const ServicoManutencoes&);

    Mensagens& mensagens_;
    const Relogio& relogio_;
    ServicoQuadras& quadras_;
    ServicoReservas& reservas_;
    Colecao<Manutencao> manutencoes_;
};

#endif
