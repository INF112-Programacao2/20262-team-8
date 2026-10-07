#ifndef MAPAQUADRAS_HPP
#define MAPAQUADRAS_HPP

#include <QDate>
#include <QGraphicsView>

class Quadra;
class Sistema;

// Planta esquemática do complexo: cada quadra é um retângulo colorido conforme
// esteja livre ou ocupada na data/hora selecionadas. Clicar numa quadra emite
// quadraClicada(); quem decide o que fazer (reservar, ver detalhes) é a janela.
class MapaQuadras : public QGraphicsView {
    Q_OBJECT
    public:
        explicit MapaQuadras(QWidget* parent = nullptr);
        void atualizar(const Sistema& sistema, const QDate& data, int hora);

    signals:
        void quadraClicada(Quadra* quadra);

    protected:
        void mousePressEvent(QMouseEvent* evento) override;
        void resizeEvent(QResizeEvent* evento) override;

    private:
        void ajustar();
        QGraphicsScene* cena_;
};

#endif
