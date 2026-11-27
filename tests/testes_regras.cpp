#include "models/cliente.hpp"
#include "models/quadra.hpp"
#include "models/sistemagestao.hpp"

#include <cassert>
#include <cmath>
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
    assert(sistema.definirDataHoje("2027-02-27"));

    Reserva* reserva = sistema.criarReserva(cliente, quadra, "2027-02-28", 17, 2);
    assert(reserva->getValorTotal() == 200.0);
    assert(sistema.listarReservasAtivas().size() == 1);
    assert(!sistema.verificarDisponibilidade(quadra, "2027-02-28", 18, 1));
    assert(sistema.verificarDisponibilidade(quadra, "2027-02-28", 19, 1));
    assert(sistema.verificarDisponibilidade(outraQuadra, "2027-02-28", 17, 2));

    sistema.criarReserva(cliente, outraQuadra, "2027-02-28", 17, 2);
    assert(sistema.listarReservasAtivas().size() == 2);
    assert(sistema.cancelarReserva(*reserva));
    assert(reserva->getStatus() == StatusReserva::Cancelada);
    assert(reserva->getValorReembolso() == 200.0);
    assert(sistema.listarReservasAtivas().size() == 1);
    assert(sistema.verificarDisponibilidade(quadra, "2027-02-28", 17, 2));
    assert(!sistema.cancelarReserva(*reserva));
    assert(cliente.getReservas().size() == 2);

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
    assert(sistema.definirDataHoje("2027-02-27"));

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

void testaCategoriasEAdicionaisDePreco() {
    SistemaGestao sistema;
    const std::vector<TipoQuadra> tipos{FUTEBOL};
    assert(sistema.definirDataHoje("2027-02-27"));

    sistema.cadastrarQuadra(
        "Simples", "Centro", tipos, 100.0, TipoEstrategia::Padrao,
        Categoria::Simples);
    sistema.cadastrarQuadra(
        "Coberta", "Centro", tipos, 100.0, TipoEstrategia::Padrao,
        Categoria::Coberta);
    sistema.cadastrarQuadra(
        "Iluminada", "Centro", tipos, 100.0, TipoEstrategia::Padrao,
        Categoria::Iluminada);
    sistema.cadastrarQuadra(
        "Premium", "Centro", tipos, 100.0, TipoEstrategia::Mensalista,
        Categoria::Premium);

    assert(sistema.getQuadra(0)->getCategoria() == "Simples");
    assert(sistema.getQuadra(1)->getCategoria() == "Coberta");
    assert(sistema.getQuadra(2)->getCategoria() == "Iluminada");
    assert(sistema.getQuadra(3)->getCategoria() == "Premium");
    assert(std::abs(sistema.getQuadra(0)->calcularValorReserva(2, 17) - 200.0) < 1e-9);
    assert(std::abs(sistema.getQuadra(1)->calcularValorReserva(2, 17) - 230.0) < 1e-9);
    assert(std::abs(sistema.getQuadra(2)->calcularValorReserva(2, 17) - 210.0) < 1e-9);
    assert(sistema.getQuadra(3)->getNomeEstrategiaPreco() == "Mensalista");
    assert(std::abs(sistema.getQuadra(3)->calcularValorReserva(2, 17) - 205.5) < 1e-9);
}

