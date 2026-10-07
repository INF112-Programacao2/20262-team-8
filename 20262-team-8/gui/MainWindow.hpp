#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <memory>
#include <vector>

#include "../models/cliente.hpp"
#include "../models/quadra.hpp"
#include "../models/reserva.hpp"

class QCheckBox;
class QComboBox;
class QDateEdit;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QPushButton;
class QStackedWidget;
class QTableWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(bool carregarDemo = false, QWidget* parent = nullptr);
    ~MainWindow() override;

    void irParaPagina(int indice);

private slots:
    void cadastrarCliente();
    void cadastrarQuadra();
    void criarReserva();
    void cancelarReservaSelecionada();
    void atualizarResumoReserva();

private:
    // construção da interface
    QWidget* criarSidebar();
    QWidget* criarPaginaPainel();
    QWidget* criarPaginaClientes();
    QWidget* criarPaginaQuadras();
    QWidget* criarPaginaReservas();

    // dados
    Cliente* adicionarCliente(const QString& nome, const QString& email);
    Quadra* adicionarQuadra(const QString& nome, const QString& local,
                            const std::vector<TipoQuadra>& tipos, double preco);
    bool adicionarReserva(Cliente* c, Quadra* q, const QString& data, int inicio, int duracao,
                          QString* erro = nullptr);
    void carregarDados();

    // atualização das telas
    void atualizarTudo();
    void atualizarPainel();
    void atualizarClientes();
    void atualizarQuadras();
    void atualizarReservas();
    void notificar(const QString& mensagem);

    // dados em memória (os clientes são donos das reservas)
    std::vector<std::unique_ptr<Cliente>> clientes;
    std::vector<std::unique_ptr<Quadra>> quadras;

    QStackedWidget* paginas = nullptr;
    QListWidget* menu = nullptr;

    // painel
    QLabel* valClientes = nullptr;
    QLabel* valQuadras = nullptr;
    QLabel* valReservas = nullptr;
    QLabel* valReceita = nullptr;
    QTableWidget* tabProximas = nullptr;

    // clientes
    QLineEdit* edNomeCliente = nullptr;
    QLineEdit* edEmail = nullptr;
    QTableWidget* tabClientes = nullptr;

    // quadras
    QLineEdit* edNomeQuadra = nullptr;
    QLineEdit* edLocal = nullptr;
    QDoubleSpinBox* spPreco = nullptr;
    std::vector<std::pair<TipoQuadra, QCheckBox*>> checksTipos;
    QTableWidget* tabQuadras = nullptr;

    // reservas
    QComboBox* cbCliente = nullptr;
    QComboBox* cbQuadra = nullptr;
    QDateEdit* deData = nullptr;
    QComboBox* cbInicio = nullptr;   // índice == hora de início (0-23)
    QComboBox* cbDuracao = nullptr;  // índice + 1 == duração em horas
    QLabel* lblResumo = nullptr;
    QPushButton* btnReservar = nullptr;
    QPushButton* btnCancelar = nullptr;
    QTableWidget* tabReservas = nullptr;
};

#endif
