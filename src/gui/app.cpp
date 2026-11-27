#include "gui/app.hpp"

#include "services/servicoReservas.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace {

constexpr unsigned larguraJanela = 1200;
constexpr unsigned alturaJanela = 760;
constexpr float larguraLateral = 215.f;
constexpr float alturaLinha = 31.f;

const char* nomesTelas[] = {
    "Clientes", "Quadras", "Nova reserva", "Reservas", "Avaliacoes", "Manutencao"
};
const char* nomesCategorias[] = {"Simples", "Coberta", "Iluminada", "Premium"};
const char* nomesPoliticas[] = {"Flexivel", "Moderada", "Rigida"};
const char* nomesEstrategias[] = {"Padrao", "Mensalista (-15%)", "Horario de pico (+20%)"};
const char* nomesModalidades[] = {
    "Futebol", "Basquete", "Volei", "Tenis", "Beach tennis", "Padel", "Handebol"
};

std::string dinheiro(double valor) {
    std::ostringstream texto;
    texto << "R$ " << std::fixed << std::setprecision(2) << valor;
    return texto.str();
}

std::string horaTexto(int hora) {
    std::ostringstream texto;
    texto << std::setfill('0') << std::setw(2) << hora << ":00";
    return texto.str();
}

ui::Box menuBox(int indice) {
    return {15.f, 106.f + static_cast<float>(indice) * 51.f, 185.f, 42.f};
}

bool parsePreco(std::string texto, double& preco) {
    std::replace(texto.begin(), texto.end(), ',', '.');
    std::istringstream entrada(texto);
    char extra = '\0';
    if (!(entrada >> preco) || (entrada >> extra) || !std::isfinite(preco) || preco <= 0) {
        return false;
    }
    return true;
}

}  // namespace

App::App()
    : janela_(sf::VideoMode({larguraJanela, alturaJanela}),
              "Gerenciador de Reservas", sf::Style::Titlebar | sf::Style::Close),
      tela_(Tela::Clientes),
      campos_{{
          ui::TextField({15, 528, 185, 34}, "AAAA-MM-DD", 10),
          ui::TextField({15, 594, 78, 34}, "0-23", 2),
          ui::TextField({250, 130, 300, 38}, "Nome completo", 80),
          ui::TextField({250, 205, 300, 38}, "email@exemplo.com", 100),
          ui::TextField({250, 105, 320, 36}, "Nome da quadra", 60),
          ui::TextField({250, 160, 320, 36}, "Localizacao", 60),
          ui::TextField({250, 215, 130, 36}, "Preco por hora", 12),
          ui::TextField({250, 395, 180, 36}, "AAAA-MM-DD", 10),
          ui::TextField({450, 395, 90, 36}, "Inicio", 2),
          ui::TextField({560, 395, 90, 36}, "Duracao", 2),
          ui::TextField({250, 410, 80, 36}, "1-5", 1),
          ui::TextField({350, 410, 420, 36}, "Comentario (opcional)", 100),
          ui::TextField({250, 370, 180, 36}, "Data inicial", 10),
          ui::TextField({450, 370, 180, 36}, "Data final", 10),
          ui::TextField({650, 370, 430, 36}, "Motivo da manutencao", 100)
      }},
      listaA_({600, 100, 480, 500}, "Nenhum item cadastrado."),
      listaB_({660, 100, 420, 220}, "Nenhum item cadastrado."),
      modalidades_{false, false, false, false, false, false, false},
      categoria_(0), politica_(0), estrategia_(0), campoFocado_(-1),
      mensagem_("Cadastre clientes e quadras para comecar."), mensagemSucesso_(true) {
    janela_.setFramerateLimit(60);

    const std::string fontes[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
        "C:/Windows/Fonts/segoeui.ttf",
        "assets/fonte.ttf"
    };
    bool carregouFonte = false;
    for (const std::string& caminho : fontes) {
        if (fonte_.openFromFile(caminho)) {
            carregouFonte = true;
            break;
        }
    }
    if (!carregouFonte) {
        throw std::runtime_error(
            "Nenhuma fonte encontrada; instale uma fonte DejaVu/Liberation ou use assets/fonte.ttf.");
    }

    campos_[0].setValue(sistema_.getDataHoje());
    campos_[1].setValue(std::to_string(sistema_.getHoraAgora()));
    atualizarListas();
}

