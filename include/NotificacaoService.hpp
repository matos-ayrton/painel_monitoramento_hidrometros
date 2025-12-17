#ifndef NOTIFICACAO_SERVICE_HPP
#define NOTIFICACAO_SERVICE_HPP

#include "INotificacaoStrategy.hpp"
#include <memory>
#include <string>

class NotificacaoService {
private:
  std::unique_ptr<INotificacaoStrategy> strategy;

public:
  // Define a estratégia de notificação
  void setStrategy(std::unique_ptr<INotificacaoStrategy> novaStrategy) {
    strategy = std::move(novaStrategy);
  }

  // Envia a notificação usando a estratégia atual
  void notificar(const std::string &destinatario, const std::string &mensagem) {
    if (strategy) {
      strategy->enviar(destinatario, mensagem);
    }
  }
};

#endif // NOTIFICACAO_SERVICE_HPP
