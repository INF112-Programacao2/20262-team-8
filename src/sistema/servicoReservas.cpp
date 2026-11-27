#include "sistema/servicoReservas.hpp"
#include "util/data.hpp"

ServicoReservas::ServicoReservas(Mensagens& mensagens, const Relogio& relogio,
                                 ServicoClientes& clientes, ServicoQuadras& quadras)
    : mensagens_(mensagens),
      relogio_(relogio),
      clientes_(clientes),
      quadras_(quadras),
      manutencao_(nullptr) {}

void ServicoReservas::definirConsultaManutencao(const ConsultaManutencao* consulta) {
    manutencao_ = consulta;
}

// Uma reserva so "aconteceu" se o dia dela ja chegou e, sendo hoje, se a hora de inicio
// ja passou. So nesse caso da para concluir ou registrar falta.
bool ServicoReservas::jaComecou(const Reserva& reserva) const {
    if (reserva.getDataReserva() != relogio_.getData()) {
        return reserva.getDataReserva() < relogio_.getData();
    }
    return reserva.getHorarioInicio() <= relogio_.getHora();
}

std::string ServicoReservas::motivoNaoComecou(const Reserva& reserva) const {
    if (reserva.getDataReserva() > relogio_.getData()) {
        return "Essa reserva ainda não aconteceu (hoje é " + relogio_.getData() + ").";
    }
    return "Essa reserva só começa às " + std::to_string(reserva.getHorarioInicio()) +
           "h (agora são " + std::to_string(relogio_.getHora()) + "h).";
}

int ServicoReservas::criar(int clienteId, int quadraId, const std::string& data,
                           int horarioInicio) {
    return criar(clienteId, quadraId, data, horarioInicio, 1);
}

int ServicoReservas::criar(int clienteId, int quadraId, const std::string& data,
                           int horarioInicio, int duracao) {
    mensagens_.limpar();
    Cliente* cliente = clientes_.achar(clienteId);
    if (cliente == nullptr) {
        mensagens_.erro("Selecione um cliente.");
        return -1;
    }
    if (cliente->estaBloqueado()) {
        mensagens_.erro("Cliente bloqueado por " + std::to_string(Cliente::LIMITE_FALTAS) +
                        " faltas sem aviso.");
        return -1;
    }
    Quadra* quadra = quadras_.achar(quadraId);
    if (quadra == nullptr) {
        mensagens_.erro("Selecione uma quadra.");
        return -1;
    }
    if (!Data::valida(data)) {
        mensagens_.erro("Data inválida. Use o formato AAAA-MM-DD.");
        return -1;
    }
    if (data < relogio_.getData()) {
        mensagens_.erro("Essa data já passou (hoje é " + relogio_.getData() + ").");
        return -1;
    }
    if (horarioInicio < 0 || horarioInicio > 23) {
        mensagens_.erro("Horário de início inválido (0 a 23).");
        return -1;
    }
    if (duracao < 1 || horarioInicio + duracao > 24) {
        mensagens_.erro("Duração inválida para o horário informado.");
        return -1;
    }
    if (!quadra->isAtiva()) {
        mensagens_.erro("A quadra está inativa.");
        return -1;
    }
    if (manutencao_ != nullptr && manutencao_->quadraBloqueadaNaData(quadraId, data)) {
        mensagens_.erro("A quadra está em manutenção nessa data.");
        return -1;
    }
    if (!verificarDisponibilidade(*quadra, data, horarioInicio, duracao)) {
        mensagens_.erro("A quadra já possui uma reserva nesse horário.");
        return -1;
    }
    if (reservas_.tamanho() >= MAX_RESERVAS) {
        mensagens_.erro("Limite de reservas atingido.");
        return -1;
    }

    // O cupom do cliente (se tiver) e usado nesta reserva.
    int desconto = cliente->getDescontoProximaReserva();
    Reserva* nova = new Reserva(cliente, quadra, data, horarioInicio, duracao, desconto);
    reservas_.adicionar(nova);

    if (desconto > 0) {
        cliente->setDescontoProximaReserva(0);
        mensagens_.aviso("Cupom de " + std::to_string(desconto) + "% aplicado.");
    }
    return nova->getId();
}

Reserva* ServicoReservas::criar(Cliente& cliente, Quadra& quadra, const std::string& data,
                                int horarioInicio, int duracaoHoras) {
    if (clientes_.achar(cliente.getId()) != &cliente ||
        quadras_.achar(quadra.getId()) != &quadra) {
        mensagens_.limpar();
        mensagens_.erro("Cliente ou quadra não cadastrados neste sistema.");
        return nullptr;
    }
    int id = criar(cliente.getId(), quadra.getId(), data, horarioInicio, duracaoHoras);
    if (id == -1) {
        return nullptr;
    }
    return achar(id);
}

