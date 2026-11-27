#ifndef CATEGORIA_HPP
#define CATEGORIA_HPP

// Categoria da quadra: cada uma corresponde a uma classe (Quadra, QuadraCoberta...).
// A ordem importa: a interface usa a posicao do botao como numero do enum.
enum class Categoria {
    Simples,
    Coberta,
    Iluminada,
    Premium
};

#endif
