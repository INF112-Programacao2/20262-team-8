#ifndef MANUTENCAO_HPP
#define MANUTENCAO_HPP

#include <string>

class Manutencao {
public:
    Manutencao(int quadraId, const std::string& dataInicio,
               const std::string& dataFim, const std::string& motivo);

    int getId() const;
    int getQuadraId() const;
    std::string getDataInicio() const;
    std::string getDataFim() const;
    std::string getMotivo() const;
    bool estaAberta() const;
    void encerrar();
    bool cobreData(const std::string& data) const;

private:
    int id;
    int quadraId;
    std::string dataInicio;
    std::string dataFim;
    std::string motivo;
    bool aberta;
};

#endif
