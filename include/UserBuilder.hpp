#ifndef USERBUILDER_HPP
#define USERBUILDER_HPP

#include "User.hpp"
#include <memory>

class UserBuilder {
private:
    int id = -1;
    std::string nome;
    std::string cpf;
    std::string endereco;
    std::string telefone;
    std::string email;

public:
    UserBuilder() = default;

    // Métodos de construção fluent (retorna a própria referência)
    UserBuilder& setId(int newId) { id = newId; return *this; }
    UserBuilder& setNome(const std::string& newNome) { nome = newNome; return *this; }
    UserBuilder& setCpf(const std::string& newCpf) { cpf = newCpf; return *this; }
    UserBuilder& setEndereco(const std::string& newEndereco) { endereco = newEndereco; return *this; }
    UserBuilder& setTelefone(const std::string& newTelefone) { telefone = newTelefone; return *this; }
    UserBuilder& setEmail(const std::string& newEmail) { email = newEmail; return *this; }

    // Método de finalização: Constrói e retorna o objeto User
    std::unique_ptr<User> build();
};

#endif // USERBUILDER_HPP