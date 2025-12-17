#ifndef WEBHOOK_NOTIFICACAO_STRATEGY_HPP
#define WEBHOOK_NOTIFICACAO_STRATEGY_HPP

#include "INotificacaoStrategy.hpp"
#include <string>

// RF23: Webhook Strategy
class WebhookNotificacaoStrategy : public INotificacaoStrategy {
public:
  void enviar(const std::string &destinatario,
              const std::string &mensagem) override;
};

#endif // WEBHOOK_NOTIFICACAO_STRATEGY_HPP
