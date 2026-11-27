#ifndef PROCESSADOR_LOTE_HPP
#define PROCESSADOR_LOTE_HPP

#include <string>
#include "sistema/sistemagestao.hpp"

/**
 * @brief Modo headless (em lote): executa os comandos de um arquivo de texto e gera um
 *        arquivo de saida com o log de cada comando e um relatorio final.
 *
 * Nao usa a interface grafica: fala so com o SistemaGestao (a mesma fachada da GUI).
 * Um comando por linha, campos separados por ";". Linhas vazias e que comecam com "#"
 * sao ignoradas. Os comandos estao descritos no README.md e em exemplos/entrada.txt.
 */
class ProcessadorLote {
public:
    /** @param sistema sistema que vai receber os comandos (precisa existir durante o uso). */
    explicit ProcessadorLote(SistemaGestao& sistema);

    /**
     * @brief Le o arquivo de entrada, executa cada comando e grava o arquivo de saida.
     * @return false se nao conseguiu abrir/gravar os arquivos (veja getErro()).
     *         Erros dentro dos comandos NAO param o processamento: ficam no log.
     */
    bool processar(const std::string& arquivoEntrada, const std::string& arquivoSaida);

    /** Mensagem do ultimo problema com os arquivos. */
    std::string getErro() const;
    int getQtdComandos() const;
    int getQtdErros() const;

private:
    // Nao pode ser copiado (guarda referencia para o sistema).
    ProcessadorLote(const ProcessadorLote&);
    ProcessadorLote& operator=(const ProcessadorLote&);

    // Executa uma linha. Devolve true se deu certo; "resultado" recebe o texto do log.
    bool executarComando(const std::string& linha, std::string& resultado);
    // Texto do relatorio final (ocupacao por quadra, clientes, conflitos).
    std::string gerarRelatorio() const;

    // Apoio para ler os campos da linha.
    static int dividir(const std::string& texto, char separador, std::string campos[], int maximo);
    static std::string limpar(const std::string& texto);
    static std::string maiusculas(const std::string& texto);
    static bool lerInteiro(const std::string& texto, int& valor);
    static bool lerDecimal(const std::string& texto, double& valor);
    static bool lerCategoria(const std::string& texto, Categoria& categoria);
    static bool lerPolitica(const std::string& texto, TipoPolitica& politica);
    static bool lerEstrategia(const std::string& texto, TipoEstrategia& estrategia);
    static bool lerModalidade(const std::string& texto, TipoQuadra& tipo);
    static std::string dinheiro(double valor);
    static std::string decimal(double valor);

    SistemaGestao& sistema_;
    std::string erro_;
    int qtdComandos_;
    int qtdErros_;
    int qtdConflitos_;  // reservas recusadas por horario ocupado ou manutencao
};

#endif
