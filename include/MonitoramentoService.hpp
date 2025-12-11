#ifndef MONITORAMENTOSERVICE_HPP
#define MONITORAMENTOSERVICE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>
#include <memory> 
#include "MonitoramentoBanco.hpp"
#include "ILeituraStrategy.hpp" 
#include "IMonitoramentoMediator.hpp" 

namespace fs = std::filesystem;

class MonitoramentoService
{
public:
    struct ShaFolder {
        int idSHA;
        std::string pasta;
        std::unique_ptr<ILeituraStrategy> strategy; // Strategy Pattern
    };

    MonitoramentoService(std::vector<ShaFolder> pastas,
               IMonitoramentoMediator& mediatorRef,
               MonitoramentoBanco& bancoRef); 

    void iniciar();

    // Habilita/desabilita saída verbose para debug (imprime varredura e leituras)
    void setVerbose(bool ativo);

private:
    std::vector<ShaFolder> pastasSHA;
    std::unordered_map<std::string, fs::file_time_type> arquivosLidos;
    
    IMonitoramentoMediator& mediator; 
    MonitoramentoBanco& banco; 
    bool verbose{false};
};

#endif