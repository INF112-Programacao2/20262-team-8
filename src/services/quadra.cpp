#include "models/quadra.hpp"
#include "estrategias/estrategiaPadrao.hpp"

#include <utility>

static int gerarId() {
    static int id = 0;
    return ++id;
}

Quadra::Quadra(const std::string& nome, const std::string& local,
               const std::vector<TipoQuadra>& tipos, double preco,
               std::unique_ptr<EstrategiaPreco> estrategiaPreco,
               std::unique_ptr<PoliticaCancelamento> politicaCancelamento)
    : id(gerarId()), nome(nome), localizacao(local), tipos(tipos),
      precoHora(preco), ativa(true), estrategiaPreco(std::move(estrategiaPreco)),
      politicaCancelamento(std::move(politicaCancelamento)) {
    if (!this->estrategiaPreco) {
        this->estrategiaPreco = std::make_unique<EstrategiaPadrao>();
    }
    if (!this->politicaCancelamento) {
        this->politicaCancelamento = criarPolitica(TipoPolitica::Flexivel);
    }
}

Quadra::~Quadra() = default;

std::string Quadra::getCategoria() const {
    return "Simples";
}

double Quadra::calcularValorReserva(int duracao, int horarioInicio) const {
    return estrategiaPreco->calcularPreco(precoHora, duracao, horarioInicio);
}

double Quadra::calcularValorReserva(int duracao) const {
    return calcularValorReserva(duracao, 0);
}

std::string QuadraCoberta::getCategoria() const {
    return "Coberta";
}

double QuadraCoberta::calcularValorReserva(int duracao, int horarioInicio) const {
    return Quadra::calcularValorReserva(duracao, horarioInicio) * 1.15;
}

std::string QuadraIluminada::getCategoria() const {
    return "Iluminada";
}

double QuadraIluminada::calcularValorReserva(int duracao, int horarioInicio) const {
    const double valorBase = Quadra::calcularValorReserva(duracao, horarioInicio);
    int horasIluminadas = 0;
    for (int hora = horarioInicio; hora < horarioInicio + duracao; ++hora) {
        if (hora >= 18) {
            ++horasIluminadas;
        }
    }
    return valorBase + 10.0 * horasIluminadas;
}

std::string QuadraPremium::getCategoria() const {
    return "Premium";
}

double QuadraPremium::calcularValorReserva(int duracao, int horarioInicio) const {
    const double valorCoberto = QuadraCoberta::calcularValorReserva(duracao, horarioInicio);
    int horasIluminadas = 0;
    for (int hora = horarioInicio; hora < horarioInicio + duracao; ++hora) {
        if (hora >= 18) {
            ++horasIluminadas;
        }
    }
    return valorCoberto + 10.0 * horasIluminadas;
}
int Quadra::getId() const {
    return id;
}

std::string Quadra::getNome() const {
    return nome;
}

std::string Quadra::getLocalizacao() const {
    return localizacao;
}

std::vector<TipoQuadra> Quadra::getTipos() const {
    return tipos;
}

double Quadra::getPrecoHora() const {
    return precoHora;
}

bool Quadra::isAtiva() const {
    return ativa;
}

std::string Quadra::getNomeEstrategiaPreco() const {
    return estrategiaPreco->getNome();
}

const PoliticaCancelamento* Quadra::getPoliticaCancelamento() const {
    return politicaCancelamento.get();
}
