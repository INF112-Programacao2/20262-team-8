#include "services/servicoQuadras.hpp"

#include "estrategias/estrategiaPreco.hpp"

#include <utility>

int ServicoQuadras::cadastrar(const std::string& nome, const std::string& localizacao,
                              const std::vector<TipoQuadra>& tipos, double precoHora,
                              TipoEstrategia estrategia) {
    auto quadra = std::make_unique<Quadra>(
        nome, localizacao, tipos, precoHora, criarEstrategia(estrategia));
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
