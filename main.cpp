#include "models/cliente.hpp"
#include "models/quadra.hpp"
#include "models/reserva.hpp"
#include "models/sistemagestao.hpp"
#include "enums/Categoria.hpp"
#include "enums/TipoEstrategia.hpp"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main() {
    SistemaGestao sistema;
    bool executando = true;

    auto lerTexto = [](const std::string& prompt, std::string& valor) {
        while (true) {
            std::cout << prompt;
            if (!std::getline(std::cin, valor)) {
                return false;
            }
            if (valor.find_first_not_of(" \t\r\n") != std::string::npos) {
                return true;
            }
            std::cout << "O valor nao pode ficar vazio.\n";
        }
    };

    auto lerInteiro = [](const std::string& prompt, int minimo, int maximo, int& valor) {
        while (true) {
            std::string entrada;
            std::cout << prompt;
            if (!std::getline(std::cin, entrada)) {
                return false;
            }

            std::istringstream fluxo(entrada);
            char extra;
            if ((fluxo >> valor) && !(fluxo >> extra)) {
                if (valor >= minimo && valor <= maximo) {
                    return true;
                }
                std::cout << "Valor fora do intervalo permitido.\n";
            } else {
                std::cout << "Digite um numero inteiro valido.\n";
            }
        }
    };

    auto nomeTipo = [](TipoQuadra tipo) {
        switch (tipo) {
            case FUTEBOL: return "Futebol";
            case BASQUETE: return "Basquete";
            case VOLEI: return "Volei";
            case TENIS: return "Tenis";
            case BEACH_TENNIS: return "Beach tennis";
            case PADEL: return "Padel";
            case HANDEBOL: return "Handebol";
        }
        return "Desconhecida";
    };

    while (executando) {
        std::cout << "\n=== Gerenciador de Reservas ===\n"
                  << "1 - Cadastrar cliente\n"
                  << "2 - Cadastrar quadra\n"
                  << "3 - Criar reserva\n"
                  << "4 - Listar clientes e quadras\n"
                  << "5 - Listar reservas ativas\n"
                  << "6 - Cancelar reserva\n"
                  << "7 - Concluir reserva\n"
                  << "8 - Registrar falta\n"
                  << "9 - Avaliar reserva concluida\n"
                  << "0 - Sair\n";

        int opcao;
        if (!lerInteiro("Escolha uma opcao: ", 0, 9, opcao) || opcao == 0) {
            break;
        }

        if (opcao == 1) {
            std::string nome;
            std::string email;
            if (!lerTexto("Nome do cliente: ", nome) ||
                !lerTexto("Email do cliente: ", email)) {
                break;
            }
            const int clienteId = sistema.cadastrarCliente(nome, email);
            std::cout << "Cliente cadastrado. ID: " << clienteId << '\n';
        } else if (opcao == 2) {
            std::string nome;
            std::string localizacao;
            if (!lerTexto("Nome da quadra: ", nome) ||
                !lerTexto("Localizacao: ", localizacao)) {
                break;
            }

            double preco;
            while (true) {
                std::string entrada;
                std::cout << "Preco por hora (use ponto para casas decimais): R$ ";
                if (!std::getline(std::cin, entrada)) {
                    executando = false;
                    break;
                }
                std::istringstream fluxo(entrada);
                char extra;
                if ((fluxo >> preco) && !(fluxo >> extra) &&
                    std::isfinite(preco) && preco > 0) {
                    break;
                }
                std::cout << "Digite um preco valido maior que zero.\n";
            }
            if (!executando) {
                break;
            }

            const TipoQuadra tiposDisponiveis[] = {
                FUTEBOL, BASQUETE, VOLEI, TENIS, BEACH_TENNIS, PADEL, HANDEBOL
            };
            std::vector<TipoQuadra> modalidades;
            std::cout << "Modalidades:\n";
            for (int i = 0; i < 7; ++i) {
                std::cout << i + 1 << " - " << nomeTipo(tiposDisponiveis[i]) << '\n';
            }
            std::cout << "Informe 0 quando terminar.\n";

            while (true) {
                int modalidade;
                if (!lerInteiro("Escolha uma modalidade: ", 0, 7, modalidade)) {
                    executando = false;
                    break;
                }
                if (modalidade == 0) {
                    break;
                }
                const TipoQuadra selecionada = tiposDisponiveis[modalidade - 1];
                bool jaSelecionada = false;
                for (TipoQuadra tipo : modalidades) {
                    if (tipo == selecionada) {
                        jaSelecionada = true;
                        break;
                    }
                }
                if (!jaSelecionada) {
                    modalidades.push_back(selecionada);
                }
            }
            if (!executando) {
                break;
            }
            if (modalidades.empty()) {
                std::cout << "Selecione ao menos uma modalidade; quadra nao cadastrada.\n";
                continue;
            }

            const Categoria categoriasDisponiveis[] = {
                Categoria::Simples, Categoria::Coberta,
                Categoria::Iluminada, Categoria::Premium
            };
            std::cout << "Categorias:\n"
                      << "1 - Simples (sem adicional)\n"
                      << "2 - Coberta (+15%)\n"
                      << "3 - Iluminada (+R$ 10/h a partir das 18h)\n"
                      << "4 - Premium (coberta e iluminada)\n";
            int categoria;
            if (!lerInteiro("Escolha a categoria: ", 1, 4, categoria)) {
                break;
            }

            const TipoEstrategia estrategiasDisponiveis[] = {
                TipoEstrategia::Padrao, TipoEstrategia::Mensalista, TipoEstrategia::Pico
            };
            std::cout << "Estrategias de preco:\n"
                      << "1 - Padrao (preco por hora)\n"
                      << "2 - Mensalista (15% de desconto)\n"
                      << "3 - Horario de pico (+20% entre 18h e 22h)\n";
            int estrategia;
            if (!lerInteiro("Escolha a estrategia de preco: ", 1, 3, estrategia)) {
                break;
            }

            const TipoPolitica politicasDisponiveis[] = {
                TipoPolitica::Flexivel, TipoPolitica::Moderada, TipoPolitica::Rigida
            };
            std::cout << "Politicas de cancelamento:\n"
                      << "1 - Flexivel (100% com 1 dia ou mais; 50% no mesmo dia)\n"
                      << "2 - Moderada (100% com 3 dias ou mais; 50% com 1-2 dias)\n"
                      << "3 - Rigida (50% com 7 dias ou mais)\n";
            int politica;
            if (!lerInteiro("Escolha a politica: ", 1, 3, politica)) {
                break;
            }

            const int quadraId = sistema.cadastrarQuadra(
                nome, localizacao, modalidades, preco,
                estrategiasDisponiveis[estrategia - 1],
                categoriasDisponiveis[categoria - 1],
                politicasDisponiveis[politica - 1]);
            std::cout << "Quadra cadastrada. ID: " << quadraId << '\n';
        } else if (opcao == 3) {
            if (sistema.getQtdClientes() == 0 || sistema.getQtdQuadras() == 0) {
                std::cout << "Cadastre ao menos um cliente e uma quadra antes de reservar.\n";
                continue;
            }

            std::cout << "\nClientes:\n";
            for (int i = 0; i < sistema.getQtdClientes(); ++i) {
                const Cliente* cliente = sistema.getCliente(i);
                std::cout << i + 1 << " - " << cliente->getNome()
                          << " (" << cliente->getEmail() << ")\n";
            }
            int indiceCliente;
            if (!lerInteiro("Numero do cliente: ", 1, sistema.getQtdClientes(), indiceCliente)) {
                break;
            }

            std::cout << "\nQuadras:\n";
            for (int i = 0; i < sistema.getQtdQuadras(); ++i) {
                const Quadra* quadra = sistema.getQuadra(i);
                std::cout << i + 1 << " - " << quadra->getNome()
                          << " | " << quadra->getLocalizacao()
                          << " | R$ " << quadra->getPrecoHora() << "/hora\n";
            }
            int indiceQuadra;
            if (!lerInteiro("Numero da quadra: ", 1, sistema.getQtdQuadras(), indiceQuadra)) {
                break;
            }

            std::string data;
            int inicio;
            int duracao;
            if (!lerTexto("Data da reserva (ex.: AAAA-MM-DD): ", data) ||
                !lerInteiro("Horario de inicio (0-23): ", 0, 23, inicio) ||
                !lerInteiro("Duracao em horas: ", 1, 24, duracao)) {
                break;
            }
            Cliente* cliente = sistema.getCliente(indiceCliente - 1);
            Quadra* quadra = sistema.getQuadra(indiceQuadra - 1);
            try {
                Reserva* reserva = sistema.criarReserva(
                    *cliente, *quadra, data, inicio, duracao);
                std::cout << "Reserva criada. Valor total: R$ "
                          << reserva->getValorTotal() << '\n';
            } catch (const std::exception& erro) {
                std::cout << "Nao foi possivel criar a reserva: " << erro.what() << '\n';
            }
        } else if (opcao == 4) {
            std::cout << "\nClientes cadastrados:\n";
            if (sistema.getQtdClientes() == 0) {
                std::cout << "Nenhum cliente cadastrado.\n";
            }
            for (int i = 0; i < sistema.getQtdClientes(); ++i) {
                const Cliente* cliente = sistema.getCliente(i);
                std::cout << cliente->getId() << " - " << cliente->getNome()
                          << " (" << cliente->getEmail() << ")\n";
            }

            std::cout << "\nQuadras cadastradas:\n";
            if (sistema.getQtdQuadras() == 0) {
                std::cout << "Nenhuma quadra cadastrada.\n";
            }
            for (int i = 0; i < sistema.getQtdQuadras(); ++i) {
                const Quadra* quadra = sistema.getQuadra(i);
                std::cout << quadra->getId() << " - " << quadra->getNome()
                          << " | Local: " << quadra->getLocalizacao()
                          << " | Preco/hora: R$ " << quadra->getPrecoHora()
                          << " | Categoria: " << quadra->getCategoria()
                          << " | Estrategia: " << quadra->getNomeEstrategiaPreco()
                          << " | Politica: "
                          << quadra->getPoliticaCancelamento()->getNome()
                          << " | Status: " << (quadra->isAtiva() ? "Ativa" : "Inativa")
                          << (sistema.quadraEmDestaque(quadra->getId())
                                  ? " | Em destaque" : "")
                          << " | Modalidades: ";
                const auto modalidades = quadra->getTipos();
                for (std::size_t i = 0; i < modalidades.size(); ++i) {
                    if (i > 0) {
                        std::cout << ", ";
                    }
                    std::cout << nomeTipo(modalidades[i]);
                }
                std::cout << '\n';
            }
        } else if (opcao == 9) {
            const auto reservas = sistema.listarReservasConcluidas();
            if (reservas.empty()) {
                std::cout << "Nao ha reservas concluidas para avaliar.\n";
                continue;
            }

            std::cout << "\nReservas concluidas:\n";
            for (std::size_t i = 0; i < reservas.size(); ++i) {
                const Reserva* reserva = reservas[i];
                std::cout << i + 1 << " - Cliente: " << reserva->getCliente()->getNome()
                          << " | Quadra: " << reserva->getQuadra()->getNome()
                          << " | Data: " << reserva->getDataReserva()
                          << " | Valor: R$ " << reserva->getValorTotal() << '\n';
            }

            int indiceReserva;
            int nota;
            std::string comentario;
            if (!lerInteiro("Numero da reserva: ", 1,
                            static_cast<int>(reservas.size()), indiceReserva) ||
                !lerInteiro("Nota (1-5): ", 1, 5, nota) ||
                !lerTexto("Comentario: ", comentario)) {
                break;
            }
            Reserva* reserva = reservas[indiceReserva - 1];
            const int avaliacaoId = sistema.avaliarReserva(
                reserva->getId(), nota, comentario);
            if (avaliacaoId < 0) {
                std::cout << "A avaliacao nao pode ser registrada.\n";
                continue;
            }
            std::cout << "Avaliacao registrada. Media da quadra: "
                      << sistema.mediaDaQuadra(reserva->getQuadra()->getId()) << '\n';
            if (nota <= 2) {
                std::cout << "Cliente recebeu cupom de 15% para a proxima reserva.\n";
                if (!reserva->getQuadra()->isAtiva()) {
                    std::cout << "A quadra foi desativada para revisao.\n";
                }
            }
        } else if (opcao >= 5 && opcao <= 8) {
            const auto reservas = sistema.listarReservasAtivas();
            if (reservas.empty()) {
                std::cout << "Nao ha reservas ativas.\n";
                continue;
            }

            std::cout << "\nReservas ativas:\n";
            for (std::size_t i = 0; i < reservas.size(); ++i) {
                const Reserva* reserva = reservas[i];
                std::cout << i + 1 << " - Cliente: " << reserva->getCliente()->getNome()
                          << " | Quadra: " << reserva->getQuadra()->getNome()
                          << " | Data: " << reserva->getDataReserva()
                          << " | Inicio: " << reserva->getHorarioInicio() << "h"
                          << " | Duracao: " << reserva->getDuracaoHoras() << "h"
                          << " | Valor: R$ " << reserva->getValorTotal() << '\n';
            }

            if (opcao >= 6) {
                int indiceReserva;
                if (!lerInteiro("Numero da reserva: ", 1,
                                static_cast<int>(reservas.size()), indiceReserva)) {
                    break;
                }
                Reserva& selecionada = *reservas[indiceReserva - 1];
                if (opcao == 6) {
                    if (sistema.cancelarReserva(selecionada)) {
                        std::cout << "Reserva cancelada. Reembolso: R$ "
                                  << selecionada.getValorReembolso() << '\n';
                    } else {
                        std::cout << "A reserva nao pode ser cancelada.\n";
                    }
                } else if (opcao == 7) {
                    if (sistema.concluirReserva(selecionada)) {
                        std::cout << "Reserva concluida.\n";
                    } else {
                        std::cout << "A reserva ainda nao comecou ou nao esta confirmada.\n";
                    }
                } else if (sistema.registrarFalta(selecionada)) {
                    std::cout << "Falta registrada. Total de faltas do cliente: "
                              << selecionada.getCliente()->getFaltas() << '\n';
                    if (selecionada.getCliente()->estaBloqueado()) {
                        std::cout << "Cliente bloqueado por faltas sem aviso.\n";
                    }
                } else {
                    std::cout << "A falta nao pode ser registrada antes do inicio da reserva.\n";
                }
            }
        }
    }

    std::cout << "Encerrando o gerenciador de reservas.\n";
    return 0;
}