void testaPoliticasDeReembolso() {
    Cliente cliente("Ana", "ana@example.com");
    SistemaGestao sistema;
    const std::vector<TipoQuadra> tipos{FUTEBOL};
    assert(sistema.definirDataHoje("2024-02-28"));
    assert(sistema.definirHoraAgora(0));

    sistema.cadastrarQuadra("Flexivel", "Centro", tipos, 100.0,
                            TipoEstrategia::Padrao, Categoria::Simples,
                            TipoPolitica::Flexivel);
    sistema.cadastrarQuadra("Moderada", "Centro", tipos, 100.0,
                            TipoEstrategia::Padrao, Categoria::Simples,
                            TipoPolitica::Moderada);
    sistema.cadastrarQuadra("Rigida", "Centro", tipos, 100.0,
                            TipoEstrategia::Padrao, Categoria::Simples,
                            TipoPolitica::Rigida);

    assert(sistema.getQuadra(0)->getPoliticaCancelamento()->getNome() == "Flexível");
    assert(sistema.getQuadra(1)->getPoliticaCancelamento()->getNome() == "Moderada");
    assert(sistema.getQuadra(2)->getPoliticaCancelamento()->getNome() == "Rígida");

    auto cancelarEVerificar = [&](int indiceQuadra, const std::string& data,
                                  int inicio, double reembolsoEsperado) {
        Reserva* reserva = sistema.criarReserva(
            cliente, *sistema.getQuadra(indiceQuadra), data, inicio, 1);
        assert(reserva->getValorReembolso() == 0.0);
        assert(sistema.cancelarReserva(*reserva));
        assert(reserva->getStatus() == StatusReserva::Cancelada);
        assert(std::abs(reserva->getValorReembolso() - reembolsoEsperado) < 1e-9);
    };

    cancelarEVerificar(0, "2024-02-28", 10, 50.0);
    cancelarEVerificar(0, "2024-02-29", 10, 100.0);
    cancelarEVerificar(1, "2024-02-28", 11, 0.0);
    cancelarEVerificar(1, "2024-02-29", 11, 50.0);
    cancelarEVerificar(1, "2024-03-01", 11, 50.0);
    cancelarEVerificar(1, "2024-03-02", 11, 100.0);
    cancelarEVerificar(2, "2024-03-05", 12, 0.0);
    cancelarEVerificar(2, "2024-03-06", 12, 50.0);

    Reserva* reservaPassada = sistema.criarReserva(
        cliente, *sistema.getQuadra(0), "2024-03-07", 13, 1);
    assert(sistema.definirDataHoje("2024-03-08"));
    assert(!sistema.cancelarReserva(*reservaPassada));
    assert(reservaPassada->estaAtiva());
}

void testaReservasPertencemAoServico() {
    const std::vector<TipoQuadra> tipos{FUTEBOL};
    Cliente cliente("Ana", "ana@example.com");
    Quadra quadra("Principal", "Centro", tipos, 100.0);

    {
        SistemaGestao primeiroSistema;
        assert(primeiroSistema.definirDataHoje("2027-02-27"));
        Reserva* reservaPrimeiro = primeiroSistema.criarReserva(
            cliente, quadra, "2027-02-28", 17, 2);

        {
            SistemaGestao segundoSistema;
            assert(segundoSistema.definirDataHoje("2027-02-27"));
            Reserva* reservaSegundo = segundoSistema.criarReserva(
                cliente, quadra, "2027-02-28", 17, 2);

            assert(primeiroSistema.listarReservasAtivas().size() == 1);
            assert(segundoSistema.listarReservasAtivas().size() == 1);
            assert(!primeiroSistema.cancelarReserva(*reservaSegundo));
            assert(segundoSistema.cancelarReserva(*reservaSegundo));
            assert(segundoSistema.listarReservasAtivas().empty());
            assert(primeiroSistema.listarReservasAtivas().size() == 1);
        }

        assert(cliente.getReservas().size() == 1);
        assert(primeiroSistema.listarReservasAtivas().front() == reservaPrimeiro);
    }

    assert(cliente.getReservas().empty());
}

void testaEstadosDeReservaEHorario() {
    const std::vector<TipoQuadra> tipos{FUTEBOL};
    Cliente cliente("Ana", "ana@example.com");
    Quadra quadra("Principal", "Centro", tipos, 100.0);
    SistemaGestao sistema;

    assert(sistema.definirDataHoje("2027-02-27"));
    assert(sistema.definirHoraAgora(10));
    assert(!sistema.definirDataHoje("2027-02-29"));
    assert(!sistema.definirHoraAgora(24));
    assert(sistema.getDataHoje() == "2027-02-27");
    assert(sistema.getHoraAgora() == 10);
    deveRejeitarReservaInvalida(sistema, cliente, quadra, "2027-02-26", 11, 1);

    Reserva* reserva = sistema.criarReserva(
        cliente, quadra, "2027-02-27", 11, 1);
    assert(reserva->getStatus() == StatusReserva::Confirmada);
    assert(reserva->getStatusTexto() == "Confirmada");
    assert(reserva->getHorarioFim() == 12);
    assert(!sistema.concluirReserva(*reserva));
    assert(!sistema.registrarFalta(*reserva));

    assert(sistema.definirHoraAgora(11));
    assert(sistema.concluirReserva(*reserva));
    assert(reserva->getStatus() == StatusReserva::Concluida);
    assert(reserva->getStatusTexto() == "Concluida");
    assert(!reserva->estaAtiva());
    assert(!sistema.concluirReserva(*reserva));
    assert(!sistema.cancelarReserva(*reserva));
}

