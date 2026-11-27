#ifndef SERVICO_QUADRAS_HPP
#define SERVICO_QUADRAS_HPP

#include "enums/Categoria.hpp"
#include "enums/TipoEstrategia.hpp"
#include "models/quadra.hpp"

#include <memory>
#include <string>
#include <vector>

class ServicoQuadras {
public:
    int cadastrar(const std::string& nome, const std::string& localizacao,
                  const std::vector<TipoQuadra>& tipos, double precoHora,
                  TipoEstrategia estrategia = TipoEstrategia::Padrao,
                  Categoria categoria = Categoria::Simples);
    int quantidade() const;
    Quadra* obter(int indice) const;

private:
    std::vector<std::unique_ptr<Quadra>> quadras;
};

#endif
