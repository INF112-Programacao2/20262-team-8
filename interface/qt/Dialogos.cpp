#include "Dialogos.hpp"

#include "Sistema.hpp"
#include "Util.hpp"

#include <QCheckBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include <algorithm>
#include <exception>

namespace {

QDialogButtonBox* criarBotoes(QDialog* dialogo, const QString& rotuloOk) {
    auto* botoes = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, dialogo);
    botoes->button(QDialogButtonBox::Ok)->setText(rotuloOk);
    botoes->button(QDialogButtonBox::Cancel)->setText("Cancelar");
    QObject::connect(botoes, &QDialogButtonBox::accepted, dialogo, &QDialog::accept);
    QObject::connect(botoes, &QDialogButtonBox::rejected, dialogo, &QDialog::reject);
    return botoes;
}

void mostrarErro(QWidget* pai, const QString& titulo, const std::exception& erro) {
    QMessageBox::warning(pai, titulo, QString::fromUtf8(erro.what()));
}

}  // namespace

// ---------------------------------------------------------------- Cliente

DialogoCliente::DialogoCliente(Sistema& sistema, QWidget* parent)
    : QDialog(parent), sistema_(sistema), nome_(new QLineEdit), email_(new QLineEdit) {
    setWindowTitle("Novo cliente");
    setMinimumWidth(360);

    auto* formulario = new QFormLayout;
    formulario->addRow("Nome:", nome_);
    formulario->addRow("E-mail:", email_);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(formulario);
    layout->addWidget(criarBotoes(this, "Cadastrar"));
}

void DialogoCliente::accept() {
    try {
        sistema_.cadastrarCliente(nome_->text().trimmed().toStdString(),
                                  email_->text().trimmed().toStdString());
        QDialog::accept();
    } catch (const std::exception& erro) {
        mostrarErro(this, "Não foi possível cadastrar", erro);
    }
}

// ----------------------------------------------------------------- Quadra

DialogoQuadra::DialogoQuadra(Sistema& sistema, QWidget* parent)
    : QDialog(parent), sistema_(sistema), nome_(new QLineEdit), localizacao_(new QLineEdit),
      preco_(new QDoubleSpinBox) {
    setWindowTitle("Nova quadra");
    setMinimumWidth(400);

    preco_->setPrefix("R$ ");
    preco_->setDecimals(2);
    preco_->setRange(0.01, 100000.0);
    preco_->setValue(50.0);

    auto* formulario = new QFormLayout;
    formulario->addRow("Nome:", nome_);
    formulario->addRow("Localização:", localizacao_);
    formulario->addRow("Preço por hora:", preco_);

    auto* grupo = new QGroupBox("Modalidades");
    auto* grade = new QGridLayout(grupo);
    int i = 0;
    for (TipoQuadra tipo : Sistema::todosOsTipos()) {
        auto* caixa = new QCheckBox(util::texto(Sistema::nomeTipo(tipo)));
        caixa->setProperty("tipo", static_cast<int>(tipo));
        modalidades_.push_back(caixa);
        grade->addWidget(caixa, i / 2, i % 2);
        ++i;
    }

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(formulario);
    layout->addWidget(grupo);
    layout->addWidget(criarBotoes(this, "Cadastrar"));
}

void DialogoQuadra::accept() {
    std::vector<TipoQuadra> tipos;
    for (const QCheckBox* caixa : modalidades_) {
        if (caixa->isChecked()) {
            tipos.push_back(static_cast<TipoQuadra>(caixa->property("tipo").toInt()));
        }
    }
    try {
        sistema_.cadastrarQuadra(nome_->text().trimmed().toStdString(),
                                 localizacao_->text().trimmed().toStdString(),
                                 tipos, preco_->value());
        QDialog::accept();
    } catch (const std::exception& erro) {
        mostrarErro(this, "Não foi possível cadastrar", erro);
    }
}

// ---------------------------------------------------------------- Reserva

