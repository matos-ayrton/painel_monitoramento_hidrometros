#include "Fachada.hpp"
#include "OcrStrategyFactory.hpp" 
#include "MonitoramentoConcretoMediator.hpp" 
#include "TesseractOcrAdapter.hpp"
#include <iostream>

// Inicialização da instância estática
Fachada* Fachada::instance = nullptr; 

Fachada& Fachada::getInstance() {
    if (instance == nullptr) {
        instance = new Fachada(); 
    }
    return *instance;
}

// Configuração das Strategies (Factory Method)
std::vector<MonitoramentoService::ShaFolder> Fachada::configurarPastas() {
    
    std::vector<MonitoramentoService::ShaFolder> config;
    
    // SHA 1: pasta do simulador principal e ROI ajustado
    config.push_back({
        1,
        "/home/ayrton/Documentos/simulador_hidrometro_real/Medicoes_202311250031",
        OcrStrategyFactory::createStrategy(StrategyType::ESTATICA, 260, 313, 545, 400, ocrAdapter)
    });

    // SHA 2: pasta do simulador analógico e ROI ajustado
    config.push_back({
        2,
        "/home/ayrton/Documentos/Simulador-Hidrometro-Analogico/Medições_202311250039",
        OcrStrategyFactory::createStrategy(StrategyType::ESTATICA, 204, 131, 360, 224, ocrAdapter)
    });
    
    return config;
}

// Construtor Privado (Singleton)
Fachada::Fachada()
    // Apenas inicializa o Banco; UsuarioService foi removido
    : banco(std::make_unique<MonitoramentoBanco>()),
      ocrAdapter(std::make_shared<TesseractOcrAdapter>())
{
    // Inicializa o adaptador OCR
    try {
        ocrAdapter->inicializar();
        ocrAdapter->definirWhitelist("0123456789");
    } catch (const std::exception& e) {
        std::cerr << "[ERRO] Falha ao inicializar OCR Adapter: " << e.what() << std::endl;
        throw;
    }

    auto pastas = configurarPastas();
    
    // 2. Criar o Mediator, passando a referência do Banco
    mediator = std::make_unique<MonitoramentoConcretoMediator>(*banco);
    
    // 3. Criar o MonitoramentoService
    monitor = std::make_unique<MonitoramentoService>(std::move(pastas), *mediator, *banco);
    
    // Habilita mensagens verbosas por padrão (comportamento "como antes")
    try {
        setVerbose(true);
    } catch (...) {
        // Não falhar a construção por causa de logs; seguir sem verbose se houver problema
    }

    ativo = false;
}

Fachada::~Fachada() {
    pararMonitoramento();
    if (ocrAdapter) {
        ocrAdapter->finalizar();
    }
}

void Fachada::iniciarMonitoramentoBackground() {
    if (!ativo) {
        std::cout << "[FACHADA] Inicializando Monitoramento de SHAs em thread separada..." << std::endl;
        threadMonitoramento = std::make_unique<std::thread>(&MonitoramentoService::iniciar, monitor.get());
        ativo = true;
    }
}

void Fachada::setVerbose(bool ativoVerbose) {
    if (monitor) {
        monitor->setVerbose(ativoVerbose);
        std::cout << "[FACHADA] Verbose set to " << (ativoVerbose ? "ON" : "OFF") << std::endl;
    }
}

void Fachada::pararMonitoramento() {
    if (ativo) {
        std::cout << "[FACHADA] Sinalizando encerramento do Monitoramento..." << std::endl;
        if (threadMonitoramento->joinable()) {
            threadMonitoramento->detach(); 
        }
        ativo = false;
    }
}

long long Fachada::obterConsumo(int idSHA) {
    return banco->getTotal(idSHA);
}

int Fachada::obterUltimaLeitura(int idSHA) {
    return banco->getUltimaLeituraAbsoluta(idSHA);
}

std::vector<int> Fachada::listarHidrometros() {
    return banco->listarHidrometros();
}

bool Fachada::estaAtivo() {
    return ativo;
}

// O método gerenciarUsuarios() foi removido

void Fachada::consultarLogsDeErro() {
    std::cout << "\n[FACHADA] Orquestrando Subsistema de Logs (Placeholder)..." << std::endl;
}