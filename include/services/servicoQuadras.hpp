#ifndef SERVICO_QUADRAS_HPP
#define SERVICO_QUADRAS_HPP

#include "enums/Categoria.hpp"
#include "enums/TipoEstrategia.hpp"
#include "enums/TipoPolitica.hpp"
#include "models/quadra.hpp"

#include <memory>
#include <string>
#include <vector>

class ServicoQuadras {
public:
    int cadastrar(const std::string& nome, const std::string& localizacao,
                  const std::vector<TipoQuadra>& tipos, double precoHora,
                  TipoEstrategia estrategia = TipoEstrategia::Padrao,
                  Categoria categoria = Categoria::Simples,
                  TipoPolitica politica = TipoPolitica::Flexivel);
    int quantidade() const;
    Quadra* obter(int indice) const;

private:
    std::vector<std::unique_ptr<Quadra>> quadras;
};

#endif
