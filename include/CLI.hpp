#ifndef CLI_HPP
#define CLI_HPP

#include <memory>
#include "Fachada.hpp"

class CLI {
public:
    CLI(std::unique_ptr<Fachada> fachada);

    // Executar o loop interativo da CLI
    void executar();

private:
    std::unique_ptr<Fachada> fachada;

    void exibirMenu();
    void consultarConsumo();
    void consultarUltimaLeitura();
    void listarHidrometros();
    void exibirStatus();
};

#endif
