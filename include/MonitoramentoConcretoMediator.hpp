#ifndef MONITORAMENTO_CONCRETO_MEDIATOR_HPP
#define MONITORAMENTO_CONCRETO_MEDIATOR_HPP

#include "IMonitoramentoMediator.hpp"
#include "MonitoramentoBanco.hpp" 

class MonitoramentoConcretoMediator : public IMonitoramentoMediator {
private:
    MonitoramentoBanco& banco; 

public:
    MonitoramentoConcretoMediator(MonitoramentoBanco& b) : banco(b) {}
    void notificarLeitura(int idSHA, int volumeAtual, int ultimaLeitura) override;
};

#endif