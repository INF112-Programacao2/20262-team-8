#include "MapaQuadras.hpp"

#include "Sistema.hpp"
#include "Util.hpp"

#include <QFont>
#include <QFontMetrics>
#include <QGraphicsEllipseItem>
#include <QGraphicsLineItem>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSimpleTextItem>
#include <QMouseEvent>
#include <QStringList>

namespace {

constexpr qreal kLargura = 230;
constexpr qreal kAltura = 140;
constexpr qreal kEspaco = 24;
constexpr int kColunas = 3;

const QColor kLivre("#1f8a4c");
const QColor kOcupada("#c0392b");

QGraphicsSimpleTextItem* adicionarTexto(QGraphicsItem* pai, const QString& texto,
                                        qreal x, qreal y, qreal pontos, bool negrito) {
    QFont fonte;
    fonte.setPointSizeF(pontos);
    fonte.setBold(negrito);
    const QString cabe = QFontMetrics(fonte).elidedText(texto, Qt::ElideRight,
                                                        static_cast<int>(kLargura - 2 * x));
    auto* item = new QGraphicsSimpleTextItem(cabe, pai);
    item->setFont(fonte);
    item->setBrush(Qt::white);
    item->setPos(x, y);
    return item;
}

}  // namespace

MapaQuadras::MapaQuadras(QWidget* parent) : QGraphicsView(parent), cena_(new QGraphicsScene(this)) {
    setScene(cena_);
    setRenderHint(QPainter::Antialiasing);
    setAlignment(Qt::AlignTop | Qt::AlignLeft);
    setFrameShape(QFrame::NoFrame);
    setMinimumHeight(260);
}

void MapaQuadras::atualizar(const Sistema& sistema, const QDate& data, int hora) {
    cena_->clear();
    const std::string dataIso = util::dataIso(data);

    int i = 0;
    for (const auto& quadra : sistema.quadras()) {
        const qreal x = (i % kColunas) * (kLargura + kEspaco);
        const qreal y = (i / kColunas) * (kAltura + kEspaco);
        ++i;

        const Reserva* reserva = sistema.reservaEm(quadra.get(), dataIso, hora);

        auto* retangulo = cena_->addRect(0, 0, kLargura, kAltura,
                                         QPen(QColor(255, 255, 255, 200), 3),
                                         QBrush(reserva ? kOcupada : kLivre));
        retangulo->setPos(x, y);
        retangulo->setData(0, QVariant::fromValue(static_cast<void*>(quadra.get())));

        // Marcações de quadra (linha central e círculo), só para dar o aspecto.
        const QPen marcacao(QColor(255, 255, 255, 70), 2);
        auto* linha = new QGraphicsLineItem(kLargura / 2, 0, kLargura / 2, kAltura, retangulo);
        linha->setPen(marcacao);
        auto* circulo = new QGraphicsEllipseItem(kLargura / 2 - 22, kAltura / 2 - 22, 44, 44, retangulo);
        circulo->setPen(marcacao);

        QStringList modalidades;
        for (TipoQuadra tipo : quadra->getTipos()) {
            modalidades << util::texto(Sistema::nomeTipo(tipo));
        }
        adicionarTexto(retangulo, util::texto(quadra->getNome()), 12, 8, 12, true);
        adicionarTexto(retangulo, modalidades.join(", "), 12, 32, 9, false);

        if (reserva) {
            adicionarTexto(retangulo, "Ocupada · " + util::texto(reserva->getCliente()->getNome()),
                           12, kAltura - 50, 10, true);
            adicionarTexto(retangulo,
                           util::hora(reserva->getHorarioInicio()) + " às " +
                               util::hora(reserva->getHorarioInicio() + reserva->getDuracaoHoras()),
                           12, kAltura - 30, 9, false);
        } else {
            adicionarTexto(retangulo, "Livre às " + util::hora(hora), 12, kAltura - 30, 10, true);
        }

        retangulo->setToolTip(QString("%1\n%2\nModalidades: %3\n%4 por hora")
                                  .arg(util::texto(quadra->getNome()),
                                       util::texto(quadra->getLocalizacao()),
                                       modalidades.join(", "),
                                       util::dinheiro(quadra->getPrecoHora())));
    }

    if (i == 0) {
        auto* aviso = cena_->addSimpleText("Nenhuma quadra cadastrada.\nUse a aba \"Quadras\" para cadastrar.");
        aviso->setBrush(palette().text());
    }

    // Área mínima de 3x2 quadras, para que 1 ou 2 quadras não sejam ampliadas demais.
    QRectF area = cena_->itemsBoundingRect().united(
        QRectF(0, 0, kColunas * kLargura + (kColunas - 1) * kEspaco, 2 * kAltura + kEspaco));
    cena_->setSceneRect(area.adjusted(-12, -12, 12, 12));
    ajustar();
}

void MapaQuadras::ajustar() {
    fitInView(cena_->sceneRect(), Qt::KeepAspectRatio);
}

void MapaQuadras::resizeEvent(QResizeEvent* evento) {
    QGraphicsView::resizeEvent(evento);
    ajustar();
}

void MapaQuadras::mousePressEvent(QMouseEvent* evento) {
    if (evento->button() == Qt::LeftButton) {
        // Textos e marcações são filhos do retângulo; sobe até achar a quadra.
        for (QGraphicsItem* item = itemAt(evento->position().toPoint()); item;
             item = item->parentItem()) {
            const QVariant dado = item->data(0);
            if (dado.isValid()) {
                emit quadraClicada(static_cast<Quadra*>(dado.value<void*>()));
                break;
            }
        }
    }
    QGraphicsView::mousePressEvent(evento);
}
