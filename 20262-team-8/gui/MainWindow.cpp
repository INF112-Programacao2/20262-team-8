#include "MainWindow.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QLocale>
#include <QMessageBox>
#include <QPushButton>
#include <QRegularExpression>
#include <QStackedWidget>
#include <QStatusBar>
#include <QTableWidget>
#include <QVBoxLayout>

#include <algorithm>

namespace {

const char* kEstilo = R"QSS(
QMainWindow, QWidget#pagina { background: #F3F5F7; }
QWidget { font-size: 14px; color: #1F2937; }

/* ---------- Sidebar ---------- */
QFrame#sidebar { background: #0E2A23; }
QLabel#marca { color: #FFFFFF; font-size: 22px; font-weight: 700; }
QLabel#marcaSub { color: #7FB89F; font-size: 12px; }
QLabel#rodape { color: #5E8F7B; font-size: 11px; }
QListWidget#menu { background: transparent; border: none; outline: none; }
QListWidget#menu::item {
    color: #C5DDD2; padding: 12px 16px; margin: 2px 0; border-radius: 10px;
}
QListWidget#menu::item:hover { background: #17402F; }
QListWidget#menu::item:selected { background: #22C55E; color: #06210F; font-weight: 700; }

/* ---------- Cabeçalhos ---------- */
QLabel#titulo { font-size: 26px; font-weight: 700; color: #111827; }
QLabel#subtitulo { color: #6B7280; font-size: 14px; }
QLabel#secao { font-size: 16px; font-weight: 700; color: #111827; }

/* ---------- Cards ---------- */
QFrame#card { background: #FFFFFF; border: 1px solid #E5E7EB; border-radius: 14px; }
QFrame#card QLabel { background: transparent; }
QLabel#statRotulo { color: #6B7280; font-size: 13px; }
QLabel#statValor { font-size: 28px; font-weight: 700; color: #111827; }
QFrame#barra { border-radius: 2px; }

/* ---------- Campos ---------- */
QLabel#campo { color: #374151; font-weight: 600; font-size: 13px; }
QLineEdit, QComboBox, QSpinBox, QDoubleSpinBox, QDateEdit {
    background: #FFFFFF; border: 1px solid #D1D5DB; border-radius: 8px;
    padding: 7px 10px; min-height: 20px; selection-background-color: #22C55E;
}
QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDoubleSpinBox:focus, QDateEdit:focus {
    border: 1px solid #16A34A;
}
QComboBox::drop-down, QDateEdit::drop-down {
    subcontrol-origin: padding; subcontrol-position: center right;
    width: 28px; border: none; background: transparent;
}
QComboBox::down-arrow, QDateEdit::down-arrow { image: url(:/chevron.png); width: 12px; height: 12px; }
QSpinBox::up-button, QSpinBox::down-button,
QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
    width: 0px; border: none;
}
QComboBox QAbstractItemView {
    background: #FFFFFF; border: 1px solid #D1D5DB;
    selection-background-color: #DCFCE7; selection-color: #14532D;
}
QCheckBox { spacing: 8px; }
QCheckBox::indicator {
    width: 16px; height: 16px; border: 1px solid #9CA3AF; border-radius: 4px; background: #FFFFFF;
}
QCheckBox::indicator:checked { background: #16A34A; border: 1px solid #16A34A; }

/* ---------- Botões ---------- */
QPushButton {
    background: #16A34A; color: #FFFFFF; border: none; border-radius: 9px;
    padding: 10px 18px; font-weight: 700;
}
QPushButton:hover { background: #15803D; }
QPushButton:pressed { background: #166534; }
QPushButton:disabled { background: #D1D5DB; color: #9CA3AF; }
QPushButton#perigo { background: #FEE2E2; color: #B91C1C; }
QPushButton#perigo:hover { background: #FECACA; }
QPushButton#perigo:disabled { background: #F3F4F6; color: #D1D5DB; }

/* ---------- Tabelas ---------- */
QTableWidget {
    background: #FFFFFF; alternate-background-color: #F9FAFB; border: none;
    gridline-color: transparent; selection-background-color: #DCFCE7; selection-color: #14532D;
}
QTableWidget::item { padding: 6px 10px; border-bottom: 1px solid #F1F3F5; }
QHeaderView::section {
    background: #FFFFFF; color: #6B7280; font-weight: 700; font-size: 12px;
    border: none; border-bottom: 2px solid #E5E7EB; padding: 8px 10px;
}
QScrollBar:vertical { background: transparent; width: 10px; margin: 2px; }
QScrollBar::handle:vertical { background: #D1D5DB; border-radius: 4px; min-height: 30px; }
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }

QStatusBar { background: #FFFFFF; color: #374151; border-top: 1px solid #E5E7EB; }
QStatusBar QLabel { padding-left: 14px; }
)QSS";

QString moeda(double valor) {
    return "R$ " + QLocale(QLocale::Portuguese, QLocale::Brazil).toString(valor, 'f', 2);
}

QString nomeTipo(TipoQuadra tipo) {
    switch (tipo) {
        case FUTEBOL: return QStringLiteral("Futebol");
        case BASQUETE: return QStringLiteral("Basquete");
        case VOLEI: return QStringLiteral("Vôlei");
        case TENIS: return QStringLiteral("Tênis");
        case BEACH_TENNIS: return QStringLiteral("Beach tennis");
        case PADEL: return QStringLiteral("Padel");
        case HANDEBOL: return QStringLiteral("Handebol");
    }
    return QStringLiteral("Desconhecida");
}

QString hora(int h) {
    return QString("%1:00").arg(h, 2, 10, QChar('0'));
}

QString dataBr(const QString& iso) {
    const QDate d = QDate::fromString(iso, "yyyy-MM-dd");
    return d.isValid() ? d.toString("dd/MM/yyyy") : iso;
}

QFrame* novoCard() {
    auto* f = new QFrame;
    f->setObjectName("card");
    return f;
}

QLabel* rotulo(const QString& texto, const char* nomeObjeto) {
    auto* l = new QLabel(texto);
    l->setObjectName(nomeObjeto);
    return l;
}

QWidget* cabecalho(const QString& titulo, const QString& subtitulo) {
    auto* w = new QWidget;
    auto* v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(2);
    v->addWidget(rotulo(titulo, "titulo"));
    v->addWidget(rotulo(subtitulo, "subtitulo"));
    return w;
}

QWidget* campo(const QString& nome, QWidget* widget) {
    auto* w = new QWidget;
    auto* v = new QVBoxLayout(w);
    v->setContentsMargins(0, 0, 0, 0);
    v->setSpacing(5);
    v->addWidget(rotulo(nome, "campo"));
    v->addWidget(widget);
    return w;
}

// colunasAuto: colunas que se ajustam ao conteúdo; as demais dividem o espaço restante.
// colunaDireita: coluna (numérica) alinhada à direita, inclusive no cabeçalho.
QTableWidget* novaTabela(const QStringList& colunas, const QList<int>& colunasAuto = {},
                         int colunaDireita = -1) {
    auto* t = new QTableWidget(0, colunas.size());
    t->setHorizontalHeaderLabels(colunas);
    t->setEditTriggers(QAbstractItemView::NoEditTriggers);
    t->setSelectionBehavior(QAbstractItemView::SelectRows);
    t->setSelectionMode(QAbstractItemView::SingleSelection);
    t->setAlternatingRowColors(true);
    t->setShowGrid(false);
    t->setFocusPolicy(Qt::NoFocus);
    t->verticalHeader()->hide();
    t->verticalHeader()->setDefaultSectionSize(40);
    auto* h = t->horizontalHeader();
    h->setSectionResizeMode(QHeaderView::Stretch);
    for (int c : colunasAuto) h->setSectionResizeMode(c, QHeaderView::ResizeToContents);
    h->setHighlightSections(false);
    h->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    if (colunaDireita >= 0) {
        t->horizontalHeaderItem(colunaDireita)->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }
    return t;
}

void preencherLinha(QTableWidget* t, int linha, const QStringList& valores,
                    int colunaDireita = -1) {
    for (int c = 0; c < valores.size(); ++c) {
        auto* item = new QTableWidgetItem(valores[c]);
        Qt::Alignment al = Qt::AlignVCenter | (c == colunaDireita ? Qt::AlignRight : Qt::AlignLeft);
        item->setTextAlignment(al);
        t->setItem(linha, c, item);
    }
}

QFrame* cartaoEstatistica(const QString& nome, const QString& cor, QLabel*& valor) {
    auto* card = novoCard();
    auto* h = new QHBoxLayout(card);
    h->setContentsMargins(18, 16, 18, 16);
    h->setSpacing(14);

    auto* barra = new QFrame;
    barra->setObjectName("barra");
    barra->setFixedWidth(5);
    barra->setStyleSheet(QString("background:%1;").arg(cor));
    h->addWidget(barra);

    auto* v = new QVBoxLayout;
    v->setSpacing(2);
    v->addWidget(rotulo(nome, "statRotulo"));
    valor = rotulo("0", "statValor");
    v->addWidget(valor);
    h->addLayout(v, 1);
    return card;
}

} // namespace

// ===================================================================
//  Construção
// ===================================================================

MainWindow::MainWindow(bool carregarDemo, QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Reservas de Quadras");
    resize(1280, 760);
    setMinimumSize(1100, 640);
    setStyleSheet(kEstilo);

    auto* central = new QWidget;
    auto* raiz = new QHBoxLayout(central);
    raiz->setContentsMargins(0, 0, 0, 0);
    raiz->setSpacing(0);
    raiz->addWidget(criarSidebar());

    paginas = new QStackedWidget;
    paginas->addWidget(criarPaginaPainel());
    paginas->addWidget(criarPaginaClientes());
    paginas->addWidget(criarPaginaQuadras());
    paginas->addWidget(criarPaginaReservas());
    raiz->addWidget(paginas, 1);
    setCentralWidget(central);

    connect(menu, &QListWidget::currentRowChanged, paginas, &QStackedWidget::setCurrentIndex);
    menu->setCurrentRow(0);

    if (carregarDemo) {
        carregarDados();
    }
    atualizarTudo();
    atualizarResumoReserva();
    statusBar()->showMessage("Pronto.");
}

MainWindow::~MainWindow() = default;

void MainWindow::irParaPagina(int indice) {
    menu->setCurrentRow(indice);
}

QWidget* MainWindow::criarSidebar() {
    auto* side = new QFrame;
    side->setObjectName("sidebar");
    side->setFixedWidth(230);

    auto* v = new QVBoxLayout(side);
    v->setContentsMargins(16, 24, 16, 16);
    v->setSpacing(4);
    v->addWidget(rotulo("Quadras", "marca"));
    v->addWidget(rotulo("Gerenciador de reservas", "marcaSub"));
    v->addSpacing(22);

    menu = new QListWidget;
    menu->setObjectName("menu");
    menu->setFocusPolicy(Qt::NoFocus);
    menu->addItems({"Painel", "Clientes", "Quadras", "Reservas"});
    v->addWidget(menu, 1);
    v->addWidget(rotulo("Time 8 · 2026/2", "rodape"));
    return side;
}

QWidget* MainWindow::criarPaginaPainel() {
    auto* pagina = new QWidget;
    pagina->setObjectName("pagina");
    auto* v = new QVBoxLayout(pagina);
    v->setContentsMargins(30, 26, 30, 20);
    v->setSpacing(18);
    v->addWidget(cabecalho("Painel", "Visão geral das suas quadras e reservas."));

    auto* grade = new QHBoxLayout;
    grade->setSpacing(14);
    grade->addWidget(cartaoEstatistica("Clientes", "#3B82F6", valClientes));
    grade->addWidget(cartaoEstatistica("Quadras", "#F59E0B", valQuadras));
    grade->addWidget(cartaoEstatistica("Reservas ativas", "#22C55E", valReservas));
    grade->addWidget(cartaoEstatistica("Receita prevista", "#8B5CF6", valReceita));
    v->addLayout(grade);

    auto* card = novoCard();
    auto* cv = new QVBoxLayout(card);
    cv->setContentsMargins(20, 18, 20, 12);
    cv->setSpacing(10);
    cv->addWidget(rotulo("Próximas reservas", "secao"));
    tabProximas = novaTabela({"Data", "Horário", "Quadra", "Cliente", "Valor"}, {0, 1, 4}, 4);
    tabProximas->setSelectionMode(QAbstractItemView::NoSelection);
    cv->addWidget(tabProximas, 1);
    v->addWidget(card, 1);
    return pagina;
}

QWidget* MainWindow::criarPaginaClientes() {
    auto* pagina = new QWidget;
    pagina->setObjectName("pagina");
    auto* v = new QVBoxLayout(pagina);
    v->setContentsMargins(30, 26, 30, 20);
    v->setSpacing(18);
    v->addWidget(cabecalho("Clientes", "Cadastre e consulte quem reserva as quadras."));

    auto* linha = new QHBoxLayout;
    linha->setSpacing(18);

    // formulário
    auto* form = novoCard();
    form->setFixedWidth(310);
    auto* fv = new QVBoxLayout(form);
    fv->setContentsMargins(20, 18, 20, 20);
    fv->setSpacing(14);
    fv->addWidget(rotulo("Novo cliente", "secao"));
    edNomeCliente = new QLineEdit;
    edNomeCliente->setPlaceholderText("Ex.: Ana Souza");
    edEmail = new QLineEdit;
    edEmail->setPlaceholderText("ana@email.com");
    fv->addWidget(campo("Nome", edNomeCliente));
    fv->addWidget(campo("E-mail", edEmail));
    auto* btn = new QPushButton("Cadastrar cliente");
    btn->setCursor(Qt::PointingHandCursor);
    fv->addWidget(btn);
    fv->addStretch(1);
    linha->addWidget(form);

    // tabela
    auto* card = novoCard();
    auto* cv = new QVBoxLayout(card);
    cv->setContentsMargins(20, 18, 20, 12);
    cv->setSpacing(10);
    cv->addWidget(rotulo("Clientes cadastrados", "secao"));
    tabClientes = novaTabela({"ID", "Nome", "E-mail", "Reservas ativas"}, {0, 3});
    cv->addWidget(tabClientes, 1);
    linha->addWidget(card, 1);
    v->addLayout(linha, 1);

    connect(btn, &QPushButton::clicked, this, &MainWindow::cadastrarCliente);
    connect(edEmail, &QLineEdit::returnPressed, this, &MainWindow::cadastrarCliente);
    connect(edNomeCliente, &QLineEdit::returnPressed, edEmail, qOverload<>(&QWidget::setFocus));
    return pagina;
}

QWidget* MainWindow::criarPaginaQuadras() {
    auto* pagina = new QWidget;
    pagina->setObjectName("pagina");
    auto* v = new QVBoxLayout(pagina);
    v->setContentsMargins(30, 26, 30, 20);
    v->setSpacing(18);
    v->addWidget(cabecalho("Quadras", "Espaços esportivos, modalidades e preço por hora."));

    auto* linha = new QHBoxLayout;
    linha->setSpacing(18);

    auto* form = novoCard();
    form->setFixedWidth(310);
    auto* fv = new QVBoxLayout(form);
    fv->setContentsMargins(20, 18, 20, 20);
    fv->setSpacing(14);
    fv->addWidget(rotulo("Nova quadra", "secao"));

    edNomeQuadra = new QLineEdit;
    edNomeQuadra->setPlaceholderText("Ex.: Arena Central");
    edLocal = new QLineEdit;
    edLocal->setPlaceholderText("Ex.: Bloco B, térreo");
    spPreco = new QDoubleSpinBox;
    spPreco->setPrefix("R$ ");
    spPreco->setRange(0.0, 99999.99);
    spPreco->setDecimals(2);
    spPreco->setSingleStep(5.0);
    spPreco->setValue(100.0);
    spPreco->setLocale(QLocale(QLocale::Portuguese, QLocale::Brazil));
    fv->addWidget(campo("Nome", edNomeQuadra));
    fv->addWidget(campo("Localização", edLocal));
    fv->addWidget(campo("Preço por hora", spPreco));

    auto* grade = new QGridLayout;
    grade->setHorizontalSpacing(12);
    grade->setVerticalSpacing(8);
    const TipoQuadra todos[] = {FUTEBOL, BASQUETE, VOLEI, TENIS, BEACH_TENNIS, PADEL, HANDEBOL};
    int i = 0;
    for (TipoQuadra t : todos) {
        auto* cb = new QCheckBox(nomeTipo(t));
        checksTipos.push_back({t, cb});
        grade->addWidget(cb, i / 2, i % 2);
        ++i;
    }
    auto* blocoTipos = new QWidget;
    blocoTipos->setLayout(grade);
    fv->addWidget(campo("Modalidades", blocoTipos));

    auto* btn = new QPushButton("Cadastrar quadra");
    btn->setCursor(Qt::PointingHandCursor);
    fv->addWidget(btn);
    fv->addStretch(1);
    linha->addWidget(form);

    auto* card = novoCard();
    auto* cv = new QVBoxLayout(card);
    cv->setContentsMargins(20, 18, 20, 12);
    cv->setSpacing(10);
    cv->addWidget(rotulo("Quadras cadastradas", "secao"));
    tabQuadras = novaTabela({"ID", "Nome", "Localização", "Modalidades", "Preço/hora"}, {0, 3, 4}, 4);
    cv->addWidget(tabQuadras, 1);
    linha->addWidget(card, 1);
    v->addLayout(linha, 1);

    connect(btn, &QPushButton::clicked, this, &MainWindow::cadastrarQuadra);
    return pagina;
}

QWidget* MainWindow::criarPaginaReservas() {
    auto* pagina = new QWidget;
    pagina->setObjectName("pagina");
    auto* v = new QVBoxLayout(pagina);
    v->setContentsMargins(30, 26, 30, 20);
    v->setSpacing(18);
    v->addWidget(cabecalho("Reservas", "Agende horários e acompanhe as reservas ativas."));

    auto* linha = new QHBoxLayout;
    linha->setSpacing(18);

    auto* form = novoCard();
    form->setFixedWidth(310);
    auto* fv = new QVBoxLayout(form);
    fv->setContentsMargins(20, 18, 20, 20);
    fv->setSpacing(14);
    fv->addWidget(rotulo("Nova reserva", "secao"));

    cbCliente = new QComboBox;
    cbQuadra = new QComboBox;
    deData = new QDateEdit(QDate::currentDate());
    deData->setCalendarPopup(true);
    deData->setDisplayFormat("dd/MM/yyyy");
    deData->setMinimumDate(QDate::currentDate());
    cbInicio = new QComboBox;
    for (int h = 0; h < 24; ++h) cbInicio->addItem(hora(h));
    cbInicio->setCurrentIndex(18);
    cbDuracao = new QComboBox;
    for (int d = 1; d <= 24; ++d) cbDuracao->addItem(d == 1 ? "1 hora" : QString("%1 horas").arg(d));

    fv->addWidget(campo("Cliente", cbCliente));
    fv->addWidget(campo("Quadra", cbQuadra));
    fv->addWidget(campo("Data", deData));
    auto* horarios = new QHBoxLayout;
    horarios->setSpacing(12);
    horarios->addWidget(campo("Início", cbInicio));
    horarios->addWidget(campo("Duração", cbDuracao));
    fv->addLayout(horarios);

    lblResumo = new QLabel;
    lblResumo->setWordWrap(true);
    lblResumo->setStyleSheet("background:#F0FDF4; color:#14532D; border-radius:8px; padding:10px;");
    fv->addWidget(lblResumo);

    btnReservar = new QPushButton("Confirmar reserva");
    btnReservar->setCursor(Qt::PointingHandCursor);
    fv->addWidget(btnReservar);
    fv->addStretch(1);
    linha->addWidget(form);

    auto* card = novoCard();
    auto* cv = new QVBoxLayout(card);
    cv->setContentsMargins(20, 18, 20, 16);
    cv->setSpacing(10);
    cv->addWidget(rotulo("Reservas ativas", "secao"));
    tabReservas = novaTabela({"Data", "Horário", "Quadra", "Cliente", "Valor"}, {0, 1, 4}, 4);
    cv->addWidget(tabReservas, 1);

    btnCancelar = new QPushButton("Cancelar reserva selecionada");
    btnCancelar->setObjectName("perigo");
    btnCancelar->setCursor(Qt::PointingHandCursor);
    btnCancelar->setEnabled(false);
    cv->addWidget(btnCancelar, 0, Qt::AlignRight);
    linha->addWidget(card, 1);
    v->addLayout(linha, 1);

    connect(btnReservar, &QPushButton::clicked, this, &MainWindow::criarReserva);
    connect(btnCancelar, &QPushButton::clicked, this, &MainWindow::cancelarReservaSelecionada);
    connect(tabReservas, &QTableWidget::itemSelectionChanged, this,
            [this] { btnCancelar->setEnabled(!tabReservas->selectedItems().isEmpty()); });
    connect(cbQuadra, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &MainWindow::atualizarResumoReserva);
    connect(cbInicio, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &MainWindow::atualizarResumoReserva);
    connect(cbDuracao, qOverload<int>(&QComboBox::currentIndexChanged), this,
            &MainWindow::atualizarResumoReserva);
    return pagina;
}

// ===================================================================
//  Dados
// ===================================================================

Cliente* MainWindow::adicionarCliente(const QString& nome, const QString& email) {
    clientes.push_back(std::make_unique<Cliente>(nome.toStdString(), email.toStdString()));
    return clientes.back().get();
}

Quadra* MainWindow::adicionarQuadra(const QString& nome, const QString& local,
                                    const std::vector<TipoQuadra>& tipos, double preco) {
    // o construtor de Quadra recebe referências não-const
    std::string n = nome.toStdString();
    std::string l = local.toStdString();
    std::vector<TipoQuadra> t = tipos;
    quadras.push_back(std::make_unique<Quadra>(n, l, t, preco));
    return quadras.back().get();
}

bool MainWindow::adicionarReserva(Cliente* c, Quadra* q, const QString& data, int inicio,
                                  int duracao, QString* erro) {
    if (inicio + duracao > 24) {
        if (erro) *erro = "A reserva deve terminar até as 24h.";
        return false;
    }
    const std::string d = data.toStdString();
    for (const Reserva* r : Reserva::listarReservasAtivas()) {
        if (r->getQuadra() == q && r->getDataReserva() == d &&
            inicio < r->getHorarioInicio() + r->getDuracaoHoras() &&
            r->getHorarioInicio() < inicio + duracao) {
            if (erro) {
                *erro = QString("A quadra já possui uma reserva nesse horário (%1 às %2).")
                            .arg(hora(r->getHorarioInicio()))
                            .arg(hora(r->getHorarioInicio() + r->getDuracaoHoras()));
            }
            return false;
        }
    }
    try {
        new Reserva(c, q, d, inicio, duracao);  // o cliente passa a ser dono da reserva
    } catch (const std::exception& e) {
        if (erro) *erro = QString::fromUtf8(e.what());
        return false;
    }
    return true;
}

void MainWindow::carregarDados() {
    Cliente* ana = adicionarCliente("Ana Souza", "ana.souza@email.com");
    Cliente* bruno = adicionarCliente("Bruno Lima", "bruno.lima@email.com");
    Cliente* carla = adicionarCliente("Carla Mendes", "carla.m@email.com");
    adicionarCliente("Diego Ramos", "diego.ramos@email.com");

    Quadra* arena = adicionarQuadra("Arena Central", "Bloco A", {FUTEBOL, HANDEBOL}, 120.0);
    Quadra* sol = adicionarQuadra("Quadra Sol", "Área externa", {VOLEI, BEACH_TENNIS}, 80.0);
    Quadra* padel = adicionarQuadra("Padel Club", "Bloco C", {PADEL, TENIS}, 150.0);

    const QDate hoje = QDate::currentDate();
    auto iso = [&](int dias) { return hoje.addDays(dias).toString("yyyy-MM-dd"); };
    adicionarReserva(ana, arena, iso(0), 19, 2);
    adicionarReserva(bruno, sol, iso(1), 8, 1);
    adicionarReserva(carla, padel, iso(1), 18, 2);
    adicionarReserva(ana, padel, iso(2), 20, 1);
    adicionarReserva(bruno, arena, iso(3), 10, 3);
    adicionarReserva(carla, sol, iso(4), 17, 2);
}

// ===================================================================
//  Ações
// ===================================================================

void MainWindow::cadastrarCliente() {
    const QString nome = edNomeCliente->text().trimmed();
    const QString email = edEmail->text().trimmed();

    if (nome.isEmpty()) {
        QMessageBox::warning(this, "Cliente", "Informe o nome do cliente.");
        edNomeCliente->setFocus();
        return;
    }
    static const QRegularExpression rxEmail(R"(^[^@\s]+@[^@\s]+\.[^@\s]+$)");
    if (!rxEmail.match(email).hasMatch()) {
        QMessageBox::warning(this, "Cliente", "Informe um e-mail válido.");
        edEmail->setFocus();
        return;
    }

    Cliente* c = adicionarCliente(nome, email);
    edNomeCliente->clear();
    edEmail->clear();
    edNomeCliente->setFocus();
    atualizarTudo();
    notificar(QString("Cliente %1 cadastrado (ID %2).").arg(nome).arg(c->getId()));
}

void MainWindow::cadastrarQuadra() {
    const QString nome = edNomeQuadra->text().trimmed();
    const QString local = edLocal->text().trimmed();

    if (nome.isEmpty() || local.isEmpty()) {
        QMessageBox::warning(this, "Quadra", "Informe o nome e a localização da quadra.");
        return;
    }
    if (spPreco->value() <= 0.0) {
        QMessageBox::warning(this, "Quadra", "O preço por hora deve ser maior que zero.");
        return;
    }
    std::vector<TipoQuadra> tipos;
    for (const auto& [tipo, cb] : checksTipos) {
        if (cb->isChecked()) tipos.push_back(tipo);
    }
    if (tipos.empty()) {
        QMessageBox::warning(this, "Quadra", "Selecione ao menos uma modalidade.");
        return;
    }

    Quadra* q = adicionarQuadra(nome, local, tipos, spPreco->value());
    edNomeQuadra->clear();
    edLocal->clear();
    for (const auto& par : checksTipos) par.second->setChecked(false);
    atualizarTudo();
    notificar(QString("Quadra %1 cadastrada (ID %2).").arg(nome).arg(q->getId()));
}

void MainWindow::criarReserva() {
    if (clientes.empty() || quadras.empty()) {
        QMessageBox::information(this, "Reserva",
                                 "Cadastre ao menos um cliente e uma quadra antes de reservar.");
        return;
    }
    Cliente* cliente = clientes[cbCliente->currentIndex()].get();
    Quadra* quadra = quadras[cbQuadra->currentIndex()].get();
    const QString data = deData->date().toString("yyyy-MM-dd");

    QString erro;
    if (!adicionarReserva(cliente, quadra, data, cbInicio->currentIndex(),
                          cbDuracao->currentIndex() + 1, &erro)) {
        QMessageBox::warning(this, "Não foi possível reservar", erro);
        return;
    }
    atualizarTudo();
    const auto ativas = Reserva::listarReservasAtivas();
    notificar(QString("Reserva criada para %1 · %2.")
                  .arg(cliente->getNome().c_str())
                  .arg(moeda(ativas.back()->getValorTotal())));
}

void MainWindow::cancelarReservaSelecionada() {
    const auto selecionados = tabReservas->selectedItems();
    if (selecionados.isEmpty()) return;
    const int linha = selecionados.first()->row();

    auto* reserva = reinterpret_cast<Reserva*>(
        tabReservas->item(linha, 0)->data(Qt::UserRole).value<quintptr>());
    const auto ativas = Reserva::listarReservasAtivas();
    if (std::find(ativas.begin(), ativas.end(), reserva) == ativas.end()) return;

    const auto resp = QMessageBox::question(
        this, "Cancelar reserva",
        QString("Cancelar a reserva de %1 em %2, %3?")
            .arg(reserva->getCliente()->getNome().c_str())
            .arg(reserva->getQuadra()->getNome().c_str())
            .arg(dataBr(QString::fromStdString(reserva->getDataReserva()))),
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
    if (resp != QMessageBox::Yes) return;

    reserva->cancelarReserva();
    atualizarTudo();
    notificar("Reserva cancelada.");
}

void MainWindow::atualizarResumoReserva() {
    const int inicio = cbInicio->currentIndex();
    const int duracao = cbDuracao->currentIndex() + 1;
    const bool temQuadra = !quadras.empty() && cbQuadra->currentIndex() >= 0;
    const bool estoura = inicio + duracao > 24;

    if (estoura) {
        lblResumo->setStyleSheet("background:#FEF2F2; color:#991B1B; border-radius:8px; padding:10px;");
        lblResumo->setText("A reserva ultrapassa as 24h. Ajuste o início ou a duração.");
    } else if (!temQuadra) {
        lblResumo->setStyleSheet("background:#F3F4F6; color:#4B5563; border-radius:8px; padding:10px;");
        lblResumo->setText("Cadastre uma quadra para ver o valor da reserva.");
    } else {
        const double total = quadras[cbQuadra->currentIndex()]->calcularValorReserva(duracao);
        lblResumo->setStyleSheet("background:#F0FDF4; color:#14532D; border-radius:8px; padding:10px;");
        lblResumo->setText(QString("%1 – %2   ·   Total: <b>%3</b>")
                               .arg(hora(inicio)).arg(hora(inicio + duracao)).arg(moeda(total)));
    }
    btnReservar->setEnabled(!estoura && temQuadra && !clientes.empty());
}

void MainWindow::notificar(const QString& mensagem) {
    statusBar()->showMessage(mensagem, 6000);
}

// ===================================================================
//  Atualização das telas
// ===================================================================

void MainWindow::atualizarTudo() {
    atualizarClientes();
    atualizarQuadras();
    atualizarReservas();
    atualizarPainel();
    atualizarResumoReserva();
}

void MainWindow::atualizarClientes() {
    const auto ativas = Reserva::listarReservasAtivas();
    tabClientes->setRowCount(static_cast<int>(clientes.size()));
    for (std::size_t i = 0; i < clientes.size(); ++i) {
        const Cliente* c = clientes[i].get();
        const auto n = std::count_if(ativas.begin(), ativas.end(),
                                     [c](const Reserva* r) { return r->getCliente() == c; });
        preencherLinha(tabClientes, static_cast<int>(i),
                       {QString::number(c->getId()), QString::fromStdString(c->getNome()),
                        QString::fromStdString(c->getEmail()), QString::number(n)});
    }

    const int atual = cbCliente->currentIndex();
    cbCliente->clear();
    for (const auto& c : clientes) cbCliente->addItem(QString::fromStdString(c->getNome()));
    if (atual >= 0 && atual < cbCliente->count()) cbCliente->setCurrentIndex(atual);
}

void MainWindow::atualizarQuadras() {
    tabQuadras->setRowCount(static_cast<int>(quadras.size()));
    for (std::size_t i = 0; i < quadras.size(); ++i) {
        const Quadra* q = quadras[i].get();
        QStringList nomes;
        for (TipoQuadra t : q->getTipos()) nomes << nomeTipo(t);
        preencherLinha(tabQuadras, static_cast<int>(i),
                       {QString::number(q->getId()), QString::fromStdString(q->getNome()),
                        QString::fromStdString(q->getLocalizacao()), nomes.join(", "),
                        moeda(q->getPrecoHora())},
                       4);
    }

    const int atual = cbQuadra->currentIndex();
    cbQuadra->blockSignals(true);
    cbQuadra->clear();
    for (const auto& q : quadras) {
        cbQuadra->addItem(QString("%1  ·  %2")
                              .arg(QString::fromStdString(q->getNome()), moeda(q->getPrecoHora())));
    }
    if (atual >= 0 && atual < cbQuadra->count()) cbQuadra->setCurrentIndex(atual);
    cbQuadra->blockSignals(false);
}

void MainWindow::atualizarReservas() {
    auto ativas = Reserva::listarReservasAtivas();
    std::sort(ativas.begin(), ativas.end(), [](const Reserva* a, const Reserva* b) {
        if (a->getDataReserva() != b->getDataReserva())
            return a->getDataReserva() < b->getDataReserva();
        return a->getHorarioInicio() < b->getHorarioInicio();
    });

    tabReservas->setRowCount(static_cast<int>(ativas.size()));
    for (std::size_t i = 0; i < ativas.size(); ++i) {
        const Reserva* r = ativas[i];
        const int linha = static_cast<int>(i);
        preencherLinha(tabReservas, linha,
                       {dataBr(QString::fromStdString(r->getDataReserva())),
                        hora(r->getHorarioInicio()) + " – " +
                            hora(r->getHorarioInicio() + r->getDuracaoHoras()),
                        QString::fromStdString(r->getQuadra()->getNome()),
                        QString::fromStdString(r->getCliente()->getNome()),
                        moeda(r->getValorTotal())},
                       4);
        tabReservas->item(linha, 0)->setData(
            Qt::UserRole, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(r)));
    }
    btnCancelar->setEnabled(false);
}

void MainWindow::atualizarPainel() {
    auto ativas = Reserva::listarReservasAtivas();
    double receita = 0.0;
    for (const Reserva* r : ativas) receita += r->getValorTotal();

    valClientes->setText(QString::number(clientes.size()));
    valQuadras->setText(QString::number(quadras.size()));
    valReservas->setText(QString::number(ativas.size()));
    valReceita->setText(moeda(receita));

    // próximas reservas (a partir de hoje), ordenadas por data e hora
    const std::string hoje = QDate::currentDate().toString("yyyy-MM-dd").toStdString();
    ativas.erase(std::remove_if(ativas.begin(), ativas.end(),
                                [&](const Reserva* r) { return r->getDataReserva() < hoje; }),
                 ativas.end());
    std::sort(ativas.begin(), ativas.end(), [](const Reserva* a, const Reserva* b) {
        if (a->getDataReserva() != b->getDataReserva())
            return a->getDataReserva() < b->getDataReserva();
        return a->getHorarioInicio() < b->getHorarioInicio();
    });
    const int n = static_cast<int>(std::min<std::size_t>(ativas.size(), 8));
    tabProximas->setRowCount(n);
    for (int i = 0; i < n; ++i) {
        const Reserva* r = ativas[i];
        preencherLinha(tabProximas, i,
                       {dataBr(QString::fromStdString(r->getDataReserva())),
                        hora(r->getHorarioInicio()) + " – " +
                            hora(r->getHorarioInicio() + r->getDuracaoHoras()),
                        QString::fromStdString(r->getQuadra()->getNome()),
                        QString::fromStdString(r->getCliente()->getNome()),
                        moeda(r->getValorTotal())},
                       4);
    }
}
