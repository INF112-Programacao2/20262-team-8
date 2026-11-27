#ifndef SERVICO_RESERVAS_HPP
#define SERVICO_RESERVAS_HPP

#include "models/cliente.hpp"
#include "models/quadra.hpp"
#include "models/reserva.hpp"
#include "services/consultaManutencao.hpp"

#include <memory>
#include <string>
#include <vector>

class ServicoReservas {
public:
    ServicoReservas();

    bool definirDataHoje(const std::string& data);
    std::string getDataHoje() const;
    bool definirHoraAgora(int hora);
    int getHoraAgora() const;
    void definirConsultaManutencao(const ConsultaManutencao* consulta);
    static bool dataValida(const std::string& data);

    Reserva* criarReserva(Cliente& cliente, Quadra& quadra, const std::string& data,
                          int horarioInicio, int duracaoHoras);
    bool cancelarReserva(Reserva& reserva);
    bool concluirReserva(Reserva& reserva);
    bool registrarFalta(Reserva& reserva);
    std::vector<Reserva*> listarReservasAtivas() const;
    std::vector<Reserva*> listarReservasConcluidas() const;
    Reserva* buscarPorId(int id) const;
    int cancelarNoPeriodo(int quadraId, const std::string& dataInicio,
                          const std::string& dataFim, int cupomPercentual);
    bool verificarDisponibilidade(const Quadra& quadra, const std::string& data,
                                  int horarioInicio, int duracaoHoras) const;

private:
    void validarDadosReserva(const std::string& data, int horarioInicio,
                             int duracaoHoras) const;
    bool pertenceAoServico(const Reserva& reserva) const;
    bool jaComecou(const Reserva& reserva) const;
    static bool ehAnoBissexto(int ano);
    static int diaOrdinal(const std::string& data);

    std::string dataHoje;
    int horaAgora;
    const ConsultaManutencao* consultaManutencao;
    std::vector<std::unique_ptr<Reserva>> reservas;
};

#endif