int App::executar() {
    while (janela_.isOpen()) {
        while (const std::optional<sf::Event> evento = janela_.pollEvent()) {
            if (evento->is<sf::Event::Closed>()) {
                janela_.close();
            } else {
                tratarEvento(*evento);
            }
        }
        atualizarRelogio();
        desenhar();
    }
    return 0;
}

void App::atualizarFoco(sf::Vector2i posicao) {
    campoFocado_ = -1;
    if (campos_[0].contains(posicao)) {
        campoFocado_ = 0;
    } else if (campos_[1].contains(posicao)) {
        campoFocado_ = 1;
    } else {
        const auto camposVisiveis = camposDaTela();
        for (ui::TextField* campo : camposVisiveis) {
            if (campo->contains(posicao)) {
                campoFocado_ = static_cast<int>(campo - &campos_[0]);
                break;
            }
        }
    }
    for (std::size_t indice = 0; indice < campos_.size(); ++indice) {
        campos_[indice].setFocused(static_cast<int>(indice) == campoFocado_);
    }
}

void App::tratarEvento(const sf::Event& evento) {
    if (const auto* digitado = evento.getIf<sf::Event::TextEntered>()) {
        if (campoFocado_ >= 0 && digitado->unicode >= 32 && digitado->unicode != 127) {
            campos_[static_cast<std::size_t>(campoFocado_)].append(digitado->unicode);
            atualizarRelogio();
        }
        return;
    }
    if (const auto* tecla = evento.getIf<sf::Event::KeyPressed>()) {
        if (campoFocado_ >= 0 && tecla->code == sf::Keyboard::Key::Backspace) {
            campos_[static_cast<std::size_t>(campoFocado_)].backspace();
            atualizarRelogio();
        } else if (tecla->code == sf::Keyboard::Key::Tab) {
            std::vector<int> disponiveis{0, 1};
            for (ui::TextField* campo : camposDaTela()) {
                disponiveis.push_back(static_cast<int>(campo - &campos_[0]));
            }
            if (!disponiveis.empty()) {
                auto atual = std::find(disponiveis.begin(), disponiveis.end(), campoFocado_);
                const std::size_t proximo = atual == disponiveis.end()
                                                ? 0
                                                : (static_cast<std::size_t>(
                                                       std::distance(disponiveis.begin(), atual)) + 1) %
                                                      disponiveis.size();
                atualizarFoco({-1, -1});
                campoFocado_ = disponiveis[proximo];
                campos_[static_cast<std::size_t>(campoFocado_)].setFocused(true);
            }
        }
        return;
    }

    const auto* clique = evento.getIf<sf::Event::MouseButtonPressed>();
    if (clique == nullptr || clique->button != sf::Mouse::Button::Left) {
        listaA_.handle(evento);
        listaB_.handle(evento);
        return;
    }

    const sf::Vector2i posicao = clique->position;
    for (int indice = 0; indice < 6; ++indice) {
        if (menuBox(indice).contains(posicao)) {
            tela_ = static_cast<Tela>(indice);
            atualizarFoco({-1, -1});
            atualizarListas();
            return;
        }
    }

    atualizarFoco(posicao);
    listaA_.handle(evento);
    listaB_.handle(evento);

    if (tela_ == Tela::Clientes) {
        if (clicou(evento, {250, 270, 300, 42})) {
            cadastrarCliente();
        } else if (clicou(evento, {250, 330, 300, 42})) {
            Cliente* cliente = buscarCliente(listaA_.selectedId());
            if (cliente == nullptr) {
                mostrarMensagem("Selecione um cliente.", false);
            } else {
                cliente->zerarFaltas();
                atualizarListas();
                mostrarMensagem("Faltas do cliente removidas.", true);
            }
        }
    } else if (tela_ == Tela::Quadras) {
        if (clicou(evento, {250, 590, 340, 42})) {
            cadastrarQuadra();
        } else if (clicou(evento, {640, 555, 440, 42})) {
            alternarEstadoQuadra();
        } else {
            for (int indice = 0; indice < 4; ++indice) {
                const float x = 250.f + static_cast<float>(indice % 2) * 190.f;
                const float y = 270.f + static_cast<float>(indice / 2) * 32.f;
                if (ui::Box{x, y, 185, 26}.contains(posicao)) {
                    categoria_ = indice;
                }
            }
            for (int indice = 0; indice < 3; ++indice) {
                const float x = 250.f + static_cast<float>(indice) * 178.f;
                if (ui::Box{x, 365, 172, 26}.contains(posicao)) {
                    politica_ = indice;
                }
                if (ui::Box{x, 425, 172, 32}.contains(posicao)) {
                    estrategia_ = indice;
                }
            }
            for (int indice = 0; indice < 7; ++indice) {
                const float x = 250.f + static_cast<float>(indice % 4) * 120.f;
                const float y = 485.f + static_cast<float>(indice / 4) * 30.f;
                if (ui::Box{x, y, 116, 25}.contains(posicao)) {
                    modalidades_[static_cast<std::size_t>(indice)] =
                        !modalidades_[static_cast<std::size_t>(indice)];
                }
            }
        }
    } else if (tela_ == Tela::NovaReserva) {
        if (clicou(evento, {250, 475, 230, 44})) {
            criarReserva();
        }
    } else if (tela_ == Tela::Reservas) {
        if (clicou(evento, {250, 570, 180, 42})) {
            processarReserva(0);
        } else if (clicou(evento, {445, 570, 180, 42})) {
            processarReserva(1);
        } else if (clicou(evento, {640, 570, 180, 42})) {
            processarReserva(2);
        }
    } else if (tela_ == Tela::Avaliacoes) {
        if (clicou(evento, {800, 410, 280, 40})) {
            avaliarReserva();
        }
    } else if (tela_ == Tela::Manutencao) {
        if (clicou(evento, {250, 430, 240, 42})) {
            registrarManutencao();
        } else if (clicou(evento, {510, 430, 360, 42})) {
            encerrarManutencao();
        }
    }
}

