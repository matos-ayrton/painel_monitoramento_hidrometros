#ifndef FACHADA_HPP
#define FACHADA_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <thread>
#include "MonitoramentoBanco.hpp"
#include "MonitoramentoService.hpp"

class Fachada {
public:
    Fachada();
    ~Fachada();

    // Iniciar o monitoramento em uma thread separada
    void iniciarMonitoramentoBackground();

    // Parar o monitoramento
    void pararMonitoramento();

    // Consultar consumo por ID do hidrometro
    int obterConsumo(int idSHA);

    // Consultar última leitura absoluta por ID do hidrometro
    int obterUltimaLeitura(int idSHA);

    // Listar todos os hidrometros registrados
    std::vector<int> listarHidrometros();

    // Verificar se o monitoramento está ativo
    bool estaAtivo();

private:
    std::unique_ptr<MonitoramentoBanco> banco;
    std::unique_ptr<MonitoramentoService> monitor;
    std::unique_ptr<std::thread> threadMonitoramento;
    bool ativo;

    // Configurar as pastas e ROI dos hidrometros
    std::vector<MonitoramentoService::ShaFolder> configurarPastas();
};

#endif
