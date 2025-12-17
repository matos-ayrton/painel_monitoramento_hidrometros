#ifndef ALERTSERVICE_HPP
#define ALERTSERVICE_HPP

#include "IObserver.hpp"
#include "NotificacaoService.hpp"
#include <mutex>
#include <string>
#include <unordered_map>

class AlertService : public IObserver {
private:
  NotificacaoService &notificacaoService;
  std::unordered_map<int, double>
      limitesConsumo; // Limit per user/hidrometro (simplificado por hidrometro
                      // ID)
  double limitePadrao = 100.0; // Default limit
  std::mutex mtx;

public:
  AlertService(NotificacaoService &notifService)
      : notificacaoService(notifService) {}

  // RF18: Configurar limite
  void configurarLimite(int idHidrometro, double limite) {
    std::lock_guard<std::mutex> lock(mtx);
    limitesConsumo[idHidrometro] = limite;
  }

  // RF19: Gatilho de Alerta (Observer update)
  void update(const std::string &origem, const std::string &mensagem) override;

  // Configurar destinatário do alerta (Novo Requisito)
  void setDestinatario(const std::string &email) {
    std::lock_guard<std::mutex> lock(mtx);
    destinatarioAlerta = email;
  }

private:
  std::string destinatarioAlerta = "admin@saneamento.com"; // Default
};

#endif // ALERTSERVICE_HPP
