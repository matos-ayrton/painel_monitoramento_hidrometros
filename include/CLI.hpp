#ifndef CLI_HPP
#define CLI_HPP

#include "Fachada.hpp"

class CLI {
public:
    CLI(Fachada& fachada);
    void executar();

private:
    Fachada& fachada; 

    void exibirMenu();
    void consultarConsumo();
    void consultarUltimaLeitura();
    void listarHidrometros();
    void exibirStatus();
};

#endif