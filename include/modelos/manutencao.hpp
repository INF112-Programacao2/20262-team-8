#ifndef MANUTENCAO_HPP
#define MANUTENCAO_HPP

#include <string>

// Periodo em que uma quadra esta interditada (piso quebrado, rede rasgada etc.).
// Enquanto estiver aberta, a quadra nao aceita reservas nas datas cobertas.
class Manutencao {
public:
    // O id e gerado automaticamente.
    Manutencao(int quadraId, const std::string& dataInicio, const std::string& dataFim,
               const std::string& motivo);

    int getId() const;
    int getQuadraId() const;
    std::string getDataInicio() const;
    std::string getDataFim() const;
    std::string getMotivo() const;
    bool estaAberta() const;
    void encerrar();

    // As datas no formato AAAA-MM-DD podem ser comparadas como texto.
    bool cobreData(const std::string& data) const;

private:
    int id_;
    int quadraId_;
    std::string dataInicio_;
    std::string dataFim_;
    std::string motivo_;
    bool aberta_;
};

#endif
