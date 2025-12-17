#include "MonitoramentoService.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

using namespace std::chrono_literals;
namespace fs = std::filesystem;

MonitoramentoService::MonitoramentoService(std::vector<ShaFolder> pastas,
                                           IMonitoramentoMediator &mediatorRef,
                                           MonitoramentoBanco &bancoRef)
    : pastasSHA(std::move(pastas)), mediator(mediatorRef), banco(bancoRef),
      verbose(false) {}

void MonitoramentoService::setVerbose(bool ativo) { verbose = ativo; }

// NOVO: Implementação do método de parada
void MonitoramentoService::parar() {
  deveParar = true;
  if (verbose) {
    std::cout << "[MON] Sinal de parada recebido." << std::endl;
  }
}

// Métodos ISubject
void MonitoramentoService::attach(std::shared_ptr<IObserver> observer) {
  std::lock_guard<std::mutex> lock(observersMtx);
  observers.push_back(observer);
}

void MonitoramentoService::detach(std::shared_ptr<IObserver> observer) {
  std::lock_guard<std::mutex> lock(observersMtx);
  observers.erase(std::remove(observers.begin(), observers.end(), observer),
                  observers.end());
}

void MonitoramentoService::notify(const std::string &mensagem) {
  std::lock_guard<std::mutex> lock(observersMtx);
  for (const auto &observer : observers) {
    observer->update("MonitoramentoService", mensagem);
  }
}

void MonitoramentoService::iniciar() {
  // 1. Loop principal agora checa a flag deveParar
  while (!deveParar) {
    for (const auto &pastaInfo : pastasSHA) {

      // 2. Checagem adicional dentro do loop interno para parada rápida
      if (deveParar)
        break;
      /*
      if (verbose) {
          std::cout << "[MON] Varrendo pasta '" << pastaInfo.pasta << "' (SHA "
      << pastaInfo.idSHA << ")" << std::endl;
      }*/

      // ... (Lógica de Retry e Acesso à Pasta) ...
      int tentativas = 0;
      const int MAX_TENTATIVAS = 10;
      const int ESPERA_MS = 500;

      while ((!fs::exists(pastaInfo.pasta) ||
              !fs::is_directory(pastaInfo.pasta)) &&
             tentativas < MAX_TENTATIVAS) {
        tentativas++;
        if (deveParar)
          return; // Retorna se a flag for setada durante a espera
        std::this_thread::sleep_for(std::chrono::milliseconds(ESPERA_MS));
      }

      if (tentativas == MAX_TENTATIVAS) {
        std::cerr << " [ERRO] SHA " << pastaInfo.idSHA
                  << ": Pasta inacessível após tentativas." << std::endl;
        continue;
      }

      for (const auto &entry : fs::directory_iterator(pastaInfo.pasta)) {

        if (deveParar)
          return; // Retorna se a flag for setada durante a iteração

        if (!entry.is_regular_file())
          continue;

        std::string ext = entry.path().extension().string();
        if (ext != ".png" && ext != ".jpg" && ext != ".jpeg" && ext != ".bmp")
          continue;

        // ... (Lógica de Processamento e Chamada ao Mediator) ...
        std::string caminhoArquivo = entry.path().string();
        fs::file_time_type tempoModificacao = fs::last_write_time(entry.path());

        if (arquivosLidos.find(caminhoArquivo) == arquivosLidos.end() ||
            arquivosLidos[caminhoArquivo] < tempoModificacao) {

          try {
            int volume_atual = pastaInfo.strategy->extrairValor(caminhoArquivo);
            int id_sha = pastaInfo.idSHA;
            int ultima_leitura = banco.getUltimaLeituraAbsoluta(id_sha);
            if (verbose) {
              int delta = (volume_atual > ultima_leitura)
                              ? (volume_atual - ultima_leitura)
                              : 0;
              std::cout << "[MON] (" << entry.path().filename().string()
                        << ") SHA " << id_sha << " => valor=" << volume_atual
                        << " ultima=" << ultima_leitura << " delta=" << delta
                        << std::endl;
            }
            mediator.notificarLeitura(id_sha, volume_atual, ultima_leitura);

            // RF19: Notificar observadores (AlertService)
            // Formato: HIDROMETRO:<ID>:CONSUMO:<VALOR>
            notify("HIDROMETRO:" + std::to_string(id_sha) +
                   ":CONSUMO:" + std::to_string(volume_atual));

            arquivosLidos[caminhoArquivo] = tempoModificacao;
          } catch (const std::exception &e) {
            // Opcional: Logar erro apenas se verbose estiver ligado
            if (verbose) {
              std::cerr << "[ERRO] Falha ao processar "
                        << entry.path().filename().string() << ": " << e.what()
                        << std::endl;
            }
            // IMPORTANTE: Marcamos como lido mesmo com erro para evitar
            // retentativas infinitas (loop de erro/CPU) no mesmo arquivo
            // até que ele seja modificado novamente.
            arquivosLidos[caminhoArquivo] = tempoModificacao;
          }
        }
      }
    }

    // 3. Checa a flag antes de dormir e se a flag foi setada durante o sono
    for (int i = 0; i < 10; ++i) { // 10s em passos de 1s
      if (deveParar)
        return;
      std::this_thread::sleep_for(1s);
    }
  }
}