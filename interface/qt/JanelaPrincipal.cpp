#include "JanelaPrincipal.hpp"

#include "Dialogos.hpp"
#include "MapaQuadras.hpp"
#include "Sistema.hpp"
#include "Util.hpp"

#include <QAction>
#include <QDateEdit>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QStringList>
#include <QTabWidget>
#include <QTableWidget>
#include <QTime>
#include <QToolBar>
#include <QVBoxLayout>

#include <algorithm>

namespace {

const QColor kOcupadaFundo("#c0392b");
const QColor kLivreFundo("#d8f0e0");
const QColor kLivreTexto("#1b4332");

QTableWidget* criarTabela(const QStringList& cabecalhos) {
    auto* tabela = new QTableWidget(0, cabecalhos.size());
    tabela->setHorizontalHeaderLabels(cabecalhos);
    tabela->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabela->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabela->setSelectionMode(QAbstractItemView::SingleSelection);
    tabela->setAlternatingRowColors(true);
    tabela->verticalHeader()->setVisible(false);
    tabela->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    return tabela;
}

QTableWidgetItem* celula(const QString& texto) {
    return new QTableWidgetItem(texto);
}

// Botões em português (os padrões do QMessageBox dependem de tradução do Qt).
bool confirmar(QWidget* pai, const QString& titulo, const QString& texto,
               const QString& rotuloSim, const QString& rotuloNao) {
    QMessageBox caixa(QMessageBox::Question, titulo, texto, QMessageBox::NoButton, pai);
    QPushButton* sim = caixa.addButton(rotuloSim, QMessageBox::AcceptRole);
    caixa.addButton(rotuloNao, QMessageBox::RejectRole);
    caixa.exec();
    return caixa.clickedButton() == sim;
}

}  // namespace

JanelaPrincipal::JanelaPrincipal(Sistema& sistema, QWidget* parent)
    : QMainWindow(parent), sistema_(sistema), data_(new QDateEdit(QDate::currentDate())),
      horaMapa_(new QSpinBox), abas_(new QTabWidget), mapa_(nullptr), agenda_(nullptr),
      reservas_(nullptr), clientes_(nullptr), quadras_(nullptr) {
    setWindowTitle("Gerenciador de Reservas de Espaços Esportivos");
    resize(1020, 680);

    // Data compartilhada pelo mapa e pela agenda.
    data_->setCalendarPopup(true);
    data_->setDisplayFormat("dd/MM/yyyy");
    auto* hoje = new QPushButton("Hoje");
    QToolBar* barra = addToolBar("Principal");
    barra->setMovable(false);
    barra->addWidget(new QLabel(" Data: "));
    barra->addWidget(data_);
    barra->addWidget(hoje);
    barra->addSeparator();
    QAction* nova = barra->addAction("Nova reserva");

    horaMapa_->setRange(0, 23);
    horaMapa_->setSuffix(":00");
    horaMapa_->setValue(QTime::currentTime().hour());

    abas_->addTab(criarAbaMapa(), "Mapa");
    abas_->addTab(criarAbaAgenda(), "Agenda do dia");
    abas_->addTab(criarAbaReservas(), "Reservas");
    abas_->addTab(criarAbaClientes(), "Clientes");
    abas_->addTab(criarAbaQuadras(), "Quadras");
    setCentralWidget(abas_);

    connect(hoje, &QPushButton::clicked, this, [this] { data_->setDate(QDate::currentDate()); });
    connect(nova, &QAction::triggered, this,
            [this] { novaReserva(nullptr, data_->date(), horaMapa_->value()); });
    connect(data_, &QDateEdit::dateChanged, this, [this] { atualizarMapa(); atualizarAgenda(); });
    connect(horaMapa_, &QSpinBox::valueChanged, this, [this] { atualizarMapa(); });

    atualizarTudo();
}

// ------------------------------------------------------------------ abas

QWidget* JanelaPrincipal::criarAbaMapa() {
    auto* aba = new QWidget;
    auto* topo = new QHBoxLayout;
    topo->addWidget(new QLabel("Horário:"));
    topo->addWidget(horaMapa_);
    topo->addSpacing(16);
    topo->addWidget(new QLabel("<span style='color:#1f8a4c'>■</span> livre &nbsp;&nbsp; "
                               "<span style='color:#c0392b'>■</span> ocupada &nbsp;&nbsp; "
                               "— clique numa quadra para reservar ou ver a reserva"));
    topo->addStretch();

    mapa_ = new MapaQuadras;
    connect(mapa_, &MapaQuadras::quadraClicada, this,
            [this](Quadra* quadra) { abrirHorario(quadra, horaMapa_->value()); });

    auto* layout = new QVBoxLayout(aba);
    layout->addLayout(topo);
    layout->addWidget(mapa_, 1);
    return aba;
}

