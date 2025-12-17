#ifndef SMS_NOTIFICACAO_STRATEGY_HPP
#define SMS_NOTIFICACAO_STRATEGY_HPP

#include "INotificacaoStrategy.hpp"
#include <string>

// RF22: SMS Strategy
class SMSNotificacaoStrategy : public INotificacaoStrategy {
public:
  void enviar(const std::string &destinatario,
              const std::string &mensagem) override;
};

#endif // SMS_NOTIFICACAO_STRATEGY_HPP
