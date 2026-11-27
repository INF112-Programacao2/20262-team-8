#include "sistema/servicoAvaliacoes.hpp"

ServicoAvaliacoes::ServicoAvaliacoes(Mensagens& mensagens, ServicoReservas& reservas)
    : mensagens_(mensagens), reservas_(reservas) {}

int ServicoAvaliacoes::avaliar(int reservaId, int nota, const std::string& comentario) {
    mensagens_.limpar();
    Reserva* reserva = reservas_.achar(reservaId);
    if (reserva == nullptr) {
        mensagens_.erro("Selecione uma reserva concluída.");
        return -1;
    }
    if (reserva->getStatus() != StatusReserva::Concluida) {
        mensagens_.erro("Só é possível avaliar reservas concluídas.");
        return -1;
    }
    if (reserva->foiAvaliada()) {
        mensagens_.erro("Essa reserva já foi avaliada.");
        return -1;
    }
    if (nota < 1 || nota > 5) {
        mensagens_.erro("A nota deve ser de 1 a 5.");
        return -1;
    }

    Quadra* quadra = reserva->getQuadra();
    Cliente* cliente = reserva->getCliente();

    Avaliacao* nova = new Avaliacao(reservaId, cliente->getId(), quadra->getId(), nota, comentario);
    avaliacoes_.adicionar(nova);
    reserva->marcarAvaliada();

    // Cliente insatisfeito (a avaliacao diz se e reclamacao): a quadra recebe
    // uma reclamacao e o cliente ganha um cupom de compensacao.
    if (nova->ehReclamacao()) {
        quadra->adicionarReclamacao();
        cliente->receberCupom(CUPOM_RECLAMACAO);
        mensagens_.aviso("Cliente insatisfeito: ganhou cupom de " +
                         std::to_string(CUPOM_RECLAMACAO) + "% na próxima reserva.");

        // Muitas reclamacoes colocam a quadra em revisao (fica inativa).
        if (quadra->getReclamacoes() >= RECLAMACOES_PARA_REVISAO && quadra->isAtiva()) {
            quadra->setAtiva(false);
            mensagens_.acrescentarAviso(" A quadra recebeu " +
                                        std::to_string(RECLAMACOES_PARA_REVISAO) +
                                        " reclamações e entrou em revisão (inativa).");
        }
    }
    return nova->getId();
}

double ServicoAvaliacoes::mediaDaQuadra(int quadraId) const {
    int soma = 0;
    int quantidade = 0;
    for (int i = 0; i < avaliacoes_.tamanho(); i++) {
        if (avaliacoes_.obter(i)->getQuadraId() == quadraId) {
            soma += avaliacoes_.obter(i)->getNota();
            quantidade++;
        }
    }
    if (quantidade == 0) {
        return 0;
    }
    return (double)soma / quantidade;
}

int ServicoAvaliacoes::contarDaQuadra(int quadraId) const {
    int quantidade = 0;
    for (int i = 0; i < avaliacoes_.tamanho(); i++) {
        if (avaliacoes_.obter(i)->getQuadraId() == quadraId) {
            quantidade++;
        }
    }
    return quantidade;
}

// Destaque: media de 4,5 ou mais, com pelo menos 3 avaliacoes.
bool ServicoAvaliacoes::quadraEmDestaque(int quadraId) const {
    return contarDaQuadra(quadraId) >= MIN_AVALIACOES_DESTAQUE &&
           mediaDaQuadra(quadraId) >= MEDIA_DESTAQUE;
}

int ServicoAvaliacoes::quantidade() const {
    return avaliacoes_.tamanho();
}

Avaliacao* ServicoAvaliacoes::obter(int indice) const {
    return avaliacoes_.obter(indice);
}