void App::atualizarRelogio() {
    const std::string data = campos_[0].value();
    if (ServicoReservas::dataValida(data) && data != sistema_.getDataHoje()) {
        sistema_.definirDataHoje(data);
    }
    const int hora = inteiro(campos_[1].value());
    if (hora >= 0 && hora <= 23 && hora != sistema_.getHoraAgora()) {
        sistema_.definirHoraAgora(hora);
    }
}

void App::mostrarMensagem(const std::string& mensagem, bool sucesso) {
    mensagem_ = mensagem;
    mensagemSucesso_ = sucesso;
}

bool App::clicou(const sf::Event& evento, ui::Box box) const {
    const auto* clique = evento.getIf<sf::Event::MouseButtonPressed>();
    return clique != nullptr && clique->button == sf::Mouse::Button::Left &&
           box.contains(clique->position);
}

int App::inteiro(const std::string& texto, int padrao) const {
    if (texto.empty() || texto.size() > 9) {
        return padrao;
    }
    int resultado = 0;
    for (char caractere : texto) {
        if (caractere < '0' || caractere > '9') {
            return padrao;
        }
        if (resultado > (std::numeric_limits<int>::max() - (caractere - '0')) / 10) {
            return padrao;
        }
        resultado = resultado * 10 + caractere - '0';
    }
    return resultado;
}

std::vector<ui::TextField*> App::camposDaTela() {
    switch (tela_) {
        case Tela::Clientes: return {&campos_[2], &campos_[3]};
        case Tela::Quadras: return {&campos_[4], &campos_[5], &campos_[6]};
        case Tela::NovaReserva: return {&campos_[7], &campos_[8], &campos_[9]};
        case Tela::Avaliacoes: return {&campos_[10], &campos_[11]};
        case Tela::Manutencao: return {&campos_[12], &campos_[13], &campos_[14]};
        case Tela::Reservas: return {};
    }
    return {};
}

std::vector<Reserva*> App::todasReservas() const {
    std::vector<Reserva*> reservas;
    for (int indiceCliente = 0; indiceCliente < sistema_.getQtdClientes(); ++indiceCliente) {
        const std::vector<Reserva*> doCliente =
            sistema_.getCliente(indiceCliente)->getReservas();
        reservas.insert(reservas.end(), doCliente.begin(), doCliente.end());
    }
    std::sort(reservas.begin(), reservas.end(),
              [](const Reserva* esquerda, const Reserva* direita) {
                  return esquerda->getId() < direita->getId();
              });
    return reservas;
}

