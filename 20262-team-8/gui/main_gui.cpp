#include <QApplication>
#include <QString>
#include <QStringList>
#include <QStyleFactory>

#include "MainWindow.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setStyle(QStyleFactory::create("Fusion"));
    app.setApplicationName("Reservas de Quadras");

    // "--demo" carrega alguns dados de exemplo
    const bool demo = app.arguments().contains("--demo");

    MainWindow janela(demo);
    janela.show();
    return app.exec();
}
