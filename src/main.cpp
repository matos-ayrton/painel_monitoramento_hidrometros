#include "Fachada.hpp"
#include "CLI.hpp"
#include <iostream>

int main() {
    try {
        auto fachada = std::make_unique<Fachada>();
        CLI cli(std::move(fachada));
        cli.executar();
    } catch (const std::exception& e) {
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}