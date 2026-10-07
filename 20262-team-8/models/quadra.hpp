#ifndef QUADRA_HPP
#define QUADRA_HPP

#include "../enums/TipoQuadra.hpp"
#include <string>
#include <vector>

class Quadra {
    private:
        int id;
        std::string nome;
        std::string localizacao;
        std::vector<TipoQuadra> tipos;
        double precoHora;
        bool ativa;
    public:
        Quadra(std::string& nome, std::string& localizacao, std::vector<TipoQuadra>& tipos, double precoHora);
        ~Quadra() = default;
        
        int getId() const;
        std::string getNome() const;
        std::string getLocalizacao() const;
        std::vector<TipoQuadra> getTipos() const;
        double getPrecoHora() const;
        bool isAtiva() const;
        
        double calcularValorReserva(int duracao) const;
};

#endif