#include "estrategias/estrategiaPreco.hpp"
#include "estrategias/estrategiaMensalista.hpp"
#include "estrategias/estrategiaPadrao.hpp"
#include "estrategias/estrategiaPico.hpp"

#include <stdexcept>

std::unique_ptr<EstrategiaPreco> criarEstrategia(TipoEstrategia tipo) {
    switch (tipo) {
        case TipoEstrategia::Padrao:
            return std::make_unique<EstrategiaPadrao>();
        case TipoEstrategia::Mensalista:
            return std::make_unique<EstrategiaMensalista>();
        case TipoEstrategia::Pico:
            return std::make_unique<EstrategiaPico>();
    }
    throw std::invalid_argument("Tipo de estrategia de preco invalido.");
}
