#include "sistema/servicoManutencoes.hpp"
#include "util/data.hpp"

ServicoManutencoes::ServicoManutencoes(Mensagens& mensagens, const Relogio& relogio,
                                       ServicoQuadras& quadras, ServicoReservas& reservas)
    : mensagens_(mensagens), relogio_(relogio), quadras_(quadras), reservas_(reservas) {}

int ServicoManutencoes::registrar(int quadraId, const std::string& dataInicio,
                                  const std::string& dataFim, const std::string& motivo) {
    mensagens_.limpar();
    if (quadras_.achar(quadraId) == nullptr) {
        mensagens_.erro("Selecione uma quadra.");
        return -1;
    }
    if (!Data::valida(dataInicio) || !Data::valida(dataFim)) {
        mensagens_.erro("Datas inválidas. Use o formato AAAA-MM-DD.");
        return -1;
    }
    if (dataFim < dataInicio) {
        mensagens_.erro("A data final não pode ser antes da data inicial.");
        return -1;
    }
    if (motivo.empty()) {
        mensagens_.erro("Informe o motivo da manutenção.");
        return -1;
    }

    Manutencao* nova = new Manutencao(quadraId, dataInicio, dataFim, motivo);
    manutencoes_.adicionar(nova);

    // A quadra quebrada afeta as reservas ja marcadas nesse periodo:
    // elas sao canceladas com reembolso total e o cliente ganha um cupom.
    int canceladas = reservas_.cancelarNoPeriodo(quadraId, dataInicio, dataFim, CUPOM_CANCELAMENTO);

    if (canceladas > 0) {
        mensagens_.aviso(std::to_string(canceladas) +
                         " reserva(s) cancelada(s) com reembolso total; os clientes ganharam cupom de " +
                         std::to_string(CUPOM_CANCELAMENTO) + "%.");
    } else {
        mensagens_.aviso("Nenhuma reserva foi afetada.");
    }
    return nova->getId();
}

bool ServicoManutencoes::encerrar(int manutencaoId) {
    mensagens_.limpar();
    Manutencao* manutencao = manutencoes_.buscarPorId(manutencaoId);
    if (manutencao == nullptr) {
        mensagens_.erro("Selecione uma manutenção.");
        return false;
    }
    if (!manutencao->estaAberta()) {
        mensagens_.erro("Essa manutenção já está encerrada.");
        return false;
    }
    manutencao->encerrar();
    return true;
}

bool ServicoManutencoes::quadraEmManutencao(int quadraId) const {
    for (int i = 0; i < manutencoes_.tamanho(); i++) {
        Manutencao* m = manutencoes_.obter(i);
        if (m->getQuadraId() == quadraId && m->estaAberta() &&
            m->getDataFim() >= relogio_.getData()) {
            return true;
        }
    }
    return false;
}

bool ServicoManutencoes::quadraBloqueadaNaData(int quadraId, const std::string& data) const {
    for (int i = 0; i < manutencoes_.tamanho(); i++) {
        Manutencao* m = manutencoes_.obter(i);
        if (m->getQuadraId() == quadraId && m->cobreData(data)) {
            return true;
        }
    }
    return false;
}

int ServicoManutencoes::quantidade() const {
    return manutencoes_.tamanho();
}

Manutencao* ServicoManutencoes::obter(int indice) const {
    return manutencoes_.obter(indice);
}
