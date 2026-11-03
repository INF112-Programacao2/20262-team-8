#ifndef ESTRATEGIA_PRECO_HPP
#define ESTRATEGIA_PRECO_HPP

class EstrategiaPreco {
public:
    virtual ~EstrategiaPreco() = default;
    
    // Recebe o preço base/hora, a duração, o horário de início e o dia/data
    virtual double calcularPreco(double precoBaseHora, int duracao, int horarioInicio) const = 0;
};

#endif