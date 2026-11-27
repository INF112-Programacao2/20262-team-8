#include "sistema/sistemagestao.hpp"
#include "util/data.hpp"

// Monta os servicos e liga um no outro. O ServicoReservas conhece a manutencao so pela
// interface ConsultaManutencao (por isso ela e ligada depois que os dois existem).
SistemaGestao::SistemaGestao()
    : clientes_(mensagens_),
      quadras_(mensagens_),
      reservas_(mensagens_, relogio_, clientes_, quadras_),
      manutencoes_(mensagens_, relogio_, quadras_, reservas_),
      avaliacoes_(mensagens_, reservas_) {
    reservas_.definirConsultaManutencao(&manutencoes_);
}

// ===================================================== data de hoje ====
bool SistemaGestao::definirDataHoje(const std::string& data) {
    if (!relogio_.definirData(data)) {
        mensagens_.erro("Data de hoje inválida. Use o formato AAAA-MM-DD.");
        return false;
    }
    return true;
}

std::string SistemaGestao::getDataHoje() const {
    return relogio_.getData();
}

bool SistemaGestao::definirHoraAgora(int hora) {
    if (!relogio_.definirHora(hora)) {
        mensagens_.erro("Hora inválida (0 a 23).");
        return false;
    }
    return true;
}

int SistemaGestao::getHoraAgora() const {
    return relogio_.getHora();
}

// ============================================================ cadastros ====
int SistemaGestao::cadastrarCliente(const std::string& nome, const std::string& email) {
    return clientes_.cadastrar(nome, email);
}

int SistemaGestao::cadastrarQuadra(const std::string& nome, const std::string& localizacao,
                                   double precoHora, Categoria categoria,
                                   const TipoQuadra tipos[], int qtdTipos,
                                   TipoPolitica politica) {
    return quadras_.cadastrar(nome, localizacao, precoHora, categoria, tipos, qtdTipos, politica);
}

bool SistemaGestao::alternarAtivaQuadra(int quadraId) {
    return quadras_.alternarAtiva(quadraId);
}

bool SistemaGestao::definirEstrategiaQuadra(int quadraId, TipoEstrategia tipo) {
    return quadras_.definirEstrategia(quadraId, tipo);
}

bool SistemaGestao::desbloquearCliente(int clienteId) {
    return clientes_.desbloquear(clienteId);
}

// ============================================================ reservas ====
int SistemaGestao::criarReserva(int clienteId, int quadraId, const std::string& data,
                                int horarioInicio) {
    return reservas_.criar(clienteId, quadraId, data, horarioInicio);
}

int SistemaGestao::criarReserva(int clienteId, int quadraId, const std::string& data,
                                int horarioInicio, int duracao) {
    return reservas_.criar(clienteId, quadraId, data, horarioInicio, duracao);
}

Reserva* SistemaGestao::criarReserva(Cliente& cliente, Quadra& quadra, const std::string& data,
                                     int horarioInicio, int duracaoHoras) {
    return reservas_.criar(cliente, quadra, data, horarioInicio, duracaoHoras);
}

double SistemaGestao::cancelarReserva(int reservaId) {
    return reservas_.cancelar(reservaId);
}

bool SistemaGestao::cancelarReserva(Reserva& reserva) {
    return reservas_.cancelar(reserva);
}

bool SistemaGestao::concluirReserva(int reservaId) {
    return reservas_.concluir(reservaId);
}

bool SistemaGestao::registrarFalta(int reservaId) {
    return reservas_.registrarFalta(reservaId);
}

int SistemaGestao::contarReservasAtivas() const {
    return Reserva::contarReservasAtivas();
}

const Reserva* SistemaGestao::getReservaAtiva(int indice) const {
    return Reserva::getReservaAtiva(indice);  // ja devolve nullptr se for invalido
}

bool SistemaGestao::verificarDisponibilidade(const Quadra& quadra, const std::string& data,
                                             int horarioInicio, int duracaoHoras) const {
    return reservas_.verificarDisponibilidade(quadra, data, horarioInicio, duracaoHoras);
}

// ========================================================== avaliacoes ====
int SistemaGestao::avaliarReserva(int reservaId, int nota, const std::string& comentario) {
    return avaliacoes_.avaliar(reservaId, nota, comentario);
}

double SistemaGestao::mediaDaQuadra(int quadraId) const {
    return avaliacoes_.mediaDaQuadra(quadraId);
}

int SistemaGestao::contarAvaliacoesDaQuadra(int quadraId) const {
    return avaliacoes_.contarDaQuadra(quadraId);
}

bool SistemaGestao::quadraEmDestaque(int quadraId) const {
    return avaliacoes_.quadraEmDestaque(quadraId);
}

// ========================================================== manutencao ====
int SistemaGestao::registrarManutencao(int quadraId, const std::string& dataInicio,
                                       const std::string& dataFim, const std::string& motivo) {
    return manutencoes_.registrar(quadraId, dataInicio, dataFim, motivo);
}

bool SistemaGestao::encerrarManutencao(int manutencaoId) {
    return manutencoes_.encerrar(manutencaoId);
}

bool SistemaGestao::quadraEmManutencao(int quadraId) const {
    return manutencoes_.quadraEmManutencao(quadraId);
}

// ============================================================ consultas ====
int SistemaGestao::getQtdClientes() const {
    return clientes_.quantidade();
}

int SistemaGestao::getQtdQuadras() const {
    return quadras_.quantidade();
}

int SistemaGestao::getQtdReservas() const {
    return reservas_.quantidade();
}

int SistemaGestao::getQtdAvaliacoes() const {
    return avaliacoes_.quantidade();
}

int SistemaGestao::getQtdManutencoes() const {
    return manutencoes_.quantidade();
}

const Cliente* SistemaGestao::getCliente(int indice) const {
    return clientes_.obter(indice);
}

const Quadra* SistemaGestao::getQuadra(int indice) const {
    return quadras_.obter(indice);
}

const Reserva* SistemaGestao::getReserva(int indice) const {
    return reservas_.obter(indice);
}

const Avaliacao* SistemaGestao::getAvaliacao(int indice) const {
    return avaliacoes_.obter(indice);
}

const Manutencao* SistemaGestao::getManutencao(int indice) const {
    return manutencoes_.obter(indice);
}

const Cliente* SistemaGestao::buscarCliente(int id) const {
    return clientes_.achar(id);
}

const Quadra* SistemaGestao::buscarQuadra(int id) const {
    return quadras_.achar(id);
}

const Reserva* SistemaGestao::buscarReserva(int id) const {
    return reservas_.achar(id);
}

std::string SistemaGestao::getUltimoErro() const {
    return mensagens_.getErro();
}

std::string SistemaGestao::getUltimoAviso() const {
    return mensagens_.getAviso();
}

// ================================================================ datas ====
bool SistemaGestao::dataValida(const std::string& data) {
    return Data::valida(data);
}

int SistemaGestao::diferencaEmDias(const std::string& de, const std::string& ate) {
    return Data::diferencaEmDias(de, ate);
}
