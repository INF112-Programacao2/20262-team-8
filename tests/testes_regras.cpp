#include "models/cliente.hpp"
#include "models/quadra.hpp"
#include "models/sistemagestao.hpp"

#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

void deveRejeitarReservaInvalida(SistemaGestao& sistema, Cliente& cliente, Quadra& quadra,
                                  const std::string& data, int inicio, int duracao) {
    bool rejeitada = false;
    try {
        sistema.criarReserva(cliente, quadra, data, inicio, duracao);
    } catch (const std::invalid_argument&) {
        rejeitada = true;
    }
    assert(rejeitada);
}

void testaReservasEAvaliaDisponibilidade() {
    const std::vector<TipoQuadra> tipos{FUTEBOL};
    Quadra quadra("Principal", "Centro", tipos, 100.0);
    Quadra outraQuadra("Secundaria", "Centro", tipos, 80.0);
    Cliente cliente("Ana", "ana@example.com");
    SistemaGestao sistema;

    Reserva* reserva = sistema.criarReserva(cliente, quadra, "2027-02-28", 17, 2);
    assert(reserva->getValorTotal() == 200.0);
    assert(sistema.listarReservasAtivas().size() == 1);
    assert(!sistema.verificarDisponibilidade(quadra, "2027-02-28", 18, 1));
    assert(sistema.verificarDisponibilidade(quadra, "2027-02-28", 19, 1));
    assert(sistema.verificarDisponibilidade(outraQuadra, "2027-02-28", 17, 2));

    sistema.criarReserva(cliente, outraQuadra, "2027-02-28", 17, 2);
    assert(sistema.listarReservasAtivas().size() == 2);
    assert(sistema.cancelarReserva(*reserva));
    assert(sistema.listarReservasAtivas().size() == 1);
    assert(sistema.verificarDisponibilidade(quadra, "2027-02-28", 17, 2));
    assert(!sistema.cancelarReserva(*reserva));

    deveRejeitarReservaInvalida(sistema, cliente, quadra, "2027-02-29", 17, 1);
    deveRejeitarReservaInvalida(sistema, cliente, quadra, "2027-02-28", 23, 2);
    deveRejeitarReservaInvalida(sistema, cliente, quadra, "2027-02-28", 17, 0);
}

void testaCadastroDeClientesPelaFachada() {
    SistemaGestao sistema;
    const int id = sistema.cadastrarCliente("Ana", "ana@example.com");

    assert(sistema.getQtdClientes() == 1);
    assert(sistema.getCliente(0) != nullptr);
    assert(sistema.getCliente(0)->getId() == id);
    assert(sistema.getCliente(-1) == nullptr);
    assert(sistema.getCliente(1) == nullptr);
}

void testaCadastroDeQuadrasPelaFachada() {
    SistemaGestao sistema;
    const std::vector<TipoQuadra> tipos{FUTEBOL};
    const int clienteId = sistema.cadastrarCliente("Ana", "ana@example.com");
    const int id = sistema.cadastrarQuadra(
        "Principal", "Centro", tipos, 100.0, TipoEstrategia::Pico);

    assert(sistema.getQtdQuadras() == 1);
    assert(sistema.getCliente(0)->getId() == clienteId);
    assert(sistema.getQuadra(0) != nullptr);
    assert(sistema.getQuadra(0)->getId() == id);
    assert(sistema.getQuadra(0)->getNomeEstrategiaPreco() == "Horário de pico");
    assert(sistema.getQuadra(0)->calcularValorReserva(2, 18) == 240.0);
    assert(sistema.getQuadra(-1) == nullptr);
    assert(sistema.getQuadra(1) == nullptr);

    Reserva* reserva = sistema.criarReserva(
        *sistema.getCliente(0), *sistema.getQuadra(0), "2027-02-28", 18, 2);
    assert(reserva->getValorTotal() == 240.0);
}

} // namespace

int main() {
    testaReservasEAvaliaDisponibilidade();
    testaCadastroDeClientesPelaFachada();
    testaCadastroDeQuadrasPelaFachada();
}
