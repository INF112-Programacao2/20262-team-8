#ifndef MENSAGENS_HPP
#define MENSAGENS_HPP

#include <string>

// Guarda a ultima mensagem de erro e o ultimo aviso do sistema.
// Todos os servicos escrevem aqui, e o SistemaGestao devolve para a interface.
class Mensagens {
public:
    Mensagens();

    void limpar();
    void erro(const std::string& texto);
    void aviso(const std::string& texto);
    void acrescentarAviso(const std::string& texto);

    std::string getErro() const;
    std::string getAviso() const;

private:
    std::string erro_;
    std::string aviso_;
};

#endif