void testaFaltasBloqueiamCliente() {
    const std::vector<TipoQuadra> tipos{FUTEBOL};
    Cliente cliente("Bruno", "bruno@example.com");
    Quadra quadra("Principal", "Centro", tipos, 100.0);
    SistemaGestao sistema;

    for (int dia = 1; dia <= 3; ++dia) {
        const std::string data = "2027-03-0" + std::to_string(dia);
        assert(sistema.definirDataHoje(data));
        assert(sistema.definirHoraAgora(0));
        Reserva* reserva = sistema.criarReserva(cliente, quadra, data, 10, 1);
        assert(sistema.definirHoraAgora(10));
        assert(sistema.registrarFalta(*reserva));
        assert(reserva->getStatus() == StatusReserva::Falta);
        assert(reserva->getStatusTexto() == "Falta");
        assert(!reserva->estaAtiva());
        assert(cliente.getFaltas() == dia);
    }

    assert(cliente.estaBloqueado());
    assert(sistema.definirDataHoje("2027-03-04"));
    assert(sistema.definirHoraAgora(0));

    bool bloqueado = false;
    try {
        sistema.criarReserva(cliente, quadra, "2027-03-04", 10, 1);
    } catch (const std::runtime_error&) {
        bloqueado = true;
    }
    assert(bloqueado);

    cliente.zerarFaltas();
    assert(!cliente.estaBloqueado());
    assert(cliente.getFaltas() == 0);
}

