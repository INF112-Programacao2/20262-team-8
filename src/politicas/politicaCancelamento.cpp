#include "politicas/politicaCancelamento.hpp"
#include "politicas/politicaFlexivel.hpp"
#include "politicas/politicaModerada.hpp"
#include "politicas/politicaRigida.hpp"

PoliticaCancelamento* criarPolitica(TipoPolitica tipo) {
    if (tipo == TipoPolitica::Moderada) {
        return new PoliticaModerada();
    }
    if (tipo == TipoPolitica::Rigida) {
        return new PoliticaRigida();
    }
    return new PoliticaFlexivel();
}
