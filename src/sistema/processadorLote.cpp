#include "sistema/processadorLote.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

ProcessadorLote::ProcessadorLote(SistemaGestao& sistema)
    : sistema_(sistema), qtdComandos_(0), qtdErros_(0), qtdConflitos_(0) {}

std::string ProcessadorLote::getErro() const { return erro_; }
int ProcessadorLote::getQtdComandos() const { return qtdComandos_; }
int ProcessadorLote::getQtdErros() const { return qtdErros_; }

// ------------------------------------------------------------- apoio de texto ----

std::string ProcessadorLote::limpar(const std::string& texto) {
    int inicio = 0;
    int fim = static_cast<int>(texto.size());
    while (inicio < fim && (texto[inicio] == ' ' || texto[inicio] == '\t' || texto[inicio] == '\r')) {
        inicio++;
    }
    while (fim > inicio && (texto[fim - 1] == ' ' || texto[fim - 1] == '\t' || texto[fim - 1] == '\r')) {
        fim--;
    }
    return texto.substr(inicio, fim - inicio);
}

std::string ProcessadorLote::maiusculas(const std::string& texto) {
    std::string r = texto;
    for (std::string::size_type i = 0; i < r.size(); i++) {
        if (r[i] >= 'a' && r[i] <= 'z') {
            r[i] = static_cast<char>(r[i] - 'a' + 'A');
        }
    }
    return r;
}

// Separa o texto pelo separador (campos ja sem espacos nas pontas). Devolve a quantidade.
// O ultimo campo guarda o resto da linha, assim um comentario pode ter ";".
int ProcessadorLote::dividir(const std::string& texto, char separador, std::string campos[],
                             int maximo) {
    int qtd = 0;
    std::string::size_type inicio = 0;
    while (qtd < maximo - 1) {
        std::string::size_type pos = texto.find(separador, inicio);
        if (pos == std::string::npos) {
            break;
        }
        campos[qtd] = limpar(texto.substr(inicio, pos - inicio));
        qtd++;
        inicio = pos + 1;
    }
    campos[qtd] = limpar(texto.substr(inicio));
    qtd++;
    return qtd;
}

