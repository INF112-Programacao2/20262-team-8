#ifndef TIPOESTRATEGIA_HPP
#define TIPOESTRATEGIA_HPP

// Estrategias de preco que uma quadra pode usar (padrao Strategy).
// A ordem importa: a interface usa a posicao do botao como numero do enum.
enum class TipoEstrategia {
    Padrao,
    Mensalista,
    Pico
};

#endif
