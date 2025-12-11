#include "CLI.hpp"
#include <iostream>
#include <iomanip>
#include <limits> 

CLI::CLI(Fachada& fachada) : fachada(fachada) {}

void CLI::executar() {
    std::cout << "╔════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║   SISTEMA DE MONITORAMENTO DE HIDRÔMETROS (CLIENTE) ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════════╝" << std::endl << std::endl;

    fachada.iniciarMonitoramentoBackground(); 
    std::cout << std::endl;

    std::string opcao;
    while (true) {
        exibirMenu();
        std::cout << "Escolha uma opção: ";
        
        if (!std::getline(std::cin, opcao)) {
            break; 
        }

        if (opcao == "1") {
            consultarConsumo();
        } else if (opcao == "2") {
            consultarUltimaLeitura();
        } else if (opcao == "3") {
            listarHidrometros();
        } else if (opcao == "4") {
            exibirStatus();
        } else if (opcao == "5") {
            std::cout << "\nEncerrando sistema..." << std::endl;
            fachada.pararMonitoramento();
            break;
        } else {
            std::cout << "\nOpção inválida. Tente novamente.\n" << std::endl;
        }
    }
}

void CLI::exibirMenu() {
    std::cout << "┌────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ 1. Consultar consumo total por ID                 │" << std::endl;
    std::cout << "│ 2. Consultar última leitura absoluta              │" << std::endl;
    std::cout << "│ 3. Listar hidrometros disponíveis                 │" << std::endl;
    std::cout << "│ 4. Exibir status do monitoramento                 │" << std::endl;
    std::cout << "│ 5. Sair                                           │" << std::endl;
    std::cout << "└────────────────────────────────────────────────────┘" << std::endl;
}

void CLI::consultarConsumo() {
    int id;
    std::cout << "\nDigite o ID do hidrometro: ";
    if (!(std::cin >> id)) {
        std::cout << "Entrada inválida." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    long long consumo = fachada.obterConsumo(id);
    std::cout << "\n✓ Consumo total do hidrometro " << id << ": " << std::setw(10) << consumo << " metros cúbicos\n" << std::endl;
}

void CLI::consultarUltimaLeitura() {
    int id;
    std::cout << "\nDigite o ID do hidrometro: ";
    if (!(std::cin >> id)) {
        std::cout << "Entrada inválida." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    int leitura = fachada.obterUltimaLeitura(id);
    std::cout << "\n✓ Última leitura absoluta do hidrometro " << id << ": " << std::setw(10) << leitura << " unidades\n" << std::endl;
}

void CLI::listarHidrometros() {
    auto hidrometros = fachada.listarHidrometros();
    std::cout << "\n╔══════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║  Hidrometros Disponíveis e Status Atual                      ║" << std::endl;
    std::cout << "╠══════════════════════════════════════════════════════════════════╣" << std::endl;

    for (int id : hidrometros) {
        long long consumo = fachada.obterConsumo(id);
        int leitura = fachada.obterUltimaLeitura(id);
        std::cout << "║ ID: " << std::setw(2) << id 
                  << " | Consumo Acumulado: " << std::setw(15) << consumo
                  << " | Última Leitura: " << std::setw(15) << leitura << " ║" << std::endl;
    }
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n" << std::endl;
}

void CLI::exibirStatus() {
    std::cout << "\n┌──────────────────────────────────────┐" << std::endl;
    if (fachada.estaAtivo()) {
        std::cout << "│ Status: ✓ MONITORAMENTO ATIVO        │" << std::endl;
    } else {
        std::cout << "│ Status: ✗ MONITORAMENTO INATIVO      │" << std::endl;
    }
    std::cout << "└──────────────────────────────────────┘\n" << std::endl;
}