#ifndef GUI_APP_HPP
#define GUI_APP_HPP

#include "gui/widgets.hpp"
#include "models/sistemagestao.hpp"

#include <array>
#include <string>
#include <vector>

class App {
public:
    App();
    int executar();

private:
    enum class Tela {
        Clientes,
        Quadras,
        NovaReserva,
        Reservas,
        Avaliacoes,
        Manutencao
    };

    void tratarEvento(const sf::Event& evento);
    void desenhar();
    void desenharLateral();
    void desenharTela();
    void desenharStatus();
    void atualizarListas();
    void atualizarRelogio();
    void atualizarFoco(sf::Vector2i posicao);
    void mostrarMensagem(const std::string& mensagem, bool sucesso);

    std::vector<ui::TextField*> camposDaTela();
    std::vector<ui::ListItem> clientesParaLista() const;
    std::vector<ui::ListItem> quadrasParaLista() const;
    std::vector<ui::ListItem> reservasParaLista(bool avaliaveis = false) const;
    std::vector<ui::ListItem> manutencoesParaLista() const;
    std::vector<Reserva*> todasReservas() const;
    Cliente* buscarCliente(int id) const;
    Quadra* buscarQuadra(int id) const;
    Reserva* buscarReserva(int id) const;
    int inteiro(const std::string& texto, int padrao = -1) const;
    bool clicou(const sf::Event& evento, ui::Box box) const;

    void cadastrarCliente();
    void cadastrarQuadra();
    void criarReserva();
    void processarReserva(int acao);
    void avaliarReserva();
    void registrarManutencao();
    void encerrarManutencao();
    void alternarEstadoQuadra();

    sf::RenderWindow janela_;
    sf::Font fonte_;
    SistemaGestao sistema_;
    Tela tela_;
    std::array<ui::TextField, 15> campos_;
    ui::ListView listaA_;
    ui::ListView listaB_;
    std::array<bool, 7> modalidades_;
    int categoria_;
    int politica_;
    int estrategia_;
    int campoFocado_;
    std::string mensagem_;
    bool mensagemSucesso_;
};

#endif
