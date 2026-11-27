#include "sistema/servicoQuadras.hpp"

ServicoQuadras::ServicoQuadras(Mensagens& mensagens) : mensagens_(mensagens) {}

int ServicoQuadras::cadastrar(const std::string& nome, const std::string& localizacao,
                              double precoHora, Categoria categoria, const TipoQuadra tipos[],
                              int qtdTipos, TipoPolitica politica) {
    mensagens_.limpar();
    if (nome.empty()) {
        mensagens_.erro("O nome da quadra não pode ficar vazio.");
        return -1;
    }
    if (localizacao.empty()) {
        mensagens_.erro("A localização da quadra não pode ficar vazia.");
        return -1;
    }
    if (precoHora <= 0) {
        mensagens_.erro("Digite um preço válido maior que zero.");
        return -1;
    }
    if (qtdTipos < 1) {
        mensagens_.erro("Selecione ao menos uma modalidade; quadra não cadastrada.");
        return -1;
    }

    // Cada categoria e uma classe diferente; todas ficam guardadas como Quadra*.
    Quadra* nova = nullptr;
    if (categoria == Categoria::Simples) {
        nova = new Quadra(nome, localizacao, tipos, qtdTipos, precoHora, politica);
    } else if (categoria == Categoria::Coberta) {
        nova = new QuadraCoberta(nome, localizacao, tipos, qtdTipos, precoHora, politica);
    } else if (categoria == Categoria::Iluminada) {
        nova = new QuadraIluminada(nome, localizacao, tipos, qtdTipos, precoHora, politica);
    } else {
        nova = new QuadraPremium(nome, localizacao, tipos, qtdTipos, precoHora, politica);
    }

    quadras_.adicionar(nova);
    return nova->getId();
}

bool ServicoQuadras::alternarAtiva(int quadraId) {
    mensagens_.limpar();
    Quadra* quadra = achar(quadraId);
    if (quadra == nullptr) {
        mensagens_.erro("Selecione uma quadra.");
        return false;
    }
    quadra->setAtiva(!quadra->isAtiva());
    return true;
}

bool ServicoQuadras::definirEstrategia(int quadraId, TipoEstrategia tipo) {
    mensagens_.limpar();
    Quadra* quadra = achar(quadraId);
    if (quadra == nullptr) {
        mensagens_.erro("Selecione uma quadra.");
        return false;
    }
    // A quadra passa a ser dona da nova estrategia e apaga a anterior.
    quadra->setEstrategiaPreco(criarEstrategia(tipo));
    const EstrategiaPreco* estrategia = quadra->getEstrategia();
    mensagens_.aviso("Estratégia " + estrategia->getNome() + " (" + estrategia->getDescricao() +
                     "): vale para as próximas reservas.");
    return true;
}

Quadra* ServicoQuadras::achar(int id) const {
    return quadras_.buscarPorId(id);
}

int ServicoQuadras::quantidade() const {
    return quadras_.tamanho();
}

Quadra* ServicoQuadras::obter(int indice) const {
    return quadras_.obter(indice);
}
