#ifndef QUADRA_HPP
#define QUADRA_HPP

#include <string>
#include "enums/TipoQuadra.hpp"
#include "enums/TipoPolitica.hpp"
#include "politicas/politicaCancelamento.hpp"
#include "estrategias/estrategiaPreco.hpp"

std::string nomeTipoQuadra(TipoQuadra tipo);

// ---------------------------------------------------------------------------
// Quadra (base): categoria "Simples", sem acrescimos.
//
// Existem duas formas de calcular o preco:
//  - as subclasses (Coberta, Iluminada, Premium) somam os seus adicionais;
//  - a EstrategiaPreco (Padrao, Mensalista, Pico) calcula o valor base.
// A estrategia padrao nao altera nada (preco/hora x duracao).
// ---------------------------------------------------------------------------
class Quadra {
public:
    static const int MAX_TIPOS = 7;

    // O id e gerado automaticamente. Se nao passar estrategia, usa a Padrao.
    // A quadra passa a ser dona da estrategia (ela da o delete).
    Quadra(const std::string& nome, const std::string& localizacao, const TipoQuadra tipos[],
           int qtdTipos, double precoHora, TipoPolitica tipoPolitica = TipoPolitica::Flexivel,
           EstrategiaPreco* estrategia = nullptr);
    virtual ~Quadra();  // obrigatorio quando ha heranca

    // Sobrecarga: sem horario (nao sabe se ha iluminacao noturna) e com horario.
    // A versao sem horario e so uma estimativa de dia: usa o preco/hora (e o adicional
    // da categoria), sem estrategia e sem adicional noturno. O sistema usa a com horario.
    virtual double calcularValorReserva(int duracao) const;
    virtual double calcularValorReserva(int duracao, int horarioInicio) const;
    virtual std::string getCategoria() const;

    int getId() const;
    std::string getNome() const;
    std::string getLocalizacao() const;
    int getQtdTipos() const;
    TipoQuadra getTipo(int indice) const;  // lanca std::out_of_range se o indice for invalido
    std::string tiposComoTexto() const;
    double getPrecoHora() const;
    bool isAtiva() const;
    void setAtiva(bool ativa);

    // Estrategia de preco (troca a anterior e libera a memoria dela).
    // nullptr, ou a mesma estrategia que ja esta em uso, nao muda nada.
    void setEstrategiaPreco(EstrategiaPreco* novaEstrategia);
    const EstrategiaPreco* getEstrategia() const;

    // Politica de cancelamento (cada quadra tem a sua)
    const PoliticaCancelamento* getPolitica() const;

    // Reclamacoes recebidas (avaliacoes com nota 1 ou 2)
    int getReclamacoes() const;
    void adicionarReclamacao();

private:
    // A quadra e dona da politica e da estrategia (criadas com new),
    // por isso nao pode ser copiada.
    Quadra(const Quadra&);
    Quadra& operator=(const Quadra&);

    int id_;
    std::string nome_;
    std::string localizacao_;
    TipoQuadra tipos_[MAX_TIPOS];
    int qtdTipos_;
    double precoHora_;
    bool ativa_;
    PoliticaCancelamento* politica_;
    EstrategiaPreco* estrategia_;
    int reclamacoes_;
};

// ---------------------------------------------------------------------------
// Coberta: +15% no valor.
// ---------------------------------------------------------------------------
class QuadraCoberta : public Quadra {
public:
    QuadraCoberta(const std::string& nome, const std::string& localizacao,
                  const TipoQuadra tipos[], int qtdTipos, double precoHora,
                  TipoPolitica tipoPolitica = TipoPolitica::Flexivel,
                  EstrategiaPreco* estrategia = nullptr);
    double calcularValorReserva(int duracao) const override;
    double calcularValorReserva(int duracao, int horarioInicio) const override;
    std::string getCategoria() const override;
};

// ---------------------------------------------------------------------------
// Iluminada: +R$ 10 por hora para as horas a partir das 18h.
// ---------------------------------------------------------------------------
class QuadraIluminada : public Quadra {
public:
    QuadraIluminada(const std::string& nome, const std::string& localizacao,
                    const TipoQuadra tipos[], int qtdTipos, double precoHora,
                    TipoPolitica tipoPolitica = TipoPolitica::Flexivel,
                    EstrategiaPreco* estrategia = nullptr);
    double calcularValorReserva(int duracao) const override;
    double calcularValorReserva(int duracao, int horarioInicio) const override;
    std::string getCategoria() const override;
};

// ---------------------------------------------------------------------------
// Premium: coberta (+15%) + iluminada (+R$ 10/h apos 18h).
// ---------------------------------------------------------------------------
class QuadraPremium : public QuadraCoberta {
public:
    QuadraPremium(const std::string& nome, const std::string& localizacao,
                  const TipoQuadra tipos[], int qtdTipos, double precoHora,
                  TipoPolitica tipoPolitica = TipoPolitica::Flexivel,
                  EstrategiaPreco* estrategia = nullptr);
    double calcularValorReserva(int duracao) const override;
    double calcularValorReserva(int duracao, int horarioInicio) const override;
    std::string getCategoria() const override;
};

#endif