Cliente* App::buscarCliente(int id) const {
    for (int indice = 0; indice < sistema_.getQtdClientes(); ++indice) {
        Cliente* cliente = sistema_.getCliente(indice);
        if (cliente != nullptr && cliente->getId() == id) {
            return cliente;
        }
    }
    return nullptr;
}

Quadra* App::buscarQuadra(int id) const {
    for (int indice = 0; indice < sistema_.getQtdQuadras(); ++indice) {
        Quadra* quadra = sistema_.getQuadra(indice);
        if (quadra != nullptr && quadra->getId() == id) {
            return quadra;
        }
    }
    return nullptr;
}

Reserva* App::buscarReserva(int id) const {
    for (Reserva* reserva : todasReservas()) {
        if (reserva->getId() == id) {
            return reserva;
        }
    }
    return nullptr;
}

std::vector<ui::ListItem> App::clientesParaLista() const {
    std::vector<ui::ListItem> itens;
    for (int indice = 0; indice < sistema_.getQtdClientes(); ++indice) {
        const Cliente* cliente = sistema_.getCliente(indice);
        std::string linha = "#" + std::to_string(cliente->getId()) + " " +
                            cliente->getNome() + " | " + cliente->getEmail();
        if (cliente->getFaltas() > 0) {
            linha += " | faltas: " + std::to_string(cliente->getFaltas());
        }
        if (cliente->getDescontoProximaReserva() > 0) {
            linha += " | cupom " +
                     std::to_string(cliente->getDescontoProximaReserva()) + "%";
        }
        if (cliente->estaBloqueado()) {
            linha += " | BLOQUEADO";
        }
        itens.push_back({cliente->getId(), std::move(linha)});
    }
    return itens;
}

std::vector<ui::ListItem> App::quadrasParaLista() const {
    std::vector<ui::ListItem> itens;
    for (int indice = 0; indice < sistema_.getQtdQuadras(); ++indice) {
        const Quadra* quadra = sistema_.getQuadra(indice);
        std::string linha = "#" + std::to_string(quadra->getId()) + " " +
                            quadra->getNome() + " | " + quadra->getCategoria() +
                            " | " + dinheiro(quadra->getPrecoHora()) + "/h";
        if (!quadra->isAtiva()) {
            linha += " | INATIVA";
        }
        if (sistema_.quadraEmManutencao(quadra->getId())) {
            linha += " | MANUTENCAO";
        }
        itens.push_back({quadra->getId(), std::move(linha)});
    }
    return itens;
}

std::vector<ui::ListItem> App::reservasParaLista(bool avaliaveis) const {
    std::vector<ui::ListItem> itens;
    for (const Reserva* reserva : todasReservas()) {
        if (avaliaveis &&
            (reserva->getStatus() != StatusReserva::Concluida || reserva->foiAvaliada())) {
            continue;
        }
        const Cliente* cliente = reserva->getCliente();
        const Quadra* quadra = reserva->getQuadra();
        std::string linha = "#" + std::to_string(reserva->getId()) + " " +
                            reserva->getDataReserva() + " " +
                            horaTexto(reserva->getHorarioInicio()) + "-" +
                            horaTexto(reserva->getHorarioFim()) + " | " +
                            (quadra == nullptr ? "Quadra removida" : quadra->getNome()) +
                            " | " +
                            (cliente == nullptr ? "Cliente removido" : cliente->getNome());
        if (avaliaveis) {
            itens.push_back({reserva->getId(), std::move(linha)});
        } else {
            linha += " | " + reserva->getStatusTexto() + " | " +
                     dinheiro(reserva->getValorTotal());
            if (reserva->getStatus() == StatusReserva::Cancelada) {
                linha += " | reembolso " + dinheiro(reserva->getValorReembolso());
            }
            itens.push_back({reserva->getId(), std::move(linha)});
        }
    }
    return itens;
}

std::vector<ui::ListItem> App::manutencoesParaLista() const {
    std::vector<ui::ListItem> itens;
    for (int indice = 0; indice < sistema_.getQtdManutencoes(); ++indice) {
        const Manutencao* manutencao = sistema_.getManutencao(indice);
        const Quadra* quadra = buscarQuadra(manutencao->getQuadraId());
        std::string linha = "#" + std::to_string(manutencao->getId()) + " Quadra " +
                            (quadra == nullptr ? std::to_string(manutencao->getQuadraId())
                                               : quadra->getNome()) +
                            " | " + manutencao->getDataInicio() + " a " +
                            manutencao->getDataFim() + " | " + manutencao->getMotivo() +
                            (manutencao->estaAberta() ? " | ABERTA" : " | ENCERRADA");
        itens.push_back({manutencao->getId(), std::move(linha)});
    }
    return itens;
}

