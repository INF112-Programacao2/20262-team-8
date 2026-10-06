#include "../models/quadra.hpp"

static int gerarId() {
    static int id = 0;
    return ++id;
}

Quadra::Quadra(const std::string& nome, const std::string& local, double preco)
    : id(gerarId()), nome(nome), localizacao(local), precoHora(preco), ativa(false) {}

Quadra::~Quadra() {  }

double Quadra::calcularValorReserva(int duracao) const {
    return precoHora * duracao;
}

int Quadra::getId() const {
    return id;
}