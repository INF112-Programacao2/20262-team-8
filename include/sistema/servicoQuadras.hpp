#ifndef SERVICO_QUADRAS_HPP
#define SERVICO_QUADRAS_HPP

#include <string>
#include "enums/Categoria.hpp"
#include "enums/TipoEstrategia.hpp"
#include "modelos/quadra.hpp"
#include "sistema/mensagens.hpp"
#include "util/colecao.hpp"

// Cadastro de quadras. E dono dos objetos Quadra.
class ServicoQuadras {
public:
    explicit ServicoQuadras(Mensagens& mensagens);

    // Devolve o id criado, ou -1 se der erro.
    int cadastrar(const std::string& nome, const std::string& localizacao, double precoHora,
                  Categoria categoria, const TipoQuadra tipos[], int qtdTipos,
                  TipoPolitica politica);
    bool alternarAtiva(int quadraId);
    // Troca a estrategia de preco da quadra (vale para as proximas reservas).
    bool definirEstrategia(int quadraId, TipoEstrategia tipo);

    Quadra* achar(int id) const;  // nullptr se nao existir
    int quantidade() const;
    Quadra* obter(int indice) const;  // nullptr se a posicao for invalida

private:
    ServicoQuadras(const ServicoQuadras&);
    ServicoQuadras& operator=(const ServicoQuadras&);

    Mensagens& mensagens_;
    Colecao<Quadra> quadras_;
};

#endif
