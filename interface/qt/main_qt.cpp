#include "JanelaPrincipal.hpp"
#include "Sistema.hpp"

#include <QApplication>
#include <QDate>

// Dados de exemplo para testar a interface: ./reservas-gui --demo
static void popularDemo(Sistema& sistema) {
    Cliente* ana = sistema.cadastrarCliente("Ana Souza", "ana@email.com");
    Cliente* bruno = sistema.cadastrarCliente("Bruno Lima", "bruno@email.com");
    sistema.cadastrarCliente("Carla Dias", "carla@email.com");

    Quadra* q1 = sistema.cadastrarQuadra("Quadra 1", "Bloco A", {FUTEBOL}, 120.0);
    Quadra* q2 = sistema.cadastrarQuadra("Quadra 2", "Bloco A", {VOLEI, BASQUETE, HANDEBOL}, 80.0);
    sistema.cadastrarQuadra("Quadra 3", "Bloco B", {TENIS}, 60.0);
    sistema.cadastrarQuadra("Arena de areia", "Bloco B", {BEACH_TENNIS, PADEL}, 70.0);

    const std::string hoje = QDate::currentDate().toString(Qt::ISODate).toStdString();
    sistema.criarReserva(ana, q1, hoje, 18, 2);
    sistema.criarReserva(bruno, q2, hoje, 9, 1);
    sistema.criarReserva(bruno, q1, hoje, 10, 3);
}

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    Sistema sistema;  // declarado antes da janela: é destruído depois dela
    if (app.arguments().contains("--demo")) {
        popularDemo(sistema);
    }

    JanelaPrincipal janela(sistema);
    janela.show();
    return app.exec();
}
