#ifndef DIALOGOS_HPP
#define DIALOGOS_HPP

#include <QDate>
#include <QDialog>

#include <vector>

class QCheckBox;
class QComboBox;
class QDateEdit;
class QDialogButtonBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QSpinBox;

class Quadra;
class Sistema;

class DialogoCliente : public QDialog {
    public:
        DialogoCliente(Sistema& sistema, QWidget* parent);
        void accept() override;

    private:
        Sistema& sistema_;
        QLineEdit* nome_;
        QLineEdit* email_;
};

class DialogoQuadra : public QDialog {
    public:
        DialogoQuadra(Sistema& sistema, QWidget* parent);
        void accept() override;

    private:
        Sistema& sistema_;
        QLineEdit* nome_;
        QLineEdit* localizacao_;
        QDoubleSpinBox* preco_;
        std::vector<QCheckBox*> modalidades_;
};

// Pré-preenche quadra/data/hora quando aberto a partir do mapa ou da agenda.
// O valor total e o conflito de horário são mostrados enquanto o usuário edita.
class DialogoReserva : public QDialog {
    public:
        DialogoReserva(Sistema& sistema, QWidget* parent, Quadra* quadra,
                       const QDate& data, int hora);
        void accept() override;

    private:
        void atualizarResumo();

        Sistema& sistema_;
        QComboBox* cliente_;
        QComboBox* quadra_;
        QDateEdit* data_;
        QSpinBox* inicio_;
        QSpinBox* duracao_;
        QLabel* resumo_;
        QDialogButtonBox* botoes_;
};

#endif
