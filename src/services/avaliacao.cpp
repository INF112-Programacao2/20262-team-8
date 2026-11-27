#include "models/avaliacao.hpp"

static int gerarIdAvaliacao() {
    static int id = 0;
    return ++id;
}

Avaliacao::Avaliacao(int reservaId, int clienteId, int quadraId, int nota,
                     const std::string& comentario)
    : id(gerarIdAvaliacao()), reservaId(reservaId), clienteId(clienteId),
      quadraId(quadraId), nota(nota), comentario(comentario) {}

int Avaliacao::getId() const {
    return id;
}

int Avaliacao::getReservaId() const {
    return reservaId;
}

int Avaliacao::getClienteId() const {
    return clienteId;
}

int Avaliacao::getQuadraId() const {
    return quadraId;
}

int Avaliacao::getNota() const {
    return nota;
}

std::string Avaliacao::getComentario() const {
    return comentario;
}

bool Avaliacao::ehReclamacao() const {
    return nota <= 2;
}
