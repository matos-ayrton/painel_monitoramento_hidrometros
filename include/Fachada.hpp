#ifndef FACHADA_HPP
#define FACHADA_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include "MonitoramentoBanco.hpp"
#include "MonitoramentoService.hpp"
#include "IMonitoramentoMediator.hpp"
#include "IOcrAdapter.hpp"
// Inclusão de UsuarioService.hpp removida

class Fachada {
private:
    // Padrão Singleton
    static Fachada* instance; 
    Fachada();
    // Prevenção de cópia (essencial para Singleton)
    Fachada(const Fachada&) = delete;
    Fachada& operator=(const Fachada&) = delete;
    
    std::unique_ptr<MonitoramentoBanco> banco;
    std::unique_ptr<IMonitoramentoMediator> mediator; 
    std::shared_ptr<IOcrAdapter> ocrAdapter;
    std::unique_ptr<MonitoramentoService> monitor;
    // unique_ptr<UsuarioService> removido
    std::unique_ptr<std::thread> threadMonitoramento;
    bool ativo;

    std::vector<MonitoramentoService::ShaFolder> configurarPastas();

public:
    // Método de acesso estático
    static Fachada& getInstance();
    
    ~Fachada();

    void iniciarMonitoramentoBackground();
    void pararMonitoramento();
    
    // Ativa/desativa saída verbose (debug) do monitoramento
    void setVerbose(bool ativo);

    long long obterConsumo(int idSHA);
    int obterUltimaLeitura(int idSHA);
    std::vector<int> listarHidrometros();
    bool estaAtivo();
    
    // Método gerenciarUsuarios() removido
    void consultarLogsDeErro(); 
};

#endif