bool ProcessadorLote::lerInteiro(const std::string& texto, int& valor) {
    try {
        std::string::size_type usados = 0;
        int v = std::stoi(texto, &usados);
        if (usados != texto.size()) {
            return false;
        }
        valor = v;
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

bool ProcessadorLote::lerDecimal(const std::string& texto, double& valor) {
    // aceita "80,5" ou "80.5"
    std::string t = texto;
    for (std::string::size_type i = 0; i < t.size(); i++) {
        if (t[i] == ',') {
            t[i] = '.';
        }
    }
    try {
        std::string::size_type usados = 0;
        double v = std::stod(t, &usados);
        if (usados != t.size()) {
            return false;
        }
        valor = v;
        return true;
    } catch (const std::invalid_argument&) {
        return false;
    } catch (const std::out_of_range&) {
        return false;
    }
}

bool ProcessadorLote::lerCategoria(const std::string& texto, Categoria& categoria) {
    std::string t = maiusculas(texto);
    if (t == "SIMPLES") categoria = Categoria::Simples;
    else if (t == "COBERTA") categoria = Categoria::Coberta;
    else if (t == "ILUMINADA") categoria = Categoria::Iluminada;
    else if (t == "PREMIUM") categoria = Categoria::Premium;
    else return false;
    return true;
}

bool ProcessadorLote::lerPolitica(const std::string& texto, TipoPolitica& politica) {
    std::string t = maiusculas(texto);
    if (t == "FLEXIVEL") politica = TipoPolitica::Flexivel;
    else if (t == "MODERADA") politica = TipoPolitica::Moderada;
    else if (t == "RIGIDA") politica = TipoPolitica::Rigida;
    else return false;
    return true;
}

bool ProcessadorLote::lerEstrategia(const std::string& texto, TipoEstrategia& estrategia) {
    std::string t = maiusculas(texto);
    if (t == "PADRAO") estrategia = TipoEstrategia::Padrao;
    else if (t == "MENSALISTA") estrategia = TipoEstrategia::Mensalista;
    else if (t == "PICO") estrategia = TipoEstrategia::Pico;
    else return false;
    return true;
}

bool ProcessadorLote::lerModalidade(const std::string& texto, TipoQuadra& tipo) {
    std::string t = maiusculas(texto);
    if (t == "FUTEBOL") tipo = TipoQuadra::Futebol;
    else if (t == "BASQUETE") tipo = TipoQuadra::Basquete;
    else if (t == "VOLEI") tipo = TipoQuadra::Volei;
    else if (t == "TENIS") tipo = TipoQuadra::Tenis;
    else if (t == "BEACHTENNIS") tipo = TipoQuadra::BeachTennis;
    else if (t == "PADEL") tipo = TipoQuadra::Padel;
    else if (t == "HANDEBOL") tipo = TipoQuadra::Handebol;
    else return false;
    return true;
}

// 12.5 -> "12,50"
std::string ProcessadorLote::decimal(double valor) {
    long centavos = static_cast<long>(valor * 100.0 + 0.5);
    long reais = centavos / 100;
    long resto = centavos % 100;
    std::string r = std::to_string(reais) + ",";
    if (resto < 10) {
        r += "0";
    }
    return r + std::to_string(resto);
}

std::string ProcessadorLote::dinheiro(double valor) { return "R$ " + decimal(valor); }

// ------------------------------------------------------------------- comandos ----

bool ProcessadorLote::executarComando(const std::string& linha, std::string& resultado) {
    const int MAX_CAMPOS = 8;
    std::string c[MAX_CAMPOS];
    int n = dividir(linha, ';', c, MAX_CAMPOS);
    std::string comando = maiusculas(c[0]);
    int id = 0;
    bool ok = true;

    if (comando == "HOJE") {
        if (n < 2) { resultado = "Uso: HOJE;AAAA-MM-DD"; return false; }
        ok = sistema_.definirDataHoje(c[1]);
        resultado = ok ? "Data de hoje: " + c[1] : sistema_.getUltimoErro();
    } else if (comando == "HORA") {
        int hora = 0;
        if (n < 2 || !lerInteiro(c[1], hora)) { resultado = "Uso: HORA;0-23"; return false; }
        ok = sistema_.definirHoraAgora(hora);
        resultado = ok ? "Hora de agora: " + std::to_string(hora) + "h" : sistema_.getUltimoErro();
    } else if (comando == "CLIENTE") {
        if (n < 3) { resultado = "Uso: CLIENTE;nome;email"; return false; }
        id = sistema_.cadastrarCliente(c[1], c[2]);
        ok = id >= 0;
        resultado = ok ? "Cliente cadastrado (id=" + std::to_string(id) + ")" : sistema_.getUltimoErro();
    } else if (comando == "QUADRA") {
        // QUADRA;nome;local;precoHora;categoria;modalidade1,modalidade2;politica(opcional)
        double preco = 0;
        Categoria categoria = Categoria::Simples;
        TipoPolitica politica = TipoPolitica::Flexivel;
        if (n < 6 || !lerDecimal(c[3], preco) || !lerCategoria(c[4], categoria)) {
            resultado = "Uso: QUADRA;nome;local;precoHora;categoria;modalidades;politica";
            return false;
        }
        if (n >= 7 && !lerPolitica(c[6], politica)) {
            resultado = "Politica desconhecida: " + c[6];
            return false;
        }
        std::string nomes[Quadra::MAX_TIPOS];
        TipoQuadra tipos[Quadra::MAX_TIPOS];
        int qtdTipos = dividir(c[5], ',', nomes, Quadra::MAX_TIPOS);
        for (int i = 0; i < qtdTipos; i++) {
            if (!lerModalidade(nomes[i], tipos[i])) {
                resultado = "Modalidade desconhecida: " + nomes[i];
                return false;
            }
        }
        id = sistema_.cadastrarQuadra(c[1], c[2], preco, categoria, tipos, qtdTipos, politica);
        ok = id >= 0;
        resultado = ok ? "Quadra cadastrada (id=" + std::to_string(id) + ")" : sistema_.getUltimoErro();
    } else if (comando == "ESTRATEGIA") {
        TipoEstrategia tipo = TipoEstrategia::Padrao;
        if (n < 3 || !lerInteiro(c[1], id) || !lerEstrategia(c[2], tipo)) {
            resultado = "Uso: ESTRATEGIA;idQuadra;PADRAO|MENSALISTA|PICO";
            return false;
        }
        ok = sistema_.definirEstrategiaQuadra(id, tipo);
        resultado = ok ? "Estrategia da quadra " + c[1] + ": " + maiusculas(c[2]) : sistema_.getUltimoErro();
    } else if (comando == "ATIVAR") {
        if (n < 2 || !lerInteiro(c[1], id)) { resultado = "Uso: ATIVAR;idQuadra"; return false; }
        ok = sistema_.alternarAtivaQuadra(id);
        const Quadra* q = sistema_.buscarQuadra(id);
        if (ok && q != nullptr) {
            resultado = q->isAtiva() ? "Quadra ativada" : "Quadra desativada";
        } else {
            resultado = sistema_.getUltimoErro();
        }
    } else if (comando == "DESBLOQUEAR") {
        if (n < 2 || !lerInteiro(c[1], id)) { resultado = "Uso: DESBLOQUEAR;idCliente"; return false; }
        ok = sistema_.desbloquearCliente(id);
        resultado = ok ? "Cliente desbloqueado" : sistema_.getUltimoErro();
    } else if (comando == "RESERVA") {
        // RESERVA;idCliente;idQuadra;data;hora;duracao(opcional, padrao 1)
        int cliente = 0, quadra = 0, hora = 0, duracao = 1;
        if (n < 5 || !lerInteiro(c[1], cliente) || !lerInteiro(c[2], quadra) ||
            !lerInteiro(c[4], hora) || (n >= 6 && !lerInteiro(c[5], duracao))) {
            resultado = "Uso: RESERVA;idCliente;idQuadra;data;hora;duracao";
            return false;
        }
        id = sistema_.criarReserva(cliente, quadra, c[3], hora, duracao);
        ok = id >= 0;
        if (ok) {
            const Reserva* r = sistema_.buscarReserva(id);
            resultado = "Reserva confirmada (id=" + std::to_string(id) + ")";
            if (r != nullptr) {
                resultado += ", valor " + dinheiro(r->getValorTotal());
            }
        } else {
            resultado = sistema_.getUltimoErro();
            if (resultado.find("reserva nesse horário") != std::string::npos ||
                resultado.find("manutenção") != std::string::npos) {
                qtdConflitos_++;
            }
        }
    } else if (comando == "CANCELAR") {
        if (n < 2 || !lerInteiro(c[1], id)) { resultado = "Uso: CANCELAR;idReserva"; return false; }
        double reembolso = sistema_.cancelarReserva(id);
        ok = reembolso >= 0;
        resultado = ok ? "Reserva cancelada, reembolso " + dinheiro(reembolso) : sistema_.getUltimoErro();
    } else if (comando == "CONCLUIR") {
        if (n < 2 || !lerInteiro(c[1], id)) { resultado = "Uso: CONCLUIR;idReserva"; return false; }
        ok = sistema_.concluirReserva(id);
        resultado = ok ? "Reserva concluida" : sistema_.getUltimoErro();
    } else if (comando == "FALTA") {
        if (n < 2 || !lerInteiro(c[1], id)) { resultado = "Uso: FALTA;idReserva"; return false; }
        ok = sistema_.registrarFalta(id);
        resultado = ok ? "Falta registrada" : sistema_.getUltimoErro();
    } else if (comando == "AVALIAR") {
        int nota = 0;
        if (n < 3 || !lerInteiro(c[1], id) || !lerInteiro(c[2], nota)) {
            resultado = "Uso: AVALIAR;idReserva;nota;comentario";
            return false;
        }
        std::string comentario = (n >= 4) ? c[3] : "";
        int avaliacao = sistema_.avaliarReserva(id, nota, comentario);
        ok = avaliacao >= 0;
        resultado = ok ? "Avaliacao registrada (id=" + std::to_string(avaliacao) + ")" : sistema_.getUltimoErro();
    } else if (comando == "MANUTENCAO") {
        if (n < 4 || !lerInteiro(c[1], id)) {
            resultado = "Uso: MANUTENCAO;idQuadra;dataInicio;dataFim;motivo";
            return false;
        }
        std::string motivo = (n >= 5) ? c[4] : "";
        int manutencao = sistema_.registrarManutencao(id, c[2], c[3], motivo);
        ok = manutencao >= 0;
        resultado = ok ? "Manutencao registrada (id=" + std::to_string(manutencao) + ")" : sistema_.getUltimoErro();
    } else if (comando == "ENCERRAR_MANUTENCAO") {
        if (n < 2 || !lerInteiro(c[1], id)) { resultado = "Uso: ENCERRAR_MANUTENCAO;idManutencao"; return false; }
        ok = sistema_.encerrarManutencao(id);
        resultado = ok ? "Manutencao encerrada" : sistema_.getUltimoErro();
    } else {
        resultado = "Comando desconhecido: " + c[0];
        return false;
    }

    // Efeitos em outras classes (cupom, reservas canceladas por manutencao...).
    // HOJE e HORA nao geram aviso (o ultimo aviso guardado seria de outro comando).
    bool geraAviso = (comando != "HOJE" && comando != "HORA");
    if (ok && geraAviso && !sistema_.getUltimoAviso().empty()) {
        resultado += " | " + sistema_.getUltimoAviso();
    }
    return ok;
}

// ------------------------------------------------------------------ processar ----

bool ProcessadorLote::processar(const std::string& arquivoEntrada, const std::string& arquivoSaida) {
    std::ifstream entrada(arquivoEntrada.c_str());
    if (!entrada.is_open()) {
        erro_ = "Nao foi possivel abrir o arquivo de entrada: " + arquivoEntrada;
        return false;
    }

    qtdComandos_ = 0;
    qtdErros_ = 0;
    qtdConflitos_ = 0;

    std::string saida = "=== LOG DA SIMULACAO EM LOTE ===\nEntrada: " + arquivoEntrada + "\n\n";
    std::string linha;
    int numeroLinha = 0;
    while (std::getline(entrada, linha)) {
        numeroLinha++;
        linha = limpar(linha);
        if (linha.empty() || linha[0] == '#') {
            continue;
        }
        std::string resultado;
        bool ok = executarComando(linha, resultado);
        qtdComandos_++;
        if (!ok) {
            qtdErros_++;
        }
        saida += "[linha " + std::to_string(numeroLinha) + "] " + linha + "\n";
        saida += std::string(ok ? "    OK:   " : "    ERRO: ") + resultado + "\n";
    }
    entrada.close();

    saida += "\n" + gerarRelatorio();

    std::ofstream arquivo(arquivoSaida.c_str());
    if (!arquivo.is_open()) {
        erro_ = "Nao foi possivel gravar o arquivo de saida: " + arquivoSaida;
        return false;
    }
    arquivo << saida;
    arquivo.close();
    return true;
}

// ------------------------------------------------------------------ relatorio ----

std::string ProcessadorLote::gerarRelatorio() const {
    std::string r = "=== RELATORIO FINAL ===\n";
    r += "Comandos executados: " + std::to_string(qtdComandos_) + "\n";
    r += "Comandos com erro:   " + std::to_string(qtdErros_) + "\n";
    r += "Conflitos (horario ocupado ou quadra em manutencao): " + std::to_string(qtdConflitos_) + "\n";
    r += "Data de hoje no sistema: " + sistema_.getDataHoje() + "\n";

    r += "\n--- Ocupacao por quadra ---\n";
    if (sistema_.getQtdQuadras() == 0) {
        r += "(nenhuma quadra cadastrada)\n";
    }
    for (int i = 0; i < sistema_.getQtdQuadras(); i++) {
        const Quadra* q = sistema_.getQuadra(i);
        int confirmadas = 0, concluidas = 0, canceladas = 0, faltas = 0, horas = 0;
        double receita = 0;
        for (int j = 0; j < sistema_.getQtdReservas(); j++) {
            const Reserva* res = sistema_.getReserva(j);
            if (res->getQuadra()->getId() != q->getId()) {
                continue;
            }
            switch (res->getStatus()) {
                case StatusReserva::Confirmada: confirmadas++; horas += res->getDuracaoHoras(); break;
                case StatusReserva::Concluida:
                    concluidas++;
                    horas += res->getDuracaoHoras();
                    receita += res->getValorTotal();
                    break;
                case StatusReserva::Cancelada: canceladas++; break;
                case StatusReserva::Falta: faltas++; break;
            }
        }
        std::string situacao = q->isAtiva() ? "ativa" : "inativa";
        if (sistema_.quadraEmManutencao(q->getId())) {
            situacao += ", em manutencao";
        }
        r += "Quadra " + std::to_string(q->getId()) + " - " + q->getNome() + " (" + q->getCategoria() +
             ", " + q->getEstrategia()->getNome() + ", " + q->getPolitica()->getNome() + ", " + situacao + ")\n";
        r += "    reservas: " + std::to_string(confirmadas) + " confirmadas, " + std::to_string(concluidas) +
             " concluidas, " + std::to_string(canceladas) + " canceladas, " + std::to_string(faltas) + " faltas\n";
        r += "    horas reservadas: " + std::to_string(horas) + "h | receita das concluidas: " + dinheiro(receita) + "\n";
        int qtdAval = sistema_.contarAvaliacoesDaQuadra(q->getId());
        if (qtdAval > 0) {
            r += "    avaliacao media: " + decimal(sistema_.mediaDaQuadra(q->getId())) + " (" +
                 std::to_string(qtdAval) + " avaliacoes)";
            r += sistema_.quadraEmDestaque(q->getId()) ? " - EM DESTAQUE\n" : "\n";
        } else {
            r += "    sem avaliacoes\n";
        }
    }

    r += "\n--- Clientes ---\n";
    if (sistema_.getQtdClientes() == 0) {
        r += "(nenhum cliente cadastrado)\n";
    }
    for (int i = 0; i < sistema_.getQtdClientes(); i++) {
        const Cliente* c = sistema_.getCliente(i);
        r += "Cliente " + std::to_string(c->getId()) + " - " + c->getNome() + ": " +
             std::to_string(c->getQtdReservas()) + " reservas, " + std::to_string(c->getFaltas()) + " faltas";
        if (c->estaBloqueado()) {
            r += " (BLOQUEADO)";
        }
        if (c->getDescontoProximaReserva() > 0) {
            r += " (cupom de " + std::to_string(c->getDescontoProximaReserva()) + "%)";
        }
        r += "\n";
    }
    return r;
}