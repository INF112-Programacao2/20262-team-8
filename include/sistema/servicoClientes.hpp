#ifndef SERVICO_CLIENTES_HPP
#define SERVICO_CLIENTES_HPP

#include <string>
#include "modelos/cliente.hpp"
#include "sistema/mensagens.hpp"
#include "util/colecao.hpp"

// Cadastro de clientes. E dono dos objetos Cliente.
class ServicoClientes {
public:
    explicit ServicoClientes(Mensagens& mensagens);

    // Devolve o id criado, ou -1 se der erro (a mensagem fica em Mensagens).
    int cadastrar(const std::string& nome, const std::string& email);
    bool desbloquear(int clienteId);

    Cliente* achar(int id) const;  // nullptr se nao existir
    int quantidade() const;
    Cliente* obter(int indice) const;  // nullptr se a posicao for invalida

private:
    ServicoClientes(const ServicoClientes&);
    ServicoClientes& operator=(const ServicoClientes&);

    Mensagens& mensagens_;
    Colecao<Cliente> clientes_;
};

#endif
