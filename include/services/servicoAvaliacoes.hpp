#ifndef SERVICO_AVALIACOES_HPP
#define SERVICO_AVALIACOES_HPP

#include "models/avaliacao.hpp"

#include <memory>
#include <string>
#include <vector>

class ServicoReservas;

class ServicoAvaliacoes {
public:
    explicit ServicoAvaliacoes(ServicoReservas& reservas);

    int avaliar(int reservaId, int nota, const std::string& comentario);
    int quantidade() const;
    const Avaliacao* obter(int indice) const;
    double mediaDaQuadra(int quadraId) const;
    int contarDaQuadra(int quadraId) const;
    bool quadraEmDestaque(int quadraId) const;

private:
    static constexpr int cupomReclamacao = 15;
    static constexpr int reclamacoesParaRevisao = 3;
    static constexpr int minimoAvaliacoesDestaque = 3;
    static constexpr double mediaDestaque = 4.5;

    ServicoReservas& reservas;
    std::vector<std::unique_ptr<Avaliacao>> avaliacoes;
};

#endif
