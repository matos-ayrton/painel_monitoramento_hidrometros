#include "AlertService.hpp"
#include <iostream>
#include <sstream>

void AlertService::update(const std::string &origem,
                          const std::string &mensagem) {
  // Mensagem esperada: "HIDROMETRO:<ID>:CONSUMO:<VALOR>"
  // Parse simples para verificar limites
  if (mensagem.find("HIDROMETRO:") != std::string::npos) {
    try {
      // Exemplo de parsing simples
      // Formato: HIDROMETRO:1:CONSUMO:150
      int idStart = mensagem.find(":") + 1;
      int idEnd = mensagem.find(":CONSUMO:");
      std::string idStr = mensagem.substr(idStart, idEnd - idStart);
      int id = std::stoi(idStr);

      int valStart = idEnd + 9; // length of ":CONSUMO:"
      std::string valStr = mensagem.substr(valStart);
      double consumo = std::stod(valStr);

      double limite = limitePadrao;
      {
        std::lock_guard<std::mutex> lock(mtx);
        if (limitesConsumo.count(id)) {
          limite = limitesConsumo[id];
        }
      }

      if (consumo > limite) {
        // RF20: Disparar notificação
        std::stringstream ss;
        ss << "ALERTA: Hidrometro " << id
           << " excedeu o limite de consumo! Atual: " << consumo
           << ", Limite: " << limite;

        std::string dest;
        {
          std::lock_guard<std::mutex> lock(mtx);
          dest = destinatarioAlerta;
        }
        notificacaoService.notificar(dest, ss.str());
      }

    } catch (...) {
      std::cerr << "[AlertService] Erro ao processar mensagem de update: "
                << mensagem << std::endl;
    }
  }
}
