#include "sistema/servicoClientes.hpp"

ServicoClientes::ServicoClientes(Mensagens& mensagens) : mensagens_(mensagens) {}

int ServicoClientes::cadastrar(const std::string& nome, const std::string& email) {
    mensagens_.limpar();
    if (nome.empty()) {
        mensagens_.erro("O nome do cliente não pode ficar vazio.");
        return -1;
    }
    if (email.empty()) {
        mensagens_.erro("O e-mail do cliente não pode ficar vazio.");
        return -1;
    }
    if (email.find('@') == std::string::npos) {
        mensagens_.erro("Informe um e-mail válido (precisa conter @).");
        return -1;
    }

    Cliente* novo = new Cliente(nome, email);
    clientes_.adicionar(novo);
    return novo->getId();
}

bool ServicoClientes::desbloquear(int clienteId) {
    mensagens_.limpar();
    Cliente* cliente = achar(clienteId);
    if (cliente == nullptr) {
        mensagens_.erro("Selecione um cliente.");
        return false;
    }
    cliente->zerarFaltas();
    return true;
}

Cliente* ServicoClientes::achar(int id) const {
    return clientes_.buscarPorId(id);
}

int ServicoClientes::quantidade() const {
    return clientes_.tamanho();
}

Cliente* ServicoClientes::obter(int indice) const {
    return clientes_.obter(indice);
}
