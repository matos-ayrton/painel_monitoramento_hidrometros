#ifndef INOTIFICACAO_STRATEGY_HPP
#define INOTIFICACAO_STRATEGY_HPP

#include <string>

// Strategy Interface for Notifications (RF20, RF21, RF22, RF23)
class INotificacaoStrategy {
public:
  virtual ~INotificacaoStrategy() = default;
  virtual void enviar(const std::string &destinatario,
                      const std::string &mensagem) = 0;
};

#endif // INOTIFICACAO_STRATEGY_HPP