void App::atualizarListas() {
    switch (tela_) {
        case Tela::Clientes:
            listaA_.setBox({600, 100, 480, 500});
            listaA_.setItems(clientesParaLista());
            listaB_.setItems({});
            break;
        case Tela::Quadras:
            listaA_.setBox({640, 100, 440, 430});
            listaA_.setItems(quadrasParaLista());
            listaB_.setItems({});
            break;
        case Tela::NovaReserva:
            listaA_.setBox({245, 110, 395, 230});
            listaA_.setItems(clientesParaLista());
            listaB_.setBox({660, 110, 420, 230});
            listaB_.setItems(quadrasParaLista());
            break;
        case Tela::Reservas:
            listaA_.setBox({245, 100, 835, 440});
            listaA_.setItems(reservasParaLista());
            listaB_.setItems({});
            break;
        case Tela::Avaliacoes: {
            listaA_.setBox({245, 100, 480, 270});
            listaA_.setItems(reservasParaLista(true));
            std::vector<ui::ListItem> avaliacoes;
            for (int indice = 0; indice < sistema_.getQtdAvaliacoes(); ++indice) {
                const Avaliacao* avaliacao = sistema_.getAvaliacao(indice);
                const Quadra* quadra = buscarQuadra(avaliacao->getQuadraId());
                const std::string nomeQuadra = quadra == nullptr
                                                    ? "Quadra removida"
                                                    : quadra->getNome();
                avaliacoes.push_back({
                    avaliacao->getId(),
                    "#" + std::to_string(avaliacao->getId()) + " Nota " +
                        std::to_string(avaliacao->getNota()) + " | " + nomeQuadra +
                        " | " + avaliacao->getComentario()
                });
            }
            listaB_.setBox({245, 500, 835, 145});
            listaB_.setItems(std::move(avaliacoes));
            break;
        }
        case Tela::Manutencao:
            listaA_.setBox({245, 110, 390, 210});
            listaA_.setItems(quadrasParaLista());
            listaB_.setBox({660, 110, 420, 230});
            listaB_.setItems(manutencoesParaLista());
            break;
    }
}

void App::cadastrarCliente() {
    if (campos_[2].value().empty() || campos_[3].value().empty()) {
        mostrarMensagem("Informe o nome e o email do cliente.", false);
        return;
    }
    const int id = sistema_.cadastrarCliente(campos_[2].value(), campos_[3].value());
    campos_[2].clear();
    campos_[3].clear();
    atualizarListas();
    mostrarMensagem("Cliente cadastrado. ID: " + std::to_string(id), true);
}

void App::cadastrarQuadra() {
    std::vector<TipoQuadra> tipos;
    const TipoQuadra opcoes[] = {
        FUTEBOL, BASQUETE, VOLEI, TENIS, BEACH_TENNIS, PADEL, HANDEBOL
    };
    for (std::size_t indice = 0; indice < modalidades_.size(); ++indice) {
        if (modalidades_[indice]) {
            tipos.push_back(opcoes[indice]);
        }
    }
    double preco = 0;
    if (campos_[4].value().empty() || campos_[5].value().empty() ||
        tipos.empty() || !parsePreco(campos_[6].value(), preco)) {
        mostrarMensagem("Preencha nome, local, preco e ao menos uma modalidade.", false);
        return;
    }
    const int id = sistema_.cadastrarQuadra(
        campos_[4].value(), campos_[5].value(), tipos, preco,
        static_cast<TipoEstrategia>(estrategia_),
        static_cast<Categoria>(categoria_),
        static_cast<TipoPolitica>(politica_));
    campos_[4].clear();
    campos_[5].clear();
    campos_[6].clear();
    modalidades_.fill(false);
    atualizarListas();
    mostrarMensagem("Quadra cadastrada. ID: " + std::to_string(id), true);
}

