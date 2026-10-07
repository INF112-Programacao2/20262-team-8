#ifndef UTIL_HPP
#define UTIL_HPP

#include "../../models/reserva.hpp"

#include <QDate>
#include <QLocale>
#include <QString>

#include <string>

// Conversões entre os tipos do núcleo (std::string, int) e os da interface (Qt).
// A data é guardada como "AAAA-MM-DD", o mesmo formato sugerido pelo CLI.
namespace util {

inline QString texto(const std::string& s) { return QString::fromStdString(s); }

inline std::string dataIso(const QDate& d) { return d.toString(Qt::ISODate).toStdString(); }

inline QString dataBr(const std::string& iso) {
    const QDate d = QDate::fromString(texto(iso), Qt::ISODate);
    return d.isValid() ? d.toString("dd/MM/yyyy") : texto(iso);
}

inline QString hora(int h) { return QString("%1:00").arg(h, 2, 10, QLatin1Char('0')); }

inline QString dinheiro(double valor) {
    return QLocale(QLocale::Portuguese, QLocale::Brazil).toCurrencyString(valor);
}

inline QString descricao(const Reserva& r) {
    return QString("Cliente: %1\nQuadra: %2\nData: %3\nHorário: %4 às %5\nValor: %6")
        .arg(texto(r.getCliente()->getNome()),
             texto(r.getQuadra()->getNome()),
             dataBr(r.getDataReserva()),
             hora(r.getHorarioInicio()),
             hora(r.getHorarioInicio() + r.getDuracaoHoras()),
             dinheiro(r.getValorTotal()));
}

}  // namespace util

#endif
