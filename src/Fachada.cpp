#include "Fachada.hpp"
#include <iostream>

Fachada::Fachada() : ativo(false) {
    banco = std::make_unique<MonitoramentoBanco>();
}

Fachada::~Fachada() {
    pararMonitoramento();
}

std::vector<MonitoramentoService::ShaFolder> Fachada::configurarPastas() {
    return std::vector<MonitoramentoService::ShaFolder> {
        {1, "/home/ayrton/Documentos/simulador_hidrometro_real/Medicoes_202311250031", 270, 312, 550, 410},
        {2, "/home/ayrton/Documentos/Simulador-Hidrometro-Analogico/Medições_202311250039", 205, 140, 443, 219}
    };
}

void Fachada::iniciarMonitoramentoBackground() {
    if (ativo) {
        std::cout << "Monitoramento já está em execução." << std::endl;
        return;
    }

    auto pastas = configurarPastas();
    monitor = std::make_unique<MonitoramentoService>(pastas, *banco);
    ativo = true;

    threadMonitoramento = std::make_unique<std::thread>([this]() {
        try {
            monitor->iniciar();
        } catch (const std::exception& e) {
            std::cerr << "Erro no monitoramento: " << e.what() << std::endl;
            ativo = false;
        }
    });

    std::cout << "Monitoramento iniciado em background." << std::endl;
}

void Fachada::pararMonitoramento() {
    if (!ativo) {
        return;
    }

    ativo = false;
    if (threadMonitoramento && threadMonitoramento->joinable()) {
        threadMonitoramento->join();
    }
    monitor.reset();
    std::cout << "Monitoramento parado." << std::endl;
}

int Fachada::obterConsumo(int idSHA) {
    return banco->getTotal(idSHA);
}

int Fachada::obterUltimaLeitura(int idSHA) {
    return banco->getUltimaLeituraAbsoluta(idSHA);
}

std::vector<int> Fachada::listarHidrometros() {
    return {1, 2};
}

bool Fachada::estaAtivo() {
    return ativo;
}
