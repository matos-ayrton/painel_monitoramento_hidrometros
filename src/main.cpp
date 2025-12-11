#include "CLI.hpp"
#include "Fachada.hpp" 
#include <iostream>

int main() {
    try {
        std::cout << "--- Inicialização do Sistema (MAIN) ---" << std::endl;

        // Obtém a instância Singleton da Fachada
        Fachada& fachadaInstance = Fachada::getInstance(); 
        
        // Instancia o CLI, passando a REFERÊNCIA da Fachada
        CLI cli(fachadaInstance); 
        
        cli.executar();
        
    } catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}