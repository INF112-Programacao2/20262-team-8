#include "Sistema.hpp"

#include <cmath>
#include <stdexcept>

namespace {
bool vazio(const std::string& texto) {
    return texto.find_first_not_of(" \t\r\n") == std::string::npos;
}
}

Cliente* Sistema::cadastrarCliente(const std::string& nome, const std::string& email) {
    if (vazio(nome)) {
        throw std::invalid_argument("O nome não pode ficar vazio.");
    }
    const auto arroba = email.find('@');
    if (arroba == std::string::npos || arroba == 0 || arroba + 1 == email.size()) {
        throw std::invalid_argument("Informe um e-mail válido.");
    }
    clientes_.push_back(std::make_unique<Cliente>(nome, email));
    return clientes_.back().get();
}

Quadra* Sistema::cadastrarQuadra(const std::string& nome, const std::string& localizacao,
                                 const std::vector<TipoQuadra>& tipos, double precoHora) {
    if (vazio(nome)) {
        throw std::invalid_argument("O nome da quadra não pode ficar vazio.");
    }
    if (vazio(localizacao)) {
        throw std::invalid_argument("A localização não pode ficar vazia.");
    }
    if (tipos.empty()) {
        throw std::invalid_argument("Selecione ao menos uma modalidade.");
    }
    if (!std::isfinite(precoHora) || precoHora <= 0) {
        throw std::invalid_argument("Informe um preço por hora maior que zero.");
    }
    // O construtor de Quadra recebe referências não-const, então passamos cópias.
    std::string n = nome;
    std::string l = localizacao;
    std::vector<TipoQuadra> t = tipos;
    quadras_.push_back(std::make_unique<Quadra>(n, l, t, precoHora));
    return quadras_.back().get();
}

bool Sistema::temConflito(const Quadra* quadra, const std::string& data,
                          int inicio, int duracao) const {
    for (const Reserva* r : Reserva::listarReservasAtivas()) {
        if (r->getQuadra() == quadra && r->getDataReserva() == data &&
            inicio < r->getHorarioInicio() + r->getDuracaoHoras() &&
            r->getHorarioInicio() < inicio + duracao) {
            return true;
        }
    }
    return false;
}

Reserva* Sistema::criarReserva(Cliente* cliente, Quadra* quadra, const std::string& data,
                               int inicio, int duracao) {
    if (cliente == nullptr || quadra == nullptr) {
        throw std::invalid_argument("Selecione um cliente e uma quadra.");
    }
    if (vazio(data)) {
        throw std::invalid_argument("Informe a data da reserva.");
    }
    if (temConflito(quadra, data, inicio, duracao)) {
        throw std::runtime_error("A quadra já possui uma reserva nesse horário.");
    }
    // O próprio Reserva valida horários e se registra na lista de ativas e no
    // cliente (que passa a ser o dono do objeto) — igual ao que o CLI faz.
    return new Reserva(cliente, quadra, data, inicio, duracao);
}

void Sistema::cancelarReserva(Reserva* reserva) {
    if (reserva != nullptr) {
        reserva->cancelarReserva();
    }
}

Reserva* Sistema::reservaEm(const Quadra* quadra, const std::string& data, int hora) const {
    for (Reserva* r : Reserva::listarReservasAtivas()) {
        if (r->getQuadra() == quadra && r->getDataReserva() == data &&
            hora >= r->getHorarioInicio() &&
            hora < r->getHorarioInicio() + r->getDuracaoHoras()) {
            return r;
        }
    }
    return nullptr;
}

std::string Sistema::nomeTipo(TipoQuadra tipo) {
    switch (tipo) {
        case FUTEBOL: return "Futebol";
        case BASQUETE: return "Basquete";
        case VOLEI: return "Vôlei";
        case TENIS: return "Tênis";
        case BEACH_TENNIS: return "Beach tennis";
        case PADEL: return "Padel";
        case HANDEBOL: return "Handebol";
    }
    return "Desconhecida";
}

const std::vector<TipoQuadra>& Sistema::todosOsTipos() {
    static const std::vector<TipoQuadra> tipos = {
        FUTEBOL, BASQUETE, VOLEI, TENIS, BEACH_TENNIS, PADEL, HANDEBOL
    };
    return tipos;
}
