#ifndef AVALIACAO_HPP
#define AVALIACAO_HPP

#include <string>

class Avaliacao {
public:
    Avaliacao(int reservaId, int clienteId, int quadraId, int nota,
              const std::string& comentario);

    int getId() const;
    int getReservaId() const;
    int getClienteId() const;
    int getQuadraId() const;
    int getNota() const;
    std::string getComentario() const;
    bool ehReclamacao() const;

private:
    int id;
    int reservaId;
    int clienteId;
    int quadraId;
    int nota;
    std::string comentario;
};

#endif
