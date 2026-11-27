#ifndef POLITICA_CANCELAMENTO_HPP
#define POLITICA_CANCELAMENTO_HPP

#include <memory>
#include <string>
#include "enums/TipoPolitica.hpp"

// Politica de cancelamento (classe abstrata): cada quadra tem uma.
// Quanto mais cedo o cliente cancela, maior o reembolso. Cada subclasse
// decide a sua regra (polimorfismo, como o calculo de valor das quadras).
class PoliticaCancelamento {
public:
    virtual ~PoliticaCancelamento() {}

    // Quantos % do valor sao devolvidos, dado quantos dias faltam para a reserva.
    virtual int percentualReembolso(int diasAntes) const = 0;
    virtual std::string getNome() const = 0;
    virtual std::string getDescricao() const = 0;
};

std::unique_ptr<PoliticaCancelamento> criarPolitica(TipoPolitica tipo);

#endif
