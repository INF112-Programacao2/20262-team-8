#include "util/data.hpp"

static int anoDe(const std::string& data) {
    return (data[0] - '0') * 1000 + (data[1] - '0') * 100 + (data[2] - '0') * 10 + (data[3] - '0');
}
static int mesDe(const std::string& data) {
    return (data[5] - '0') * 10 + (data[6] - '0');
}
static int diaDe(const std::string& data) {
    return (data[8] - '0') * 10 + (data[9] - '0');
}
static bool anoBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

// Transforma uma data em "numero do dia" (quantos dias desde o ano 1),
// para podermos subtrair duas datas.
static int numeroDoDia(const std::string& data) {
    int ano = anoDe(data);
    int mes = mesDe(data);
    int dia = diaDe(data);

    int diasDoMes[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (anoBissexto(ano)) {
        diasDoMes[1] = 29;
    }

    int anosAnteriores = ano - 1;
    int total = anosAnteriores * 365 + anosAnteriores / 4 - anosAnteriores / 100 +
                anosAnteriores / 400;
    for (int m = 0; m < mes - 1; m++) {
        total += diasDoMes[m];
    }
    return total + dia;
}

bool Data::valida(const std::string& data) {
    // Formato esperado: AAAA-MM-DD (10 caracteres, com '-' nas posicoes 4 e 7).
    if (data.size() != 10 || data[4] != '-' || data[7] != '-') {
        return false;
    }
    for (int i = 0; i < 10; i++) {
        if (i != 4 && i != 7 && (data[i] < '0' || data[i] > '9')) {
            return false;
        }
    }

    int ano = anoDe(data);
    int mes = mesDe(data);
    int dia = diaDe(data);
    if (ano < 1 || mes < 1 || mes > 12 || dia < 1) {
        return false;
    }

    int diasDoMes[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maximo = diasDoMes[mes - 1];
    if (mes == 2 && anoBissexto(ano)) {
        maximo = 29;
    }
    return dia <= maximo;
}

int Data::diferencaEmDias(const std::string& de, const std::string& ate) {
    return numeroDoDia(ate) - numeroDoDia(de);
}
