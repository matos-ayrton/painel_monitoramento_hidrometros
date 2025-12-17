#include "WebhookNotificacaoStrategy.hpp"
#include <iostream>

void WebhookNotificacaoStrategy::enviar(const std::string &destinatario,
                                        const std::string &mensagem) {
  // Simulação de Webhook
  std::cout << "[WEBHOOK] POST " << destinatario << " body={" << mensagem << "}"
            << std::endl;
  // Aqui faria um POST HTTP
}
