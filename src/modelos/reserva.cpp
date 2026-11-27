#include "modelos/reserva.hpp"
#include "util/geradorId.hpp"

Reserva* Reserva::reservasAtivas_[Reserva::MAX_ATIVAS];
int Reserva::qtdAtivas_ = 0;

// Dois intervalos [ini, fim) se sobrepoem quando cada um comeca antes de o outro terminar.
static bool intervalosSeSobrepoem(int inicioA, int fimA, int inicioB, int fimB) {
    return inicioA < fimB && inicioB < fimA;
}

Reserva::Reserva(Cliente* cliente, Quadra* quadra, const std::string& dataReserva,
                 int horarioInicio, int duracao, int descontoPercentual) {
    id_ = proximoId<Reserva>();
    cliente_ = cliente;
    quadra_ = quadra;
    dataReserva_ = dataReserva;
    horarioInicio_ = horarioInicio;
    duracao_ = duracao;
    desconto_ = descontoPercentual;
    reembolso_ = 0;
    status_ = StatusReserva::Confirmada;
    avaliada_ = false;
    valorTotal_ = 0;

    solicitarCalculoValor();
    adicionarReservaAtiva(this);
    if (cliente_ != nullptr) {
        cliente_->adicionarReserva(this);
    }
}

Reserva::~Reserva() {
    removerReservaAtiva(this);
}

// Polimorfismo: o C++ escolhe a versao da subclasse real da quadra.
void Reserva::solicitarCalculoValor() {
    if (quadra_ != nullptr) {
        double valorSemDesconto = quadra_->calcularValorReserva(duracao_, horarioInicio_);
        valorTotal_ = valorSemDesconto * (100 - desconto_) / 100.0;
    } else {
        valorTotal_ = 0.0;
    }
}

int Reserva::getId() const {
    return id_;
}

Cliente* Reserva::getCliente() const {
    return cliente_;
}

Quadra* Reserva::getQuadra() const {
    return quadra_;
}

std::string Reserva::getDataReserva() const {
    return dataReserva_;
}

int Reserva::getHorarioInicio() const {
    return horarioInicio_;
}

int Reserva::getDuracaoHoras() const {
    return duracao_;
}

int Reserva::getHorarioFim() const {
    return horarioInicio_ + duracao_;
}

double Reserva::getValorTotal() const {
    return valorTotal_;
}

int Reserva::getDesconto() const {
    return desconto_;
}

StatusReserva Reserva::getStatus() const {
    return status_;
}

std::string Reserva::getStatusTexto() const {
    if (status_ == StatusReserva::Concluida) {
        return "Concluída";
    }
    if (status_ == StatusReserva::Cancelada) {
        return "Cancelada";
    }
    if (status_ == StatusReserva::Falta) {
        return "Falta";
    }
    return "Confirmada";
}

bool Reserva::estaAtiva() const {
    return status_ == StatusReserva::Confirmada;
}

void Reserva::concluir() {
    status_ = StatusReserva::Concluida;
    removerReservaAtiva(this);
}

void Reserva::registrarFalta() {
    status_ = StatusReserva::Falta;
    removerReservaAtiva(this);
}

void Reserva::cancelarReserva(double reembolso) {
    status_ = StatusReserva::Cancelada;
    reembolso_ = reembolso;
    removerReservaAtiva(this);
}

double Reserva::getReembolso() const {
    return reembolso_;
}

bool Reserva::foiAvaliada() const {
    return avaliada_;
}

void Reserva::marcarAvaliada() {
    avaliada_ = true;
}

bool Reserva::temConflitoComOutraReserva(const Reserva& outra) const {
    if (dataReserva_ != outra.dataReserva_) {
        return false;
    }
    return intervalosSeSobrepoem(horarioInicio_, getHorarioFim(), outra.horarioInicio_,
                                 outra.getHorarioFim());
}

bool Reserva::conflitaCom(int quadraId, const std::string& data, int inicio, int duracao) const {
    if (!estaAtiva() || quadra_ == nullptr || quadra_->getId() != quadraId ||
        dataReserva_ != data) {
        return false;
    }
    return intervalosSeSobrepoem(horarioInicio_, getHorarioFim(), inicio, inicio + duracao);
}

// ----- lista estatica de reservas ativas ------------------------------------
int Reserva::contarReservasAtivas() {
    return qtdAtivas_;
}

Reserva* Reserva::getReservaAtiva(int indice) {
    if (indice < 0 || indice >= qtdAtivas_) {
        return nullptr;
    }
    return reservasAtivas_[indice];
}

void Reserva::adicionarReservaAtiva(Reserva* reserva) {
    if (qtdAtivas_ < MAX_ATIVAS) {
        reservasAtivas_[qtdAtivas_] = reserva;
        qtdAtivas_++;
    }
}

void Reserva::removerReservaAtiva(Reserva* reserva) {
    for (int i = 0; i < qtdAtivas_; i++) {
        if (reservasAtivas_[i] == reserva) {
            // Puxa as seguintes uma posicao para tras (mantem a ordem).
            for (int j = i; j < qtdAtivas_ - 1; j++) {
                reservasAtivas_[j] = reservasAtivas_[j + 1];
            }
            qtdAtivas_--;
            return;
        }
    }
}
