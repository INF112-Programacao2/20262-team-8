#ifndef COLECAO_HPP
#define COLECAO_HPP

// Lista de ponteiros que cresce sozinha (sem limite fixo de tamanho).
// Feita so com array e new/delete, sem vector.
//
// A Colecao e DONA dos objetos: no destrutor ela da delete em cada um.
// O tipo T precisa ter o metodo getId() (todas as classes do dominio tem).
template <typename T>
class Colecao {
public:
    Colecao() {
        capacidade_ = 8;
        tamanho_ = 0;
        itens_ = new T*[capacidade_];
    }

    ~Colecao() {
        for (int i = 0; i < tamanho_; i++) {
            delete itens_[i];
        }
        delete[] itens_;
    }

    // Guarda o ponteiro (a colecao passa a ser dona dele).
    void adicionar(T* item) {
        if (tamanho_ == capacidade_) {
            aumentar();
        }
        itens_[tamanho_] = item;
        tamanho_++;
    }

    int tamanho() const {
        return tamanho_;
    }

    // nullptr se a posicao for invalida.
    T* obter(int indice) const {
        if (indice < 0 || indice >= tamanho_) {
            return nullptr;
        }
        return itens_[indice];
    }

    // nullptr se nenhum item tiver esse id.
    T* buscarPorId(int id) const {
        for (int i = 0; i < tamanho_; i++) {
            if (itens_[i]->getId() == id) {
                return itens_[i];
            }
        }
        return nullptr;
    }

private:
    // Nao pode ser copiada (ela e dona dos objetos).
    Colecao(const Colecao&);
    Colecao& operator=(const Colecao&);

    // Dobra a capacidade: cria um array maior, copia os ponteiros e apaga o antigo.
    void aumentar() {
        int novaCapacidade = capacidade_ * 2;
        T** novos = new T*[novaCapacidade];
        for (int i = 0; i < tamanho_; i++) {
            novos[i] = itens_[i];
        }
        delete[] itens_;
        itens_ = novos;
        capacidade_ = novaCapacidade;
    }

    T** itens_;
    int tamanho_;
    int capacidade_;
};

#endif
