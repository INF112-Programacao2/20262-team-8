#include "../models/quadra.hpp"
#include "strategies/estrategiaPadrao.hpp"

static int gerarId() {
    static int id = 0;
    return ++id;
}

Quadra::Quadra(const std::string& nome, const std::string& local, 
               const std::vector<TipoQuadra>& tipos, double preco,
               EstrategiaPreco* estrategia)
    : id(gerarId()), nome(nome), localizacao(local), tipos(tipos), 
      precoHora(preco), ativa(true), estrategiaPreco(estrategia) {
    
    // Se nao passou estrategia, cria a padrao com new
    if (this->estrategiaPreco == nullptr) {
        this->estrategiaPreco = new EstrategiaPadrao();
    }
}

Quadra::~Quadra() {
    delete estrategiaPreco; // Libera a memoria manualmente
}

void Quadra::setEstrategiaPreco(EstrategiaPreco* novaEstrategia) {
    if (novaEstrategia != nullptr) {
        delete estrategiaPreco; // Deleta a anterior antes de trocar
        estrategiaPreco = novaEstrategia;
    }
}

double Quadra::calcularValorReserva(int duracao, int horarioInicio) const {
    if (estrategiaPreco != nullptr) {
        return estrategiaPreco->calcularPreco(precoHora, duracao, horarioInicio);
    }
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
