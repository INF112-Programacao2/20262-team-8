#ifndef RELOGIO_HPP
#define RELOGIO_HPP

#include <string>

// Data e hora de "agora". O sistema nao le o relogio do computador:
// quem define o dia e a hora e o usuario (isso permite testar).
class Relogio {
public:
    Relogio();

    bool definirData(const std::string& data);  // AAAA-MM-DD; false se for invalida
    std::string getData() const;
    bool definirHora(int hora);  // 0 a 23; false se for invalida
    int getHora() const;

private:
    std::string data_;
    int hora_;
};

#endif
