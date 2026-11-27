#include "include/gui/app.hpp"

#include <iostream>
#include <stdexcept>

int main() {
    try {
        App app;
        return app.executar();
    } catch (const std::runtime_error& erro) {
        std::cerr << "Nao foi possivel iniciar a interface grafica: "
                  << erro.what() << '\n';
        return 1;
    }
}
