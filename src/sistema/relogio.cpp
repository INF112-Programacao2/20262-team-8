#include "sistema/relogio.hpp"
#include "util/data.hpp"

Relogio::Relogio() {
    data_ = "2026-10-31";
    hora_ = 0;
}

bool Relogio::definirData(const std::string& data) {
    if (!Data::valida(data)) {
        return false;
    }
    data_ = data;
    return true;
}

std::string Relogio::getData() const {
    return data_;
}

bool Relogio::definirHora(int hora) {
    if (hora < 0 || hora > 23) {
        return false;
    }
    hora_ = hora;
    return true;
}

int Relogio::getHora() const {
    return hora_;
}
