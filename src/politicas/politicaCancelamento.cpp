#include "politicas/politicaCancelamento.hpp"
#include "politicas/politicaFlexivel.hpp"
#include "politicas/politicaModerada.hpp"
#include "politicas/politicaRigida.hpp"

#include <stdexcept>

std::unique_ptr<PoliticaCancelamento> criarPolitica(TipoPolitica tipo) {
    switch (tipo) {
        case TipoPolitica::Flexivel:
            return std::make_unique<PoliticaFlexivel>();
        case TipoPolitica::Moderada:
            return std::make_unique<PoliticaModerada>();
        case TipoPolitica::Rigida:
            return std::make_unique<PoliticaRigida>();
    }
    throw std::invalid_argument("Tipo de politica de cancelamento invalido.");
}
