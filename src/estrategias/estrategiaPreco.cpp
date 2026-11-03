#include "estrategias/estrategiaPreco.hpp"
#include "estrategias/estrategiaMensalista.hpp"
#include "estrategias/estrategiaPadrao.hpp"
#include "estrategias/estrategiaPico.hpp"

EstrategiaPreco* criarEstrategia(TipoEstrategia tipo) {
    if (tipo == TipoEstrategia::Mensalista) {
        return new EstrategiaMensalista();
    }
    if (tipo == TipoEstrategia::Pico) {
        return new EstrategiaPico();
    }
    return new EstrategiaPadrao();
}
