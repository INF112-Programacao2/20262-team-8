#ifndef JANELAPRINCIPAL_HPP
#define JANELAPRINCIPAL_HPP

#include <QDate>
#include <QMainWindow>

class QDateEdit;
class QSpinBox;
class QTabWidget;
class QTableWidget;

class MapaQuadras;
class Quadra;
class Reserva;
class Sistema;

class JanelaPrincipal : public QMainWindow {
    Q_OBJECT
    public:
        explicit JanelaPrincipal(Sistema& sistema, QWidget* parent = nullptr);

    private:
        QWidget* criarAbaMapa();
        QWidget* criarAbaAgenda();
        QWidget* criarAbaReservas();
        QWidget* criarAbaClientes();
        QWidget* criarAbaQuadras();

        void atualizarTudo();
        void atualizarMapa();
        void atualizarAgenda();
        void atualizarReservas();
        void atualizarClientes();
        void atualizarQuadras();

        // Ponto único de entrada para criar/ver/cancelar, usado por mapa, agenda e abas.
        void novaReserva(Quadra* quadra, const QDate& data, int hora);
        void abrirHorario(Quadra* quadra, int hora);
        void mostrarReserva(Reserva* reserva);
        void cancelarSelecionada();
        void cadastrarCliente();
        void cadastrarQuadra();

        Sistema& sistema_;
        QDateEdit* data_;
        QSpinBox* horaMapa_;
        QTabWidget* abas_;
        MapaQuadras* mapa_;
        QTableWidget* agenda_;
        QTableWidget* reservas_;
        QTableWidget* clientes_;
        QTableWidget* quadras_;
};

#endif
