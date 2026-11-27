#include "models/manutencao.hpp"

static int gerarIdManutencao() {
    static int id = 0;
    return ++id;
}

Manutencao::Manutencao(int quadraId, const std::string& dataInicio,
                       const std::string& dataFim, const std::string& motivo)
    : id(gerarIdManutencao()), quadraId(quadraId), dataInicio(dataInicio),
      dataFim(dataFim), motivo(motivo), aberta(true) {}

int Manutencao::getId() const {
    return id;
}

int Manutencao::getQuadraId() const {
    return quadraId;
}

std::string Manutencao::getDataInicio() const {
    return dataInicio;
}

std::string Manutencao::getDataFim() const {
    return dataFim;
}

std::string Manutencao::getMotivo() const {
    return motivo;
}

bool Manutencao::estaAberta() const {
    return aberta;
}

void Manutencao::encerrar() {
    aberta = false;
}

bool Manutencao::cobreData(const std::string& data) const {
    return aberta && data >= dataInicio && data <= dataFim;
}
