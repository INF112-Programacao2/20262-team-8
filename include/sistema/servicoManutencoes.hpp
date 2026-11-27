#ifndef SERVICO_MANUTENCOES_HPP
#define SERVICO_MANUTENCOES_HPP

#include "models/manutencao.hpp"
#include "services/consultaManutencao.hpp"

#include <memory>
#include <string>
#include <vector>

class ServicoQuadras;
class ServicoReservas;

class ServicoManutencoes : public ConsultaManutencao {
public:
    static constexpr int cupomCancelamento = 10;

    ServicoManutencoes(ServicoQuadras& quadras, ServicoReservas& reservas);

    int registrar(int quadraId, const std::string& dataInicio,
                  const std::string& dataFim, const std::string& motivo);
    bool encerrar(int manutencaoId);
    bool quadraEmManutencao(int quadraId) const;
    bool quadraBloqueadaNaData(int quadraId, const std::string& data) const override;
    int quantidade() const;
    const Manutencao* obter(int indice) const;

private:
    ServicoQuadras& quadras;
    ServicoReservas& reservas;
    std::vector<std::unique_ptr<Manutencao>> manutencoes;
};

#endif
