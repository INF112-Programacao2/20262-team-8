#include "sistema/mensagens.hpp"

Mensagens::Mensagens() {
    erro_ = "";
    aviso_ = "";
}

void Mensagens::limpar() {
    erro_ = "";
    aviso_ = "";
}

void Mensagens::erro(const std::string& texto) {
    erro_ = texto;
}

void Mensagens::aviso(const std::string& texto) {
    aviso_ = texto;
}

void Mensagens::acrescentarAviso(const std::string& texto) {
    aviso_ += texto;
}

std::string Mensagens::getErro() const {
    return erro_;
}

std::string Mensagens::getAviso() const {
    return aviso_;
}
