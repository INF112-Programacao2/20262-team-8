#include "modelos/avaliacao.hpp"
#include "util/geradorId.hpp"

Avaliacao::Avaliacao(int reservaId, int clienteId, int quadraId, int nota,
                     const std::string& comentario) {
    id_ = proximoId<Avaliacao>();
    reservaId_ = reservaId;
    clienteId_ = clienteId;
    quadraId_ = quadraId;
    nota_ = nota;
    comentario_ = comentario;
}

int Avaliacao::getId() const {
    return id_;
}

int Avaliacao::getReservaId() const {
    return reservaId_;
}

int Avaliacao::getClienteId() const {
    return clienteId_;
}

int Avaliacao::getQuadraId() const {
    return quadraId_;
}

int Avaliacao::getNota() const {
    return nota_;
}

std::string Avaliacao::getComentario() const {
    return comentario_;
}

bool Avaliacao::ehReclamacao() const {
    return nota_ <= 2;
}
