#ifndef AVALIACAO_HPP
#define AVALIACAO_HPP

#include <string>

// Avaliacao que um cliente faz de uma quadra depois de uma reserva concluida.
// Liga tres coisas: a Reserva, o Cliente e a Quadra.
class Avaliacao {
public:
    // O id e gerado automaticamente.
    Avaliacao(int reservaId, int clienteId, int quadraId, int nota, const std::string& comentario);

    int getId() const;
    int getReservaId() const;
    int getClienteId() const;
    int getQuadraId() const;
    int getNota() const;
    std::string getComentario() const;

    // Nota 1 ou 2 conta como reclamacao (cliente insatisfeito).
    bool ehReclamacao() const;

private:
    int id_;
    int reservaId_;
    int clienteId_;
    int quadraId_;
    int nota_;
    std::string comentario_;
};

#endif
