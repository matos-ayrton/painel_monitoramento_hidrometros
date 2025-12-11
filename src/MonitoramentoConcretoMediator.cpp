#include "MonitoramentoConcretoMediator.hpp"
#include <iostream>

void MonitoramentoConcretoMediator::notificarLeitura(int idSHA, int volumeAtual, int ultimaLeitura) {
    
    int delta_volume = 0;
    
    // CÁLCULO DO DELTA E ATUALIZAÇÃO DO CONSUMO
    if (volumeAtual > ultimaLeitura) {
        delta_volume = volumeAtual - ultimaLeitura;
        banco.adicionarVolumeGasto(idSHA, delta_volume);
    }
    
    // REGISTRA A NOVA LEITURA ABSOLUTA
    banco.registrarLeitura(idSHA, volumeAtual);

    // O Mediator coordenaria Alertas e Logs aqui.
}