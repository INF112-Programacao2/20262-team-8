#ifndef QUADRA_HPP
#define QUADRA_HPP

#include <string>

class Quadra {
    protected:
        int id;
        std::string nome;
        std::string localizacao;
        double precoHora;
        bool ativa;
    public:
        Quadra(const std::string& nome, const std::string& localizacao, double precoHora);
        virtual ~Quadra() = default;

        virtual double calcularValorReserva(int duracao) const;
        virtual int getId() const;
};

#endif