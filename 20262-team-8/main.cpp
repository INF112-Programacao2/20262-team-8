#include "models/cliente.hpp"
#include "models/quadra.hpp"
#include "models/reserva.hpp"

#include <cmath>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

int main() {
    std::vector<std::unique_ptr<Cliente>> clientes;
    std::vector<std::unique_ptr<Quadra>> quadras;
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
                  << "0 - Sair\n";

        int opcao;
        if (!lerInteiro("Escolha uma opcao: ", 0, 6, opcao) || opcao == 0) {
            break;
        }

        if (opcao == 1) {
            std::string nome;
            std::string email;
            if (!lerTexto("Nome do cliente: ", nome) ||
                !lerTexto("Email do cliente: ", email)) {
                break;
            }
            clientes.push_back(std::make_unique<Cliente>(nome, email));
            std::cout << "Cliente cadastrado. ID: " << clientes.back()->getId() << '\n';
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

            quadras.push_back(std::make_unique<Quadra>(nome, localizacao, modalidades, preco));
            std::cout << "Quadra cadastrada. ID: " << quadras.back()->getId() << '\n';
        } else if (opcao == 3) {
            if (clientes.empty() || quadras.empty()) {
                std::cout << "Cadastre ao menos um cliente e uma quadra antes de reservar.\n";
                continue;
            }

            std::cout << "\nClientes:\n";
            for (std::size_t i = 0; i < clientes.size(); ++i) {
                std::cout << i + 1 << " - " << clientes[i]->getNome()
                          << " (" << clientes[i]->getEmail() << ")\n";
            }
            int indiceCliente;
            if (!lerInteiro("Numero do cliente: ", 1,
                            static_cast<int>(clientes.size()), indiceCliente)) {
                break;
            }

            std::cout << "\nQuadras:\n";
            for (std::size_t i = 0; i < quadras.size(); ++i) {
                std::cout << i + 1 << " - " << quadras[i]->getNome()
                          << " | " << quadras[i]->getLocalizacao()
                          << " | R$ " << quadras[i]->getPrecoHora() << "/hora\n";
            }
            int indiceQuadra;
            if (!lerInteiro("Numero da quadra: ", 1,
                            static_cast<int>(quadras.size()), indiceQuadra)) {
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
            if (inicio + duracao > 24) {
                std::cout << "A reserva deve terminar ate as 24h.\n";
                continue;
            }

            Cliente* cliente = clientes[indiceCliente - 1].get();
            Quadra* quadra = quadras[indiceQuadra - 1].get();
            bool conflito = false;
            for (const Reserva* reserva : Reserva::listarReservasAtivas()) {
                if (reserva->getQuadra() == quadra &&
                    reserva->getDataReserva() == data &&
                    inicio < reserva->getHorarioInicio() + reserva->getDuracaoHoras() &&
                    reserva->getHorarioInicio() < inicio + duracao) {
                    conflito = true;
                    break;
                }
            }
            if (conflito) {
                std::cout << "A quadra ja possui uma reserva nesse horario.\n";
                continue;
            }

            try {
                new Reserva(cliente, quadra, data, inicio, duracao);
                const auto reservas = Reserva::listarReservasAtivas();
                std::cout << "Reserva criada. Valor total: R$ "
                          << reservas.back()->getValorTotal() << '\n';
            } catch (const std::exception& erro) {
                std::cout << "Nao foi possivel criar a reserva: " << erro.what() << '\n';
            }
        } else if (opcao == 4) {
            std::cout << "\nClientes cadastrados:\n";
            if (clientes.empty()) {
                std::cout << "Nenhum cliente cadastrado.\n";
            }
            for (const auto& cliente : clientes) {
                std::cout << cliente->getId() << " - " << cliente->getNome()
                          << " (" << cliente->getEmail() << ")\n";
            }

            std::cout << "\nQuadras cadastradas:\n";
            if (quadras.empty()) {
                std::cout << "Nenhuma quadra cadastrada.\n";
            }
            for (const auto& quadra : quadras) {
                std::cout << quadra->getId() << " - " << quadra->getNome()
                          << " | Local: " << quadra->getLocalizacao()
                          << " | Preco/hora: R$ " << quadra->getPrecoHora()
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
        } else if (opcao == 5 || opcao == 6) {
            const auto reservas = Reserva::listarReservasAtivas();
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

            if (opcao == 6) {
                int indiceReserva;
                if (!lerInteiro("Numero da reserva para cancelar: ", 1,
                                static_cast<int>(reservas.size()), indiceReserva)) {
                    break;
                }
                reservas[indiceReserva - 1]->cancelarReserva();
                std::cout << "Reserva cancelada.\n";
            }
        }
    }

    std::cout << "Encerrando o gerenciador de reservas.\n";
    return 0;
}