void App::criarReserva() {
    Cliente* cliente = buscarCliente(listaA_.selectedId());
    Quadra* quadra = buscarQuadra(listaB_.selectedId());
    const int hora = inteiro(campos_[8].value());
    const int duracao = inteiro(campos_[9].value());
    if (cliente == nullptr || quadra == nullptr || campos_[7].value().empty() ||
        hora < 0 || duracao <= 0) {
        mostrarMensagem("Selecione cliente e quadra; informe data, inicio e duracao.", false);
        return;
    }
    try {
        Reserva* reserva = sistema_.criarReserva(
            *cliente, *quadra, campos_[7].value(), hora, duracao);
        atualizarListas();
        mostrarMensagem("Reserva #" + std::to_string(reserva->getId()) +
                        " criada por " + dinheiro(reserva->getValorTotal()) + ".", true);
    } catch (const std::invalid_argument& erro) {
        mostrarMensagem(erro.what(), false);
    } catch (const std::runtime_error& erro) {
        mostrarMensagem(erro.what(), false);
    }
}

void App::processarReserva(int acao) {
    Reserva* reserva = buscarReserva(listaA_.selectedId());
    if (reserva == nullptr) {
        mostrarMensagem("Selecione uma reserva.", false);
        return;
    }
    bool sucesso = false;
    std::string resultado;
    if (acao == 0) {
        sucesso = sistema_.concluirReserva(*reserva);
        resultado = "Reserva concluida.";
    } else if (acao == 1) {
        sucesso = sistema_.registrarFalta(*reserva);
        resultado = "Falta registrada.";
    } else {
        sucesso = sistema_.cancelarReserva(*reserva);
        resultado = "Reserva cancelada. Reembolso: " +
                    dinheiro(reserva->getValorReembolso()) + ".";
    }
    if (!sucesso) {
        mostrarMensagem("A operacao nao se aplica a esta reserva ou data.", false);
        return;
    }
    atualizarListas();
    mostrarMensagem(resultado, true);
}

void App::avaliarReserva() {
    const int nota = inteiro(campos_[10].value());
    if (nota < 1 || nota > 5) {
        mostrarMensagem("A nota deve ser um numero de 1 a 5.", false);
        return;
    }
    const int id = sistema_.avaliarReserva(
        listaA_.selectedId(), nota, campos_[11].value());
    if (id < 0) {
        mostrarMensagem("Nao foi possivel registrar a avaliacao.", false);
        return;
    }
    campos_[10].clear();
    campos_[11].clear();
    atualizarListas();
    mostrarMensagem("Avaliacao registrada. ID: " + std::to_string(id), true);
}

void App::registrarManutencao() {
    const int quadraId = listaA_.selectedId();
    const int id = sistema_.registrarManutencao(
        quadraId, campos_[12].value(), campos_[13].value(), campos_[14].value());
    if (id < 0) {
        mostrarMensagem("Verifique a quadra, as datas e o motivo da manutencao.", false);
        return;
    }
    campos_[12].clear();
    campos_[13].clear();
    campos_[14].clear();
    atualizarListas();
    mostrarMensagem("Manutencao registrada; reservas do periodo foram processadas.", true);
}

void App::encerrarManutencao() {
    if (!sistema_.encerrarManutencao(listaB_.selectedId())) {
        mostrarMensagem("Selecione uma manutencao aberta.", false);
        return;
    }
    atualizarListas();
    mostrarMensagem("Manutencao encerrada.", true);
}

void App::alternarEstadoQuadra() {
    Quadra* quadra = buscarQuadra(listaA_.selectedId());
    if (quadra == nullptr) {
        mostrarMensagem("Selecione uma quadra.", false);
        return;
    }
    quadra->setAtiva(!quadra->isAtiva());
    atualizarListas();
    mostrarMensagem("Estado da quadra atualizado.", true);
}

