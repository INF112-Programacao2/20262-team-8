#ifndef QUADRA_HPP
#define QUADRA_HPP

#include "../enums/TipoQuadra.hpp"
#include <string>
#include <vector>

// Classe base. Uma quadra "simples" cobra apenas preco por hora.
class Quadra {
protected:
    int id;
    std::string nome;
    std::string localizacao;
    std::vector<TipoQuadra> tipos;
    double precoHora;
    bool ativa;
public:
    Quadra(const std::string& nome, const std::string& localizacao,
           const std::vector<TipoQuadra>& tipos, double precoHora);
    virtual ~Quadra() = default;  // obrigatorio quando ha heranca

    int getId() const;
    std::string getNome() const;
    std::string getLocalizacao() const;
    std::vector<TipoQuadra> getTipos() const;
    double getPrecoHora() const;
    bool isAtiva() const;

    // cada tipo de quadra calcula do seu jeito.
    virtual std::string getCategoria() const;
    virtual double calcularValorReserva(int duracao, int horarioInicio) const;

    // SOBRECARGA: versao simples, sem considerar o horario.
    double calcularValorReserva(int duracao) const;
};

// Quadra coberta: adicional de 15% sobre o valor total.
class QuadraCoberta : public Quadra {
public:
    using Quadra::Quadra;                     // reaproveita o construtor da base
    using Quadra::calcularValorReserva;       // mantem a sobrecarga visivel
    std::string getCategoria() const override;
    double calcularValorReserva(int duracao, int horarioInicio) const override;
};

// Quadra iluminada: R$ 10 a mais por hora jogada a partir das 18h.
class QuadraIluminada : public Quadra {
public:
    using Quadra::Quadra;
    using Quadra::calcularValorReserva;
    std::string getCategoria() const override;
    double calcularValorReserva(int duracao, int horarioInicio) const override;
};

// Quadra premium: coberta + iluminada (heranca em cadeia).
class QuadraPremium : public QuadraCoberta {
public:
    using QuadraCoberta::QuadraCoberta;
    using QuadraCoberta::calcularValorReserva;
    std::string getCategoria() const override;
    double calcularValorReserva(int duracao, int horarioInicio) const override;
};

#endif
