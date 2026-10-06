#include "../models/quadra.hpp"

static int gerarId() {
    static int id = 0;
    return ++id;
}

Quadra::Quadra(std::string& nome, std::string& local, std::vector<TipoQuadra>& tipos, double preco)
    : id(gerarId()), nome(nome), localizacao(local), tipos(tipos), precoHora(preco), ativa(false) {}

double Quadra::calcularValorReserva(int duracao) const {
    return precoHora * duracao;
}

int Quadra::getId() const {
    return id;
}

std::string Quadra::getNome() const {
    return nome;
}

std::string Quadra::getLocalizacao() const {
    return localizacao;
}

std::vector<TipoQuadra> Quadra::getTipos() const {
    return tipos;
}

double Quadra::getPrecoHora() const {
    return precoHora;
}

bool Quadra::isAtiva() const {
    return ativa;
}