void App::desenharLateral() {
    ui::drawBox(janela_, {0, 0, larguraLateral, static_cast<float>(alturaJanela)},
                ui::colors::sidebar);
    ui::drawText(janela_, fonte_, "RESERVAS", 22, 20, 28, ui::colors::text, true);
    for (int indice = 0; indice < 6; ++indice) {
        const sf::Color fill = static_cast<int>(tela_) == indice
                                   ? ui::colors::selection
                                   : ui::colors::panel;
        ui::drawButton(janela_, fonte_, menuBox(indice), nomesTelas[indice], fill);
    }
    ui::drawText(janela_, fonte_, "Data de hoje", 13, 18, 497, ui::colors::muted);
    campos_[0].draw(janela_, fonte_);
    ui::drawText(janela_, fonte_, "Hora", 13, 18, 565, ui::colors::muted);
    campos_[1].draw(janela_, fonte_);
    ui::drawText(janela_, fonte_,
                 "Clientes: " + std::to_string(sistema_.getQtdClientes()),
                 13, 18, 650, ui::colors::muted);
    ui::drawText(janela_, fonte_,
                 "Quadras: " + std::to_string(sistema_.getQtdQuadras()),
                 13, 18, 672, ui::colors::muted);
    ui::drawText(janela_, fonte_,
                 "Avaliacoes: " + std::to_string(sistema_.getQtdAvaliacoes()),
                 13, 18, 694, ui::colors::muted);
}

