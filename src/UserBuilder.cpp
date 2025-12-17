#include "UserBuilder.hpp"
#include <stdexcept>
#include <utility>

// Implementação do construtor da classe Produto (User)
User::User(int id, std::string nome, std::string cpf, std::string endereco, 
           std::string telefone, std::string email)
    : id(id), nome(std::move(nome)), cpf(std::move(cpf)), endereco(std::move(endereco)),
      telefone(std::move(telefone)), email(std::move(email)),
      gastoAguaTotal(0.0) // Inicializa o total como zero
{}


// Implementação do método Build do Builder
std::unique_ptr<User> UserBuilder::build() {
    // Validação mínima dos campos obrigatórios (RF05)
    if (id <= 0 || nome.empty() || cpf.empty() || endereco.empty() || telefone.empty() || email.empty()) {
        throw std::invalid_argument("[UserBuilder] Todos os campos básicos de cadastro (ID, Nome, CPF, Endereço, Telefone, Email) são obrigatórios.");
    }

    // Retorna a instância única (unique_ptr) do User
    return std::make_unique<User>(id, nome, cpf, endereco, telefone, email);
}