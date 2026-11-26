#include "services/servicoClientes.hpp"

#include <utility>

int ServicoClientes::cadastrar(const std::string& nome, const std::string& email) {
    auto cliente = std::make_unique<Cliente>(nome, email);
    const int id = cliente->getId();
    clientes.push_back(std::move(cliente));
    return id;
}

int ServicoClientes::quantidade() const {
    return static_cast<int>(clientes.size());
}

Cliente* ServicoClientes::obter(int indice) const {
    if (indice < 0 || indice >= quantidade()) {
        return nullptr;
    }
    return clientes[static_cast<std::size_t>(indice)].get();
}