void App::desenharTela() {
    atualizarListas();
    const std::string titulo = nomesTelas[static_cast<int>(tela_)];
    ui::drawText(janela_, fonte_, titulo, 26, 245, 28, ui::colors::text, true);

    if (tela_ == Tela::Clientes) {
        ui::drawText(janela_, fonte_, "Nome", 14, 250, 105, ui::colors::muted);
        campos_[2].draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Email", 14, 250, 180, ui::colors::muted);
        campos_[3].draw(janela_, fonte_);
        ui::drawButton(janela_, fonte_, {250, 270, 300, 42}, "Cadastrar cliente");
        ui::drawButton(janela_, fonte_, {250, 330, 300, 42},
                       "Zerar faltas do selecionado", ui::colors::selection);
        ui::drawText(janela_, fonte_, "Clientes cadastrados", 15, 600, 75,
                     ui::colors::muted);
        listaA_.draw(janela_, fonte_);
    } else if (tela_ == Tela::Quadras) {
        ui::drawText(janela_, fonte_, "Nome", 13, 250, 82, ui::colors::muted);
        campos_[4].draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Localizacao", 13, 250, 137, ui::colors::muted);
        campos_[5].draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Preco/hora", 13, 250, 192, ui::colors::muted);
        campos_[6].draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Categoria", 15, 250, 250, ui::colors::muted);
        for (int indice = 0; indice < 4; ++indice) {
            const float x = 250.f + static_cast<float>(indice % 2) * 190.f;
            const float y = 270.f + static_cast<float>(indice / 2) * 32.f;
            ui::drawBox(janela_, {x, y + 3, 18, 18}, ui::colors::field,
                        categoria_ == indice ? ui::colors::accent : ui::colors::border, 2);
            if (categoria_ == indice) {
                ui::drawBox(janela_, {x + 5, y + 8, 8, 8}, ui::colors::accent);
            }
            ui::drawText(janela_, fonte_, nomesCategorias[indice], 14, x + 26, y,
                         ui::colors::text);
        }
        ui::drawText(janela_, fonte_, "Politica de cancelamento", 15, 250, 342,
                     ui::colors::muted);
        for (int indice = 0; indice < 3; ++indice) {
            const float x = 250.f + static_cast<float>(indice) * 178.f;
            ui::drawBox(janela_, {x, 368, 16, 16}, ui::colors::field,
                        politica_ == indice ? ui::colors::accent : ui::colors::border, 2);
            ui::drawText(janela_, fonte_, nomesPoliticas[indice], 13, x + 22, 365,
                         ui::colors::text);
        }
        ui::drawText(janela_, fonte_, "Estrategia de preco", 15, 250, 402,
                     ui::colors::muted);
        for (int indice = 0; indice < 3; ++indice) {
            const float x = 250.f + static_cast<float>(indice) * 178.f;
            ui::drawBox(janela_, {x, 430, 16, 16}, ui::colors::field,
                        estrategia_ == indice ? ui::colors::accent : ui::colors::border, 2);
            ui::drawText(janela_, fonte_, nomesEstrategias[indice], 12, x + 22, 427,
                         ui::colors::text);
        }
        ui::drawText(janela_, fonte_, "Modalidades", 15, 250, 462, ui::colors::muted);
        for (int indice = 0; indice < 7; ++indice) {
            const float x = 250.f + static_cast<float>(indice % 4) * 120.f;
            const float y = 485.f + static_cast<float>(indice / 4) * 30.f;
            ui::drawBox(janela_, {x, y + 3, 17, 17}, ui::colors::field,
                        modalidades_[static_cast<std::size_t>(indice)]
                            ? ui::colors::accent : ui::colors::border,
                        2);
            if (modalidades_[static_cast<std::size_t>(indice)]) {
                ui::drawBox(janela_, {x + 5, y + 8, 7, 7}, ui::colors::accent);
            }
            ui::drawText(janela_, fonte_, nomesModalidades[indice], 12, x + 22, y,
                         ui::colors::text);
        }
        ui::drawButton(janela_, fonte_, {250, 590, 340, 42}, "Cadastrar quadra");
        ui::drawText(janela_, fonte_, "Quadras cadastradas", 15, 640, 75,
                     ui::colors::muted);
        listaA_.draw(janela_, fonte_);
        ui::drawButton(janela_, fonte_, {640, 555, 440, 42},
                       "Ativar / inativar quadra selecionada", ui::colors::selection);
    } else if (tela_ == Tela::NovaReserva) {
        ui::drawText(janela_, fonte_, "Selecione um cliente", 14, 245, 85,
                     ui::colors::muted);
        listaA_.draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Selecione uma quadra", 14, 660, 85,
                     ui::colors::muted);
        listaB_.draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Data", 13, 250, 372, ui::colors::muted);
        campos_[7].draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Inicio (0-23)", 13, 450, 372, ui::colors::muted);
        campos_[8].draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Duracao (horas)", 13, 560, 372,
                     ui::colors::muted);
        campos_[9].draw(janela_, fonte_);
        ui::drawButton(janela_, fonte_, {250, 475, 230, 44}, "Criar reserva");
    } else if (tela_ == Tela::Reservas) {
        ui::drawText(janela_, fonte_, "Reservas dos clientes cadastrados", 14,
                     245, 75, ui::colors::muted);
        listaA_.draw(janela_, fonte_);
        ui::drawButton(janela_, fonte_, {250, 570, 180, 42}, "Concluir");
        ui::drawButton(janela_, fonte_, {445, 570, 180, 42}, "Registrar falta",
                       ui::colors::warning);
        ui::drawButton(janela_, fonte_, {640, 570, 180, 42}, "Cancelar",
                       ui::colors::danger);
    } else if (tela_ == Tela::Avaliacoes) {
        ui::drawText(janela_, fonte_, "Reservas concluidas sem avaliacao", 14,
                     245, 75, ui::colors::muted);
        listaA_.draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Nota", 13, 250, 388, ui::colors::muted);
        campos_[10].draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Comentario", 13, 350, 388,
                     ui::colors::muted);
        campos_[11].draw(janela_, fonte_);
        ui::drawButton(janela_, fonte_, {800, 410, 280, 40}, "Enviar avaliacao");
        ui::drawText(janela_, fonte_, "Avaliacoes registradas", 14, 245, 475,
                     ui::colors::muted);
        listaB_.draw(janela_, fonte_);
    } else if (tela_ == Tela::Manutencao) {
        ui::drawText(janela_, fonte_, "Quadra", 14, 245, 85, ui::colors::muted);
        listaA_.draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Manutencoes", 14, 660, 85,
                     ui::colors::muted);
        listaB_.draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Inicio", 13, 250, 347, ui::colors::muted);
        campos_[12].draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Fim", 13, 450, 347, ui::colors::muted);
        campos_[13].draw(janela_, fonte_);
        ui::drawText(janela_, fonte_, "Motivo", 13, 650, 347, ui::colors::muted);
        campos_[14].draw(janela_, fonte_);
        ui::drawButton(janela_, fonte_, {250, 430, 240, 42}, "Registrar manutencao",
                       ui::colors::danger);
        ui::drawButton(janela_, fonte_, {510, 430, 360, 42},
                       "Encerrar manutencao selecionada", ui::colors::selection);
    }
}

void App::desenharStatus() {
    ui::drawBox(janela_, {230, 710, 950, 34}, ui::colors::panel);
    ui::drawText(janela_, fonte_, mensagem_, 13, 245, 718,
                 mensagemSucesso_ ? ui::colors::successText : ui::colors::errorText);
}

void App::desenhar() {
    janela_.clear(ui::colors::background);
    desenharLateral();
    desenharTela();
    desenharStatus();
    janela_.display();
}
