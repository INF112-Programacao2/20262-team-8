#ifndef SISTEMA_GESTAO_HPP
#define SISTEMA_GESTAO_HPP

#include "../models/cliente.hpp"
#include "../models/avaliacao.hpp"
#include "../models/quadra.hpp"
#include "../models/reserva.hpp"
#include "enums/Categoria.hpp"
#include "services/servicoAvaliacoes.hpp"
#include "services/servicoClientes.hpp"
#include "services/servicoQuadras.hpp"
#include "services/servicoReservas.hpp"

#include <string>
#include <vector>

class SistemaGestao {
public:
    SistemaGestao();

    bool definirDataHoje(const std::string& data);
    std::string getDataHoje() const;
    bool definirHoraAgora(int hora);
    int getHoraAgora() const;

    int cadastrarCliente(const std::string& nome, const std::string& email);
    int getQtdClientes() const;
    Cliente* getCliente(int indice) const;
    int cadastrarQuadra(const std::string& nome, const std::string& localizacao,
                        const std::vector<TipoQuadra>& tipos, double precoHora,
                        TipoEstrategia estrategia = TipoEstrategia::Padrao,
                        Categoria categoria = Categoria::Simples,
                        TipoPolitica politica = TipoPolitica::Flexivel);
    int getQtdQuadras() const;
    Quadra* getQuadra(int indice) const;

    // Cliente e quadra devem permanecer vivos enquanto a reserva existir.
    Reserva* criarReserva(Cliente& cliente, Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras);
    bool cancelarReserva(Reserva& reserva);
    bool concluirReserva(Reserva& reserva);
    bool registrarFalta(Reserva& reserva);
    int avaliarReserva(int reservaId, int nota, const std::string& comentario);
    std::vector<Reserva*> listarReservasAtivas() const;
    std::vector<Reserva*> listarReservasConcluidas() const;
    double mediaDaQuadra(int quadraId) const;
    int contarAvaliacoesDaQuadra(int quadraId) const;
    bool quadraEmDestaque(int quadraId) const;
    int getQtdAvaliacoes() const;
    const Avaliacao* getAvaliacao(int indice) const;
    bool verificarDisponibilidade(const Quadra& quadra, const std::string& data, int horarioInicio, int duracaoHoras) const;
private:
    ServicoQuadras servicoQuadras;
    ServicoClientes servicoClientes;
    ServicoReservas servicoReservas;
    ServicoAvaliacoes servicoAvaliacoes;
};

#endif
