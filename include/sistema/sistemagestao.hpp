#ifndef SISTEMA_GESTAO_HPP
#define SISTEMA_GESTAO_HPP

#include <string>
#include "enums/Categoria.hpp"
#include "enums/TipoEstrategia.hpp"
#include "modelos/avaliacao.hpp"
#include "modelos/cliente.hpp"
#include "modelos/manutencao.hpp"
#include "modelos/quadra.hpp"
#include "modelos/reserva.hpp"
#include "sistema/mensagens.hpp"
#include "sistema/relogio.hpp"
#include "sistema/servicoAvaliacoes.hpp"
#include "sistema/servicoClientes.hpp"
#include "sistema/servicoManutencoes.hpp"
#include "sistema/servicoQuadras.hpp"
#include "sistema/servicoReservas.hpp"

// Fachada (Facade) do sistema: e o unico ponto de entrada para a interface e para os testes.
// Ela NAO tem regras de negocio: so cria os servicos, liga um no outro e repassa as chamadas.
// As regras ficam em cada servico:
//   ServicoClientes, ServicoQuadras, ServicoReservas, ServicoAvaliacoes, ServicoManutencoes.
// Os servicos sao donos dos objetos (clientes, quadras, reservas...).
//
// Quando algo da errado, as funcoes devolvem -1 (ou false / nullptr) e a mensagem
// de erro fica guardada: use getUltimoErro().
// Quando uma operacao causa efeitos em outras classes (cupom, cancelamentos...),
// a explicacao fica em getUltimoAviso().
class SistemaGestao {
public:
    SistemaGestao();

    // ----- Data e hora de "agora" (o sistema nao le o relogio do computador) -----
    // A hora decide se uma reserva de hoje ja comecou (para concluir ou registrar falta).
    bool definirDataHoje(const std::string& data);
    std::string getDataHoje() const;
    bool definirHoraAgora(int hora);  // 0 a 23
    int getHoraAgora() const;

    // ----- Cadastros (devolvem o id criado, ou -1 se der erro) -----
    int cadastrarCliente(const std::string& nome, const std::string& email);
    int cadastrarQuadra(const std::string& nome, const std::string& localizacao,
                        double precoHora, Categoria categoria, const TipoQuadra tipos[],
                        int qtdTipos, TipoPolitica politica = TipoPolitica::Flexivel);
    bool alternarAtivaQuadra(int quadraId);
    // Troca a estrategia de preco da quadra (vale para as proximas reservas).
    bool definirEstrategiaQuadra(int quadraId, TipoEstrategia tipo);
    bool desbloquearCliente(int clienteId);

    // ----- Reservas -----
    // Sobrecarga: sem duracao, a reserva dura 1 hora (padrao).
    int criarReserva(int clienteId, int quadraId, const std::string& data, int horarioInicio);
    int criarReserva(int clienteId, int quadraId, const std::string& data, int horarioInicio,
                     int duracao);
    // Mesma coisa, recebendo os objetos (eles precisam estar cadastrados neste sistema).
    // Devolve nullptr se der erro.
    Reserva* criarReserva(Cliente& cliente, Quadra& quadra, const std::string& data,
                          int horarioInicio, int duracaoHoras);

    // Devolve o valor reembolsado (ou -1 se deu erro). O reembolso depende
    // da politica de cancelamento da quadra e de quantos dias faltam.
    double cancelarReserva(int reservaId);
    bool cancelarReserva(Reserva& reserva);
    bool concluirReserva(int reservaId);
    bool registrarFalta(int reservaId);

    // Reservas ativas (confirmadas): percorra de 0 ate contarReservasAtivas() - 1.
    int contarReservasAtivas() const;
    const Reserva* getReservaAtiva(int indice) const;
    // Data e horarios validos e sem conflito com outra reserva ativa da quadra.
    bool verificarDisponibilidade(const Quadra& quadra, const std::string& data,
                                  int horarioInicio, int duracaoHoras) const;

    // ----- Avaliacoes -----
    int avaliarReserva(int reservaId, int nota, const std::string& comentario);
    double mediaDaQuadra(int quadraId) const;  // 0 se nao houver avaliacoes
    int contarAvaliacoesDaQuadra(int quadraId) const;
    bool quadraEmDestaque(int quadraId) const;

    // ----- Manutencao -----
    int registrarManutencao(int quadraId, const std::string& dataInicio,
                            const std::string& dataFim, const std::string& motivo);
    bool encerrarManutencao(int manutencaoId);
    bool quadraEmManutencao(int quadraId) const;  // ha manutencao aberta que ainda nao terminou

    // ----- Consultas -----
    int getQtdClientes() const;
    int getQtdQuadras() const;
    int getQtdReservas() const;
    int getQtdAvaliacoes() const;
    int getQtdManutencoes() const;

    // Acesso pela posicao (0, 1, 2...) para percorrer com um for.
    // Devolvem nullptr se a posicao for invalida.
    const Cliente* getCliente(int indice) const;
    const Quadra* getQuadra(int indice) const;
    const Reserva* getReserva(int indice) const;
    const Avaliacao* getAvaliacao(int indice) const;
    const Manutencao* getManutencao(int indice) const;

    // Busca pelo ID. Devolve nullptr se nao existir.
    const Cliente* buscarCliente(int id) const;
    const Quadra* buscarQuadra(int id) const;
    const Reserva* buscarReserva(int id) const;

    std::string getUltimoErro() const;
    std::string getUltimoAviso() const;

    static bool dataValida(const std::string& data);  // formato AAAA-MM-DD
    // Quantos dias de "de" ate "ate" (negativo se "ate" for antes). Datas devem ser validas.
    static int diferencaEmDias(const std::string& de, const std::string& ate);

private:
    // Impede copia do sistema (os servicos sao donos dos objetos criados com new).
    SistemaGestao(const SistemaGestao&);
    SistemaGestao& operator=(const SistemaGestao&);

    // A ordem importa: cada servico usa os que foram declarados antes dele.
    Mensagens mensagens_;
    Relogio relogio_;
    ServicoClientes clientes_;
    ServicoQuadras quadras_;
    ServicoReservas reservas_;
    ServicoManutencoes manutencoes_;
    ServicoAvaliacoes avaliacoes_;
};

#endif
