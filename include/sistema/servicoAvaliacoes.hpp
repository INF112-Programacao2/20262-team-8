#ifndef SERVICO_AVALIACOES_HPP
#define SERVICO_AVALIACOES_HPP

#include <string>
#include "modelos/avaliacao.hpp"
#include "sistema/mensagens.hpp"
#include "sistema/servicoReservas.hpp"
#include "util/colecao.hpp"

// Avaliacoes das quadras e o que elas causam (cupom, reclamacoes, destaque).
// E dono dos objetos Avaliacao.
class ServicoAvaliacoes {
public:
    static const int RECLAMACOES_PARA_REVISAO = 3;
    static const int CUPOM_RECLAMACAO = 15;  // % de desconto para o cliente insatisfeito
    static const int MIN_AVALIACOES_DESTAQUE = 3;
    static constexpr double MEDIA_DESTAQUE = 4.5;

    ServicoAvaliacoes(Mensagens& mensagens, ServicoReservas& reservas);

    // Devolve o id da avaliacao, ou -1 se der erro.
    int avaliar(int reservaId, int nota, const std::string& comentario);
    double mediaDaQuadra(int quadraId) const;  // 0 se nao houver avaliacoes
    int contarDaQuadra(int quadraId) const;
    bool quadraEmDestaque(int quadraId) const;

    int quantidade() const;
    Avaliacao* obter(int indice) const;  // nullptr se a posicao for invalida

private:
    ServicoAvaliacoes(const ServicoAvaliacoes&);
    ServicoAvaliacoes& operator=(const ServicoAvaliacoes&);

    Mensagens& mensagens_;
    ServicoReservas& reservas_;
    Colecao<Avaliacao> avaliacoes_;
};

#endif