QWidget* JanelaPrincipal::criarAbaAgenda() {
    agenda_ = new QTableWidget(24, 0);
    agenda_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    agenda_->setSelectionMode(QAbstractItemView::SingleSelection);
    agenda_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(agenda_, &QTableWidget::cellDoubleClicked, this, [this](int linha, int coluna) {
        const auto& quadras = sistema_.quadras();
        if (coluna >= 0 && coluna < static_cast<int>(quadras.size())) {
            abrirHorario(quadras[coluna].get(), linha);
        }
    });

    auto* aba = new QWidget;
    auto* layout = new QVBoxLayout(aba);
    layout->addWidget(new QLabel("Dê dois cliques num horário livre para reservar, "
                                 "ou num ocupado para ver os detalhes."));
    layout->addWidget(agenda_, 1);
    return aba;
}

QWidget* JanelaPrincipal::criarAbaReservas() {
    reservas_ = criarTabela({"Cliente", "Quadra", "Data", "Início", "Duração", "Valor"});
    auto* nova = new QPushButton("Nova reserva");
    auto* cancelar = new QPushButton("Cancelar selecionada");
    connect(nova, &QPushButton::clicked, this,
            [this] { novaReserva(nullptr, data_->date(), horaMapa_->value()); });
    connect(cancelar, &QPushButton::clicked, this, [this] { cancelarSelecionada(); });

    auto* botoes = new QHBoxLayout;
    botoes->addWidget(nova);
    botoes->addWidget(cancelar);
    botoes->addStretch();

    auto* aba = new QWidget;
    auto* layout = new QVBoxLayout(aba);
    layout->addWidget(reservas_, 1);
    layout->addLayout(botoes);
    return aba;
}

QWidget* JanelaPrincipal::criarAbaClientes() {
    clientes_ = criarTabela({"ID", "Nome", "E-mail"});
    auto* adicionar = new QPushButton("Novo cliente");
    connect(adicionar, &QPushButton::clicked, this, [this] { cadastrarCliente(); });

    auto* aba = new QWidget;
    auto* layout = new QVBoxLayout(aba);
    layout->addWidget(clientes_, 1);
    auto* botoes = new QHBoxLayout;
    botoes->addWidget(adicionar);
    botoes->addStretch();
    layout->addLayout(botoes);
    return aba;
}

QWidget* JanelaPrincipal::criarAbaQuadras() {
    quadras_ = criarTabela({"ID", "Nome", "Localização", "Modalidades", "Preço/hora"});
    auto* adicionar = new QPushButton("Nova quadra");
    connect(adicionar, &QPushButton::clicked, this, [this] { cadastrarQuadra(); });

    auto* aba = new QWidget;
    auto* layout = new QVBoxLayout(aba);
    layout->addWidget(quadras_, 1);
    auto* botoes = new QHBoxLayout;
    botoes->addWidget(adicionar);
    botoes->addStretch();
    layout->addLayout(botoes);
    return aba;
}

// ----------------------------------------------------------- atualização

void JanelaPrincipal::atualizarTudo() {
    atualizarMapa();
    atualizarAgenda();
    atualizarReservas();
    atualizarClientes();
    atualizarQuadras();
}

void JanelaPrincipal::atualizarMapa() {
    mapa_->atualizar(sistema_, data_->date(), horaMapa_->value());
}

void JanelaPrincipal::atualizarAgenda() {
    const auto& quadras = sistema_.quadras();
    const std::string dataIso = util::dataIso(data_->date());

    agenda_->clear();
    agenda_->setColumnCount(static_cast<int>(quadras.size()));
    QStringList colunas;
    for (const auto& q : quadras) {
        colunas << util::texto(q->getNome());
    }
    agenda_->setHorizontalHeaderLabels(colunas);
    QStringList linhas;
    for (int h = 0; h < 24; ++h) {
        linhas << util::hora(h);
    }
    agenda_->setVerticalHeaderLabels(linhas);

    for (int c = 0; c < static_cast<int>(quadras.size()); ++c) {
        for (int h = 0; h < 24; ++h) {
            const Reserva* r = sistema_.reservaEm(quadras[c].get(), dataIso, h);
            auto* item = new QTableWidgetItem;
            item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
            if (r) {
                item->setBackground(kOcupadaFundo);
                item->setForeground(Qt::white);
                if (h == r->getHorarioInicio()) {
                    item->setText(util::texto(r->getCliente()->getNome()) +
                                  QString(" (%1 h)").arg(r->getDuracaoHoras()));
                }
                item->setToolTip(util::descricao(*r));
            } else {
                item->setBackground(kLivreFundo);
                item->setForeground(kLivreTexto);
                item->setText("livre");
            }
            agenda_->setItem(h, c, item);
        }
    }
}

