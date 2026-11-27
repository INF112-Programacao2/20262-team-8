#include "modelos/quadra.hpp"
#include <stdexcept>
#include "estrategias/estrategiaPadrao.hpp"
#include "util/geradorId.hpp"

// Valores usados no calculo (so existem neste arquivo).
const double ADICIONAL_NOTURNO_POR_HORA = 10.0;
const int INICIO_DA_NOITE = 18;

// Quantas horas da reserva caem a partir das 18h.
static int horasNoturnas(int horarioInicio, int duracao) {
    int quantidade = 0;
    for (int hora = horarioInicio; hora < horarioInicio + duracao; hora++) {
        if (hora >= INICIO_DA_NOITE) {
            quantidade++;
        }
    }
    return quantidade;
}

std::string nomeTipoQuadra(TipoQuadra tipo) {
    switch (tipo) {
        case TipoQuadra::Futebol:     return "Futebol";
        case TipoQuadra::Basquete:    return "Basquete";
        case TipoQuadra::Volei:       return "Vôlei";
        case TipoQuadra::Tenis:       return "Tênis";
        case TipoQuadra::BeachTennis: return "Beach tennis";
        case TipoQuadra::Padel:       return "Padel";
        case TipoQuadra::Handebol:    return "Handebol";
    }
    return "Desconhecida";
}

// ----- Quadra ---------------------------------------------------------------
Quadra::Quadra(const std::string& nome, const std::string& localizacao, const TipoQuadra tipos[],
               int qtdTipos, double precoHora, TipoPolitica tipoPolitica,
               EstrategiaPreco* estrategia) {
    id_ = proximoId<Quadra>();
    nome_ = nome;
    localizacao_ = localizacao;
    precoHora_ = precoHora;
    ativa_ = true;
    reclamacoes_ = 0;
    politica_ = criarPolitica(tipoPolitica);
    estrategia_ = estrategia;
    if (estrategia_ == nullptr) {
        estrategia_ = new EstrategiaPadrao();
    }
    qtdTipos_ = 0;
    for (int i = 0; i < qtdTipos && i < MAX_TIPOS; i++) {
        tipos_[i] = tipos[i];
        qtdTipos_++;
    }
}

Quadra::~Quadra() {
    delete politica_;
    delete estrategia_;
}

double Quadra::calcularValorReserva(int duracao) const {
    return precoHora_ * duracao;
}

// A estrategia de preco calcula o valor base; as subclasses somam os adicionais.
double Quadra::calcularValorReserva(int duracao, int horarioInicio) const {
    return estrategia_->calcularPreco(precoHora_, duracao, horarioInicio);
}

std::string Quadra::getCategoria() const {
    return "Simples";
}

int Quadra::getId() const {
    return id_;
}

std::string Quadra::getNome() const {
    return nome_;
}

std::string Quadra::getLocalizacao() const {
    return localizacao_;
}

int Quadra::getQtdTipos() const {
    return qtdTipos_;
}

TipoQuadra Quadra::getTipo(int indice) const {
    if (indice < 0 || indice >= qtdTipos_) {
        throw std::out_of_range("Quadra::getTipo: indice invalido");
    }
    return tipos_[indice];
}

std::string Quadra::tiposComoTexto() const {
    std::string texto = "";
    for (int i = 0; i < qtdTipos_; i++) {
        if (i > 0) {
            texto += ", ";
        }
        texto += nomeTipoQuadra(tipos_[i]);
    }
    return texto;
}

double Quadra::getPrecoHora() const {
    return precoHora_;
}

bool Quadra::isAtiva() const {
    return ativa_;
}

void Quadra::setAtiva(bool ativa) {
    ativa_ = ativa;
}

void Quadra::setEstrategiaPreco(EstrategiaPreco* novaEstrategia) {
    // Se for o mesmo objeto que ja esta em uso, nao pode apagar: ele seria usado depois.
    if (novaEstrategia != nullptr && novaEstrategia != estrategia_) {
        delete estrategia_;  // libera a anterior antes de trocar
        estrategia_ = novaEstrategia;
    }
}

const EstrategiaPreco* Quadra::getEstrategia() const {
    return estrategia_;
}

const PoliticaCancelamento* Quadra::getPolitica() const {
    return politica_;
}

int Quadra::getReclamacoes() const {
    return reclamacoes_;
}

void Quadra::adicionarReclamacao() {
    reclamacoes_++;
}

// ----- Coberta --------------------------------------------------------------
QuadraCoberta::QuadraCoberta(const std::string& nome, const std::string& localizacao,
                             const TipoQuadra tipos[], int qtdTipos, double precoHora,
                             TipoPolitica tipoPolitica, EstrategiaPreco* estrategia)
    : Quadra(nome, localizacao, tipos, qtdTipos, precoHora, tipoPolitica, estrategia) {}

double QuadraCoberta::calcularValorReserva(int duracao) const {
    return Quadra::calcularValorReserva(duracao) * 1.15;
}

double QuadraCoberta::calcularValorReserva(int duracao, int horarioInicio) const {
    return Quadra::calcularValorReserva(duracao, horarioInicio) * 1.15;
}

std::string QuadraCoberta::getCategoria() const {
    return "Coberta (+15%)";
}

// ----- Iluminada ------------------------------------------------------------
QuadraIluminada::QuadraIluminada(const std::string& nome, const std::string& localizacao,
                                 const TipoQuadra tipos[], int qtdTipos, double precoHora,
                                 TipoPolitica tipoPolitica, EstrategiaPreco* estrategia)
    : Quadra(nome, localizacao, tipos, qtdTipos, precoHora, tipoPolitica, estrategia) {}

double QuadraIluminada::calcularValorReserva(int duracao) const {
    return Quadra::calcularValorReserva(duracao);
}

double QuadraIluminada::calcularValorReserva(int duracao, int horarioInicio) const {
    return Quadra::calcularValorReserva(duracao, horarioInicio) +
           ADICIONAL_NOTURNO_POR_HORA * horasNoturnas(horarioInicio, duracao);
}

std::string QuadraIluminada::getCategoria() const {
    return "Iluminada (+R$ 10/h após 18h)";
}

// ----- Premium --------------------------------------------------------------
QuadraPremium::QuadraPremium(const std::string& nome, const std::string& localizacao,
                             const TipoQuadra tipos[], int qtdTipos, double precoHora,
                             TipoPolitica tipoPolitica, EstrategiaPreco* estrategia)
    : QuadraCoberta(nome, localizacao, tipos, qtdTipos, precoHora, tipoPolitica, estrategia) {}

double QuadraPremium::calcularValorReserva(int duracao) const {
    return QuadraCoberta::calcularValorReserva(duracao);
}

double QuadraPremium::calcularValorReserva(int duracao, int horarioInicio) const {
    return QuadraCoberta::calcularValorReserva(duracao, horarioInicio) +
           ADICIONAL_NOTURNO_POR_HORA * horasNoturnas(horarioInicio, duracao);
}

std::string QuadraPremium::getCategoria() const {
    return "Premium (coberta + iluminada)";
}
