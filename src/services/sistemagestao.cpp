#include "models/sistemagestao.hpp"

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
                                   TipoEstrategia estrategia) {
    return servicoQuadras.cadastrar(nome, localizacao, tipos, precoHora, estrategia);
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

std::vector<Reserva*> SistemaGestao::listarReservasAtivas() const {
    return servicoReservas.listarReservasAtivas();
}

bool SistemaGestao::verificarDisponibilidade(const Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras) const {
    return servicoReservas.verificarDisponibilidade(
        quadra, data, horarioInicio, duracaoHoras);
}
