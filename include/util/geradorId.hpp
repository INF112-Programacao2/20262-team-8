#ifndef GERADOR_ID_HPP
#define GERADOR_ID_HPP

// Gera ids sequenciais (1, 2, 3...). Cada classe tem a sua propria contagem:
//
//     id_ = proximoId<Cliente>();
//
// O tipo entre <> so serve para separar os contadores (um por classe);
// ele nao precisa ser criado nem estar completo.
template <typename T>
int proximoId() {
    static int id = 0;
    id++;
    return id;
}

#endif