void JanelaPrincipal::atualizarReservas() {
    auto ativas = sistema_.reservasAtivas();
    std::sort(ativas.begin(), ativas.end(), [](const Reserva* a, const Reserva* b) {
        if (a->getDataReserva() != b->getDataReserva()) {
            return a->getDataReserva() < b->getDataReserva();
        }
        return a->getHorarioInicio() < b->getHorarioInicio();
    });

    reservas_->setRowCount(static_cast<int>(ativas.size()));
    for (int i = 0; i < static_cast<int>(ativas.size()); ++i) {
        const Reserva* r = ativas[i];
        auto* primeira = celula(util::texto(r->getCliente()->getNome()));
        primeira->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(ativas[i])));
        reservas_->setItem(i, 0, primeira);
        reservas_->setItem(i, 1, celula(util::texto(r->getQuadra()->getNome())));
        reservas_->setItem(i, 2, celula(util::dataBr(r->getDataReserva())));
        reservas_->setItem(i, 3, celula(util::hora(r->getHorarioInicio())));
        reservas_->setItem(i, 4, celula(QString("%1 h").arg(r->getDuracaoHoras())));
        reservas_->setItem(i, 5, celula(util::dinheiro(r->getValorTotal())));
    }
}

void JanelaPrincipal::atualizarClientes() {
    const auto& lista = sistema_.clientes();
    clientes_->setRowCount(static_cast<int>(lista.size()));
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        clientes_->setItem(i, 0, celula(QString::number(lista[i]->getId())));
        clientes_->setItem(i, 1, celula(util::texto(lista[i]->getNome())));
        clientes_->setItem(i, 2, celula(util::texto(lista[i]->getEmail())));
    }
}

void JanelaPrincipal::atualizarQuadras() {
    const auto& lista = sistema_.quadras();
    quadras_->setRowCount(static_cast<int>(lista.size()));
    for (int i = 0; i < static_cast<int>(lista.size()); ++i) {
        QStringList modalidades;
        for (TipoQuadra tipo : lista[i]->getTipos()) {
            modalidades << util::texto(Sistema::nomeTipo(tipo));
        }
        quadras_->setItem(i, 0, celula(QString::number(lista[i]->getId())));
        quadras_->setItem(i, 1, celula(util::texto(lista[i]->getNome())));
        quadras_->setItem(i, 2, celula(util::texto(lista[i]->getLocalizacao())));
        quadras_->setItem(i, 3, celula(modalidades.join(", ")));
        quadras_->setItem(i, 4, celula(util::dinheiro(lista[i]->getPrecoHora())));
    }
}

// ---------------------------------------------------------------- ações

void JanelaPrincipal::novaReserva(Quadra* quadra, const QDate& data, int hora) {
    if (sistema_.clientes().empty() || sistema_.quadras().empty()) {
        QMessageBox::information(this, "Nova reserva",
                                 "Cadastre ao menos um cliente e uma quadra antes de reservar.");
        return;
    }
    DialogoReserva dialogo(sistema_, this, quadra, data, hora);
    if (dialogo.exec() == QDialog::Accepted) {
        atualizarTudo();
    }
}

void JanelaPrincipal::abrirHorario(Quadra* quadra, int hora) {
    if (Reserva* r = sistema_.reservaEm(quadra, util::dataIso(data_->date()), hora)) {
        mostrarReserva(r);
    } else {
        novaReserva(quadra, data_->date(), hora);
    }
}

void JanelaPrincipal::mostrarReserva(Reserva* reserva) {
    QMessageBox caixa(QMessageBox::Information, "Reserva", util::descricao(*reserva),
                      QMessageBox::NoButton, this);
    QPushButton* cancelar = caixa.addButton("Cancelar reserva", QMessageBox::DestructiveRole);
    caixa.addButton("Fechar", QMessageBox::RejectRole);
    caixa.exec();
    if (caixa.clickedButton() == cancelar) {
        sistema_.cancelarReserva(reserva);
        atualizarTudo();
    }
}

void JanelaPrincipal::cancelarSelecionada() {
    const int linha = reservas_->currentRow();
    if (linha < 0) {
        QMessageBox::information(this, "Cancelar reserva", "Selecione uma reserva na tabela.");
        return;
    }
    auto* reserva = static_cast<Reserva*>(
        reservas_->item(linha, 0)->data(Qt::UserRole).value<void*>());
    if (confirmar(this, "Cancelar reserva", util::descricao(*reserva) + "\n\nCancelar esta reserva?",
                  "Cancelar reserva", "Manter")) {
        sistema_.cancelarReserva(reserva);
        atualizarTudo();
    }
}

void JanelaPrincipal::cadastrarCliente() {
    DialogoCliente dialogo(sistema_, this);
    if (dialogo.exec() == QDialog::Accepted) {
        atualizarTudo();
    }
}

void JanelaPrincipal::cadastrarQuadra() {
    DialogoQuadra dialogo(sistema_, this);
    if (dialogo.exec() == QDialog::Accepted) {
        atualizarTudo();
    }
}
