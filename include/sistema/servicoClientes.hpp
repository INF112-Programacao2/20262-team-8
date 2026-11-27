#ifndef SERVICO_CLIENTES_HPP
#define SERVICO_CLIENTES_HPP

#include "models/cliente.hpp"

#include <memory>
#include <string>
#include <vector>

class ServicoClientes {
public:
    int cadastrar(const std::string& nome, const std::string& email);
    int quantidade() const;
    Cliente* obter(int indice) const;

private:
    std::vector<std::unique_ptr<Cliente>> clientes;
};

#endif