DialogoReserva::DialogoReserva(Sistema& sistema, QWidget* parent, Quadra* quadra,
                               const QDate& data, int hora)
    : QDialog(parent), sistema_(sistema), cliente_(new QComboBox), quadra_(new QComboBox),
      data_(new QDateEdit), inicio_(new QSpinBox), duracao_(new QSpinBox),
      resumo_(new QLabel) {
    setWindowTitle("Nova reserva");
    setMinimumWidth(420);

    for (const auto& c : sistema_.clientes()) {
        cliente_->addItem(util::texto(c->getNome()) + " (" + util::texto(c->getEmail()) + ")",
                          QVariant::fromValue(static_cast<void*>(c.get())));
    }
    for (const auto& q : sistema_.quadras()) {
        quadra_->addItem(util::texto(q->getNome()) + " — " + util::dinheiro(q->getPrecoHora()) + "/h",
                         QVariant::fromValue(static_cast<void*>(q.get())));
        if (q.get() == quadra) {
            quadra_->setCurrentIndex(quadra_->count() - 1);
        }
    }

    // Reservas só a partir de hoje.
    data_->setCalendarPopup(true);
    data_->setDisplayFormat("dd/MM/yyyy");
    data_->setMinimumDate(QDate::currentDate());
    data_->setDate(std::max(data, QDate::currentDate()));

    inicio_->setRange(0, 23);
    inicio_->setSuffix(":00");
    inicio_->setValue(std::clamp(hora, 0, 23));
    duracao_->setSuffix(" h");
    duracao_->setRange(1, 24 - inicio_->value());
    duracao_->setValue(1);

    resumo_->setWordWrap(true);
    resumo_->setTextFormat(Qt::RichText);

    auto* formulario = new QFormLayout;
    formulario->addRow("Cliente:", cliente_);
    formulario->addRow("Quadra:", quadra_);
    formulario->addRow("Data:", data_);
    formulario->addRow("Início:", inicio_);
    formulario->addRow("Duração:", duracao_);

    botoes_ = criarBotoes(this, "Reservar");
    auto* layout = new QVBoxLayout(this);
    layout->addLayout(formulario);
    layout->addWidget(resumo_);
    layout->addWidget(botoes_);

    connect(cliente_, &QComboBox::currentIndexChanged, this, [this] { atualizarResumo(); });
    connect(quadra_, &QComboBox::currentIndexChanged, this, [this] { atualizarResumo(); });
    connect(data_, &QDateEdit::dateChanged, this, [this] { atualizarResumo(); });
    connect(duracao_, &QSpinBox::valueChanged, this, [this] { atualizarResumo(); });
    connect(inicio_, &QSpinBox::valueChanged, this, [this](int inicio) {
        duracao_->setMaximum(24 - inicio);  // a reserva não pode passar da meia-noite
        atualizarResumo();
    });
    atualizarResumo();
}

void DialogoReserva::atualizarResumo() {
    auto* quadra = static_cast<Quadra*>(quadra_->currentData().value<void*>());
    if (quadra == nullptr) {
        return;
    }
    const int inicio = inicio_->value();
    const int duracao = duracao_->value();

    if (sistema_.temConflito(quadra, util::dataIso(data_->date()), inicio, duracao)) {
        resumo_->setText("<span style='color:#e74c3c'><b>Esta quadra já tem reserva nesse horário.</b></span>");
        botoes_->button(QDialogButtonBox::Ok)->setEnabled(false);
    } else {
        resumo_->setText(QString("Das %1 às %2 — valor total: <b>%3</b>")
                             .arg(util::hora(inicio), util::hora(inicio + duracao),
                                  util::dinheiro(quadra->calcularValorReserva(duracao))));
        botoes_->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void DialogoReserva::accept() {
    auto* cliente = static_cast<Cliente*>(cliente_->currentData().value<void*>());
    auto* quadra = static_cast<Quadra*>(quadra_->currentData().value<void*>());
    try {
        sistema_.criarReserva(cliente, quadra, util::dataIso(data_->date()),
                              inicio_->value(), duracao_->value());
        QDialog::accept();
    } catch (const std::exception& erro) {
        mostrarErro(this, "Não foi possível reservar", erro);
    }
}
