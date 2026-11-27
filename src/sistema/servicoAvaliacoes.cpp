#include "services/servicoAvaliacoes.hpp"

#include "models/cliente.hpp"
#include "models/quadra.hpp"
#include "models/reserva.hpp"
#include "services/servicoReservas.hpp"

#include <utility>

ServicoAvaliacoes::ServicoAvaliacoes(ServicoReservas& reservas)
    : reservas(reservas) {}

int ServicoAvaliacoes::avaliar(int reservaId, int nota, const std::string& comentario) {
    Reserva* reserva = reservas.buscarPorId(reservaId);
    if (reserva == nullptr || reserva->getStatus() != StatusReserva::Concluida ||
        reserva->foiAvaliada() || nota < 1 || nota > 5) {
        return -1;
    }

    auto avaliacao = std::make_unique<Avaliacao>(
        reservaId, reserva->getCliente()->getId(), reserva->getQuadra()->getId(),
        nota, comentario);
    const int id = avaliacao->getId();
    avaliacoes.reserve(avaliacoes.size() + 1);
    avaliacoes.push_back(std::move(avaliacao));
    reserva->marcarAvaliada();

    if (avaliacoes.back()->ehReclamacao()) {
        Quadra* quadra = reserva->getQuadra();
        Cliente* cliente = reserva->getCliente();
        quadra->adicionarReclamacao();
        cliente->receberCupom(cupomReclamacao);
        if (quadra->getReclamacoes() >= reclamacoesParaRevisao) {
            quadra->setAtiva(false);
        }
    }
    return id;
}

int ServicoAvaliacoes::quantidade() const {
    return static_cast<int>(avaliacoes.size());
}

const Avaliacao* ServicoAvaliacoes::obter(int indice) const {
    if (indice < 0 || indice >= quantidade()) {
        return nullptr;
    }
    return avaliacoes[static_cast<std::size_t>(indice)].get();
}

double ServicoAvaliacoes::mediaDaQuadra(int quadraId) const {
    int soma = 0;
    int quantidadeQuadra = 0;
    for (const auto& avaliacao : avaliacoes) {
        if (avaliacao->getQuadraId() == quadraId) {
            soma += avaliacao->getNota();
            ++quantidadeQuadra;
        }
    }
    if (quantidadeQuadra == 0) {
        return 0.0;
    }
    return static_cast<double>(soma) / quantidadeQuadra;
}

int ServicoAvaliacoes::contarDaQuadra(int quadraId) const {
    int quantidadeQuadra = 0;
    for (const auto& avaliacao : avaliacoes) {
        if (avaliacao->getQuadraId() == quadraId) {
            ++quantidadeQuadra;
        }
    }
    return quantidadeQuadra;
}

bool ServicoAvaliacoes::quadraEmDestaque(int quadraId) const {
    return contarDaQuadra(quadraId) >= minimoAvaliacoesDestaque &&
           mediaDaQuadra(quadraId) >= mediaDestaque;
}
