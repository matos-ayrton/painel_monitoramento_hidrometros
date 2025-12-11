#include "MonitoramentoService.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <stdexcept>

using namespace std::chrono_literals; 
namespace fs = std::filesystem;

MonitoramentoService::MonitoramentoService(std::vector<ShaFolder> pastas,
                       IMonitoramentoMediator& mediatorRef,
                       MonitoramentoBanco& bancoRef)
    : pastasSHA(std::move(pastas)), mediator(mediatorRef), banco(bancoRef), verbose(false)
{}

void MonitoramentoService::setVerbose(bool ativo) {
    verbose = ativo;
}

void MonitoramentoService::iniciar() {
    while (true) {
        for (const auto& pastaInfo : pastasSHA) {
            if (verbose) {
                std::cout << "[MON] Varrendo pasta '" << pastaInfo.pasta << "' (SHA " << pastaInfo.idSHA << ")" << std::endl;
            }
            
            // Lógica de Retry para acesso à pasta (para mitigar race conditions)
            int tentativas = 0;
            const int MAX_TENTATIVAS = 10;
            const int ESPERA_MS = 500; 
            
            while ((!fs::exists(pastaInfo.pasta) || !fs::is_directory(pastaInfo.pasta)) && tentativas < MAX_TENTATIVAS) {
                tentativas++;
                std::this_thread::sleep_for(std::chrono::milliseconds(ESPERA_MS));
            }
            
            if (tentativas == MAX_TENTATIVAS) {
                std::cerr << " [ERRO] SHA " << pastaInfo.idSHA << ": Pasta inacessível após tentativas." << std::endl;
                continue;
            }

            for (const auto& entry : fs::directory_iterator(pastaInfo.pasta)) {
                
                if (!entry.is_regular_file()) continue;

                std::string ext = entry.path().extension().string();
                if (ext != ".png" && ext != ".jpg" && ext != ".jpeg" && ext != ".bmp") continue;

                std::string caminhoArquivo = entry.path().string();
                fs::file_time_type tempoModificacao = fs::last_write_time(entry.path());

                if (arquivosLidos.find(caminhoArquivo) == arquivosLidos.end() || 
                    arquivosLidos[caminhoArquivo] < tempoModificacao) {
                    
                    try {
                        // 1. CHAMA O STRATEGY
                        if (verbose) std::cout << "[MON] Processando arquivo: " << entry.path().filename().string() << std::endl;
                        int volume_atual = pastaInfo.strategy->extrairValor(caminhoArquivo);
                        int id_sha = pastaInfo.idSHA;
                        
                        // 2. CONSULTA O BANCO
                        int ultima_leitura = banco.getUltimaLeituraAbsoluta(id_sha); 
                        
                        // 3. NOTIFICA O MEDIATOR
                        if (verbose) {
                            int delta = (volume_atual > ultima_leitura) ? (volume_atual - ultima_leitura) : 0;
                            std::cout << "[MON] SHA " << id_sha << " => valor=" << volume_atual
                                      << " ultima=" << ultima_leitura << " delta=" << delta << std::endl;
                        }
                        mediator.notificarLeitura(id_sha, volume_atual, ultima_leitura);
                        
                        arquivosLidos[caminhoArquivo] = tempoModificacao;
                        
                    } 
                    catch (const std::exception& e) {
                        std::cerr << "[ERRO] SHA " << pastaInfo.idSHA << " em " << entry.path().filename().string() 
                                  << ": " << e.what() << std::endl;
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(10s);
    }
}