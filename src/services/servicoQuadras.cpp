#include "services/servicoQuadras.hpp"

#include "estrategias/estrategiaPreco.hpp"

#include <stdexcept>
#include <utility>

int ServicoQuadras::cadastrar(const std::string& nome, const std::string& localizacao,
                              const std::vector<TipoQuadra>& tipos, double precoHora,
                              TipoEstrategia estrategia, Categoria categoria,
                              TipoPolitica politica) {
    auto estrategiaPreco = criarEstrategia(estrategia);
    auto politicaCancelamento = criarPolitica(politica);
    std::unique_ptr<Quadra> quadra;
    switch (categoria) {
        case Categoria::Simples:
            quadra = std::make_unique<Quadra>(
                nome, localizacao, tipos, precoHora, std::move(estrategiaPreco),
                std::move(politicaCancelamento));
            break;
        case Categoria::Coberta:
            quadra = std::make_unique<QuadraCoberta>(
                nome, localizacao, tipos, precoHora, std::move(estrategiaPreco),
                std::move(politicaCancelamento));
            break;
        case Categoria::Iluminada:
            quadra = std::make_unique<QuadraIluminada>(
                nome, localizacao, tipos, precoHora, std::move(estrategiaPreco),
                std::move(politicaCancelamento));
            break;
        case Categoria::Premium:
            quadra = std::make_unique<QuadraPremium>(
                nome, localizacao, tipos, precoHora, std::move(estrategiaPreco),
                std::move(politicaCancelamento));
            break;
        default:
            throw std::invalid_argument("Categoria de quadra invalida.");
    }

    const int id = quadra->getId();
    quadras.push_back(std::move(quadra));
    return id;
}

int ServicoQuadras::quantidade() const {
    return static_cast<int>(quadras.size());
}

Quadra* ServicoQuadras::obter(int indice) const {
    if (indice < 0 || indice >= quantidade()) {
        return nullptr;
    }
    return quadras[static_cast<std::size_t>(indice)].get();
}
