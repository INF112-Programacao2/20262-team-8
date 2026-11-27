#include "models/sistemagestao.hpp"

SistemaGestao::SistemaGestao()
    : servicoManutencoes(servicoQuadras, servicoReservas),
      servicoAvaliacoes(servicoReservas) {
    servicoReservas.definirConsultaManutencao(&servicoManutencoes);
}

bool SistemaGestao::definirDataHoje(const std::string& data) {
    return servicoReservas.definirDataHoje(data);
}

std::string SistemaGestao::getDataHoje() const {
    return servicoReservas.getDataHoje();
}

bool SistemaGestao::definirHoraAgora(int hora) {
    return servicoReservas.definirHoraAgora(hora);
}

int SistemaGestao::getHoraAgora() const {
    return servicoReservas.getHoraAgora();
}

int SistemaGestao::cadastrarCliente(const std::string& nome, const std::string& email) {
    return servicoClientes.cadastrar(nome, email);
}

int SistemaGestao::getQtdClientes() const {
    return servicoClientes.quantidade();
}

Cliente* SistemaGestao::getCliente(int indice) const {
    return servicoClientes.obter(indice);
}

int SistemaGestao::cadastrarQuadra(const std::string& nome, const std::string& localizacao,
                                   const std::vector<TipoQuadra>& tipos, double precoHora,
                                   TipoEstrategia estrategia, Categoria categoria,
                                   TipoPolitica politica) {
    return servicoQuadras.cadastrar(
        nome, localizacao, tipos, precoHora, estrategia, categoria, politica);
}

int SistemaGestao::getQtdQuadras() const {
    return servicoQuadras.quantidade();
}

Quadra* SistemaGestao::getQuadra(int indice) const {
    return servicoQuadras.obter(indice);
}

Reserva* SistemaGestao::criarReserva(Cliente& cliente, Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras) {
    return servicoReservas.criarReserva(cliente, quadra, data, horarioInicio, duracaoHoras);
}

bool SistemaGestao::cancelarReserva(Reserva& reserva) {
    return servicoReservas.cancelarReserva(reserva);
}

bool SistemaGestao::concluirReserva(Reserva& reserva) {
    return servicoReservas.concluirReserva(reserva);
}

bool SistemaGestao::registrarFalta(Reserva& reserva) {
    return servicoReservas.registrarFalta(reserva);
}

int SistemaGestao::avaliarReserva(int reservaId, int nota,
                                  const std::string& comentario) {
    return servicoAvaliacoes.avaliar(reservaId, nota, comentario);
}

std::vector<Reserva*> SistemaGestao::listarReservasAtivas() const {
    return servicoReservas.listarReservasAtivas();
}

std::vector<Reserva*> SistemaGestao::listarReservasConcluidas() const {
    return servicoReservas.listarReservasConcluidas();
}

double SistemaGestao::mediaDaQuadra(int quadraId) const {
    return servicoAvaliacoes.mediaDaQuadra(quadraId);
}

int SistemaGestao::contarAvaliacoesDaQuadra(int quadraId) const {
    return servicoAvaliacoes.contarDaQuadra(quadraId);
}

bool SistemaGestao::quadraEmDestaque(int quadraId) const {
    return servicoAvaliacoes.quadraEmDestaque(quadraId);
}

int SistemaGestao::getQtdAvaliacoes() const {
    return servicoAvaliacoes.quantidade();
}

const Avaliacao* SistemaGestao::getAvaliacao(int indice) const {
    return servicoAvaliacoes.obter(indice);
}

int SistemaGestao::registrarManutencao(int quadraId, const std::string& dataInicio,
                                       const std::string& dataFim,
                                       const std::string& motivo) {
    return servicoManutencoes.registrar(quadraId, dataInicio, dataFim, motivo);
}

bool SistemaGestao::encerrarManutencao(int manutencaoId) {
    return servicoManutencoes.encerrar(manutencaoId);
}

bool SistemaGestao::quadraEmManutencao(int quadraId) const {
    return servicoManutencoes.quadraEmManutencao(quadraId);
}

int SistemaGestao::getQtdManutencoes() const {
    return servicoManutencoes.quantidade();
}

const Manutencao* SistemaGestao::getManutencao(int indice) const {
    return servicoManutencoes.obter(indice);
}

bool SistemaGestao::verificarDisponibilidade(const Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras) const {
    return servicoReservas.verificarDisponibilidade(
        quadra, data, horarioInicio, duracaoHoras);
}
