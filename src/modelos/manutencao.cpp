#include "modelos/manutencao.hpp"
#include "util/geradorId.hpp"

Manutencao::Manutencao(int quadraId, const std::string& dataInicio, const std::string& dataFim,
                       const std::string& motivo) {
    id_ = proximoId<Manutencao>();
    quadraId_ = quadraId;
    dataInicio_ = dataInicio;
    dataFim_ = dataFim;
    motivo_ = motivo;
    aberta_ = true;
}

int Manutencao::getId() const {
    return id_;
}

int Manutencao::getQuadraId() const {
    return quadraId_;
}

std::string Manutencao::getDataInicio() const {
    return dataInicio_;
}

std::string Manutencao::getDataFim() const {
    return dataFim_;
}

std::string Manutencao::getMotivo() const {
    return motivo_;
}

bool Manutencao::estaAberta() const {
    return aberta_;
}

void Manutencao::encerrar() {
    aberta_ = false;
}

bool Manutencao::cobreData(const std::string& data) const {
    return aberta_ && data >= dataInicio_ && data <= dataFim_;
}
