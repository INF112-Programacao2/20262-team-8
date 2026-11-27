#include "services/servicoManutencoes.hpp"

#include "services/servicoQuadras.hpp"
#include "services/servicoReservas.hpp"

#include <stdexcept>
#include <utility>

ServicoManutencoes::ServicoManutencoes(ServicoQuadras& quadras,
                                       ServicoReservas& reservas)
    : quadras(quadras), reservas(reservas) {}

int ServicoManutencoes::registrar(int quadraId, const std::string& dataInicio,
                                  const std::string& dataFim, const std::string& motivo) {
    if (quadras.buscarPorId(quadraId) == nullptr) {
        return -1;
    }
    if (!ServicoReservas::dataValida(dataInicio) ||
        !ServicoReservas::dataValida(dataFim) || dataFim < dataInicio ||
        motivo.find_first_not_of(" \t\r\n") == std::string::npos) {
        return -1;
    }

    manutencoes.reserve(manutencoes.size() + 1);
    auto manutencao = std::make_unique<Manutencao>(
        quadraId, dataInicio, dataFim, motivo);
    const int id = manutencao->getId();
    manutencoes.push_back(std::move(manutencao));

    reservas.cancelarNoPeriodo(
        quadraId, dataInicio, dataFim, cupomCancelamento);
    return id;
}

bool ServicoManutencoes::encerrar(int manutencaoId) {
    for (const auto& manutencao : manutencoes) {
        if (manutencao->getId() == manutencaoId && manutencao->estaAberta()) {
            manutencao->encerrar();
            return true;
        }
    }
    return false;
}

bool ServicoManutencoes::quadraEmManutencao(int quadraId) const {
    const std::string dataHoje = reservas.getDataHoje();
    for (const auto& manutencao : manutencoes) {
        if (manutencao->getQuadraId() == quadraId && manutencao->estaAberta() &&
            manutencao->getDataFim() >= dataHoje) {
            return true;
        }
    }
    return false;
}

bool ServicoManutencoes::quadraBloqueadaNaData(int quadraId,
                                               const std::string& data) const {
    for (const auto& manutencao : manutencoes) {
        if (manutencao->getQuadraId() == quadraId && manutencao->cobreData(data)) {
            return true;
        }
    }
    return false;
}

int ServicoManutencoes::quantidade() const {
    return static_cast<int>(manutencoes.size());
}

const Manutencao* ServicoManutencoes::obter(int indice) const {
    if (indice < 0 || indice >= quantidade()) {
        return nullptr;
    }
    return manutencoes[static_cast<std::size_t>(indice)].get();
}
