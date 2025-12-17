#ifndef EMAIL_NOTIFICACAO_STRATEGY_HPP
#define EMAIL_NOTIFICACAO_STRATEGY_HPP

#include "INotificacaoStrategy.hpp"
#include <string>

// RF21: Email Strategy (Real Libcurl Implementation)
class EmailNotificacaoStrategy : public INotificacaoStrategy {
public:
  void enviar(const std::string &destinatario,
              const std::string &mensagem) override;
};

#endif // EMAIL_NOTIFICACAO_STRATEGY_HPP
