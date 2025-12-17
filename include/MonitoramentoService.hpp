#ifndef MONITORAMENTOSERVICE_HPP
#define MONITORAMENTOSERVICE_HPP

#include "ILeituraStrategy.hpp"
#include "IMonitoramentoMediator.hpp"
#include "ISubject.hpp"
#include "MonitoramentoBanco.hpp"
#include <algorithm>
#include <atomic>
#include <filesystem>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

class MonitoramentoService : public ISubject {
public:
  struct ShaFolder {
    int idSHA;
    std::string pasta;
    std::unique_ptr<ILeituraStrategy> strategy; // Strategy Pattern
  };

  MonitoramentoService(std::vector<ShaFolder> pastas,
                       IMonitoramentoMediator &mediatorRef,
                       MonitoramentoBanco &bancoRef);

  void iniciar();
  void parar();

  // Habilita/desabilita saída verbose para debug (imprime varredura e leituras)
  void setVerbose(bool ativo);

  // Métodos ISubject
  void attach(std::shared_ptr<IObserver> observer) override;
  void detach(std::shared_ptr<IObserver> observer) override;
  void notify(const std::string &mensagem) override;

private:
  std::vector<ShaFolder> pastasSHA;
  std::unordered_map<std::string, fs::file_time_type> arquivosLidos;

  IMonitoramentoMediator &mediator;
  MonitoramentoBanco &banco;
  bool verbose{false};
  std::atomic<bool> deveParar{false};

  std::vector<std::shared_ptr<IObserver>> observers;
  std::mutex observersMtx;
};

#endif