#ifndef IMONITORAMENTO_MEDIATOR_HPP
#define IMONITORAMENTO_MEDIATOR_HPP

// Padrão Mediator: Interface
class IMonitoramentoMediator {
public:
    virtual ~IMonitoramentoMediator() = default;
    virtual void notificarLeitura(int idSHA, int volumeAtual, int ultimaLeitura) = 0;
};

#endif