double ServicoReservas::cancelar(int reservaId) {
    mensagens_.limpar();
    Reserva* reserva = achar(reservaId);
    if (reserva == nullptr) {
        mensagens_.erro("Selecione uma reserva.");
        return -1;
    }
    if (!reserva->estaAtiva()) {
        mensagens_.erro("Só é possível cancelar reservas confirmadas.");
        return -1;
    }
    if (reserva->getDataReserva() < relogio_.getData()) {
        mensagens_.erro("Essa reserva já passou: conclua ou registre a falta.");
        return -1;
    }

    // A politica da quadra decide quantos % sao devolvidos (polimorfismo).
    int dias = Data::diferencaEmDias(relogio_.getData(), reserva->getDataReserva());
    const PoliticaCancelamento* politica = reserva->getQuadra()->getPolitica();
    int percentual = politica->percentualReembolso(dias);
    double reembolso = reserva->getValorTotal() * percentual / 100.0;

    reserva->cancelarReserva(reembolso);
    mensagens_.aviso("Política " + politica->getNome() + ": " + std::to_string(percentual) +
                     "% de reembolso (faltam " + std::to_string(dias) + " dia(s)).");
    return reembolso;
}

bool ServicoReservas::cancelar(Reserva& reserva) {
    if (achar(reserva.getId()) != &reserva) {
        mensagens_.limpar();
        mensagens_.erro("Reserva não cadastrada neste sistema.");
        return false;
    }
    return cancelar(reserva.getId()) >= 0;
}

bool ServicoReservas::concluir(int reservaId) {
    mensagens_.limpar();
    Reserva* reserva = achar(reservaId);
    if (reserva == nullptr) {
        mensagens_.erro("Selecione uma reserva.");
        return false;
    }
    if (!reserva->estaAtiva()) {
        mensagens_.erro("Só é possível concluir reservas confirmadas.");
        return false;
    }
    if (!jaComecou(*reserva)) {
        mensagens_.erro(motivoNaoComecou(*reserva));
        return false;
    }
    reserva->concluir();
    mensagens_.aviso("O cliente já pode avaliar a quadra.");
    return true;
}

bool ServicoReservas::registrarFalta(int reservaId) {
    mensagens_.limpar();
    Reserva* reserva = achar(reservaId);
    if (reserva == nullptr) {
        mensagens_.erro("Selecione uma reserva.");
        return false;
    }
    if (!reserva->estaAtiva()) {
        mensagens_.erro("Só é possível registrar falta em reservas confirmadas.");
        return false;
    }
    if (!jaComecou(*reserva)) {
        mensagens_.erro(motivoNaoComecou(*reserva));
        return false;
    }

    // A falta afeta a reserva E o cliente.
    reserva->registrarFalta();
    Cliente* cliente = reserva->getCliente();
    cliente->registrarFalta();
    mensagens_.aviso(cliente->getNome() + " tem " + std::to_string(cliente->getFaltas()) +
                     " falta(s) sem aviso.");
    if (cliente->estaBloqueado()) {
        mensagens_.acrescentarAviso(" Cliente bloqueado!");
    }
    return true;
}

int ServicoReservas::cancelarNoPeriodo(int quadraId, const std::string& dataInicio,
                                       const std::string& dataFim, int cupomPercentual) {
    int canceladas = 0;
    for (int i = 0; i < reservas_.tamanho(); i++) {
        Reserva* r = reservas_.obter(i);
        if (r->estaAtiva() && r->getQuadra()->getId() == quadraId &&
            r->getDataReserva() >= dataInicio && r->getDataReserva() <= dataFim) {
            r->cancelarReserva(r->getValorTotal());
            r->getCliente()->receberCupom(cupomPercentual);
            canceladas++;
        }
    }
    return canceladas;
}

bool ServicoReservas::verificarDisponibilidade(const Quadra& quadra, const std::string& data,
                                               int horarioInicio, int duracaoHoras) const {
    if (!Data::valida(data) || horarioInicio < 0 || horarioInicio > 23 || duracaoHoras < 1 ||
        horarioInicio + duracaoHoras > 24) {
        return false;
    }
    for (int i = 0; i < reservas_.tamanho(); i++) {
        if (reservas_.obter(i)->conflitaCom(quadra.getId(), data, horarioInicio, duracaoHoras)) {
            return false;
        }
    }
    return true;
}

Reserva* ServicoReservas::achar(int id) const {
    return reservas_.buscarPorId(id);
}

int ServicoReservas::quantidade() const {
    return reservas_.tamanho();
}

Reserva* ServicoReservas::obter(int indice) const {
    return reservas_.obter(indice);
}