void testaAvaliacoesEReclamacoes() {
    SistemaGestao sistema;
    assert(sistema.definirDataHoje("2027-01-01"));
    assert(sistema.definirHoraAgora(0));
    const int cliente1Id = sistema.cadastrarCliente("Ana", "ana@example.com");
    const int cliente2Id = sistema.cadastrarCliente("Bruno", "bruno@example.com");
    const int cliente3Id = sistema.cadastrarCliente("Carla", "carla@example.com");
    const std::vector<TipoQuadra> tipos{FUTEBOL};
    const int quadraDestaqueId = sistema.cadastrarQuadra(
        "Destaque", "Centro", tipos, 100.0);
    const int quadraReclamacaoId = sistema.cadastrarQuadra(
        "Em revisao", "Bairro", tipos, 100.0);
    Quadra* quadraDestaque = sistema.getQuadra(0);
    Quadra* quadraReclamacao = sistema.getQuadra(1);
    Cliente* cliente1 = sistema.getCliente(0);
    Cliente* cliente2 = sistema.getCliente(1);
    Cliente* cliente3 = sistema.getCliente(2);
    assert(cliente1->getId() == cliente1Id);
    assert(cliente2->getId() == cliente2Id);
    assert(cliente3->getId() == cliente3Id);
    assert(quadraReclamacao->getId() == quadraReclamacaoId);

    auto concluirReservaDoDia = [&](Cliente& cliente, Quadra& quadra,
                                    const std::string& data, int hora) {
        assert(sistema.definirDataHoje(data));
        assert(sistema.definirHoraAgora(0));
        Reserva* reserva = sistema.criarReserva(cliente, quadra, data, hora, 1);
        assert(sistema.definirHoraAgora(hora));
        assert(sistema.concluirReserva(*reserva));
        return reserva;
    };

    Reserva* pendente = sistema.criarReserva(
        *cliente1, *quadraDestaque, "2027-01-01", 11, 1);
    assert(sistema.avaliarReserva(pendente->getId(), 5, "Ainda nao concluida") == -1);
    assert(sistema.cancelarReserva(*pendente));

    Reserva* primeiroDestaque = concluirReservaDoDia(
        *cliente1, *quadraDestaque, "2027-01-01", 10);
    assert(sistema.avaliarReserva(primeiroDestaque->getId(), 6, "Nota invalida") == -1);
    assert(sistema.avaliarReserva(primeiroDestaque->getId(), 5, "Excelente") > 0);
    assert(sistema.avaliarReserva(primeiroDestaque->getId(), 5, "Duplicada") == -1);

    Reserva* segundoDestaque = concluirReservaDoDia(
        *cliente2, *quadraDestaque, "2027-01-02", 10);
    assert(sistema.avaliarReserva(segundoDestaque->getId(), 4, "Muito bom") > 0);
    Reserva* terceiroDestaque = concluirReservaDoDia(
        *cliente3, *quadraDestaque, "2027-01-03", 10);
    assert(sistema.avaliarReserva(terceiroDestaque->getId(), 5, "Excelente") > 0);

    assert(sistema.getQtdAvaliacoes() == 3);
    assert(sistema.getAvaliacao(0)->getReservaId() == primeiroDestaque->getId());
    assert(sistema.getAvaliacao(-1) == nullptr);
    assert(sistema.getAvaliacao(3) == nullptr);
    assert(sistema.contarAvaliacoesDaQuadra(quadraDestaqueId) == 3);
    assert(std::abs(sistema.mediaDaQuadra(quadraDestaqueId) - (14.0 / 3.0)) < 1e-9);
    assert(sistema.quadraEmDestaque(quadraDestaqueId));

    Reserva* primeiraReclamacao = concluirReservaDoDia(
        *cliente1, *quadraReclamacao, "2027-01-04", 10);
    assert(sistema.avaliarReserva(primeiraReclamacao->getId(), 1, "Ruim") > 0);
    assert(cliente1->getDescontoProximaReserva() == 15);
    assert(quadraReclamacao->getReclamacoes() == 1);

    assert(sistema.definirDataHoje("2027-01-05"));
    assert(sistema.definirHoraAgora(0));
    Reserva* reservaComCupom = sistema.criarReserva(
        *cliente1, *quadraDestaque, "2027-01-05", 12, 1);
    assert(reservaComCupom->getDescontoPercentual() == 15);
    assert(reservaComCupom->getValorTotal() == 85.0);
    assert(cliente1->getDescontoProximaReserva() == 0);

    Reserva* segundaReclamacao = concluirReservaDoDia(
        *cliente2, *quadraReclamacao, "2027-01-05", 10);
    assert(sistema.avaliarReserva(segundaReclamacao->getId(), 2, "Ruim") > 0);
    assert(cliente2->getDescontoProximaReserva() == 15);

    Reserva* terceiraReclamacao = concluirReservaDoDia(
        *cliente3, *quadraReclamacao, "2027-01-06", 10);
    assert(sistema.avaliarReserva(terceiraReclamacao->getId(), 1, "Ruim") > 0);
    assert(quadraReclamacao->getReclamacoes() == 3);
    assert(!quadraReclamacao->isAtiva());

    bool quadraInativaRejeitada = false;
    try {
        sistema.criarReserva(
            *cliente2, *quadraReclamacao, "2027-01-07", 10, 1);
    } catch (const std::runtime_error&) {
        quadraInativaRejeitada = true;
    }
    assert(quadraInativaRejeitada);
}

} // namespace

int main() {
    testaReservasEAvaliaDisponibilidade();
    testaCadastroDeClientesPelaFachada();
    testaCadastroDeQuadrasPelaFachada();
    testaCategoriasEAdicionaisDePreco();
    testaPoliticasDeReembolso();
    testaReservasPertencemAoServico();
    testaEstadosDeReservaEHorario();
    testaFaltasBloqueiamCliente();
    testaAvaliacoesEReclamacoes();
}
