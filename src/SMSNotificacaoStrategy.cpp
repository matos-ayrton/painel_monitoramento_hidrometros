#include "SMSNotificacaoStrategy.hpp"
#include <iostream>

void SMSNotificacaoStrategy::enviar(const std::string &destinatario,
                                    const std::string &mensagem) {
  // Simulação de chamada a API de SMS (Adapter implícito)
  std::cout << "[SMS] Enviando para " << destinatario << ": " << mensagem
            << std::endl;
  // Aqui chamaria Twilio API ou similar
}
