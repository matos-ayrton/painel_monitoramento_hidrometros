#include "PerfilManager.hpp"
#include "PerfilFactory.hpp"

#include <functional>
#include <iostream>

PerfilManager::PerfilManager(DB &dbRef) : db(dbRef) {
  db.ensureSchema();
  // Garante que o Gerente Principal 'GESTOR' exista ao iniciar o sistema.
  if (!db.userExists("GESTOR")) {
    // Senha '123' (hash: 1215167660, se usar std::hash)
    db.addUser("GESTOR", hashSenha("123"), "gerente", true);
    std::cout << "[PerfilManager] Usuário GESTOR inicial criado. Senha: 123"
              << std::endl;
  }
}

std::string PerfilManager::hashSenha(const std::string &s) const {
  // Implementação do hash (NÃO É CRIPTOGRAFICAMENTE SEGURO)
  auto h = std::hash<std::string>{}(s);
  return std::to_string(h);
}

// NOVO: Função de autenticação unificada que retorna o IPerfil (Factory Method)
std::unique_ptr<IPerfil> PerfilManager::autenticar(const std::string &matricula,
                                                   const std::string &senha) {
  auto storedData = db.getUserHashAndRole(matricula);
  if (!storedData.has_value()) {
    return nullptr; // Usuário não existe ou está inativo
  }

  // Desempacotamento da tupla (hash e role)
  const auto &[storedHash, storedRole] = storedData.value();

  if (storedHash == hashSenha(senha)) {
    // Factory Method Call
    return PerfilFactory::criarPerfil(storedRole);
  }

  return nullptr; // Senha incorreta
}

// RF02: Registro de Administradores
bool PerfilManager::registrarAdministrador(const std::string &matricula,
                                           const std::string &senha) {
  // SIMULAÇÃO RF02: Simula verificação de matrícula no "BD de Funcionários
  // Externo"
  if (matricula.empty() || matricula.length() < 3) {
    std::cerr << "[PerfilManager] Matrícula inválida para registro (Simulação "
                 "de validação)."
              << std::endl;
    return false;
  }

  std::string senhaHash = hashSenha(senha);
  return db.addUser(matricula, senhaHash, "administrador", true);
}

// RF04: Desligar Acesso de Administrador
bool PerfilManager::desligarAdministrador(const std::string &matricula) {
  if (!db.userExists(matricula))
    return false;

  // Restrição de Segurança: O Gerente Principal ('GESTOR') não pode ser
  // desativado
  if (matricula == "GESTOR") {
    std::cerr << "[PerfilManager] O Gerente Principal ('GESTOR') não pode ser "
                 "desativado."
              << std::endl;
    return false;
  }
  return db.setUserActive(matricula, false);
}

// Nova: Alteração de Senha
bool PerfilManager::alterarSenha(const std::string &matricula,
                                 const std::string &novaSenha) {
  if (!db.userExists(matricula))
    return false;
  std::string novoHash = hashSenha(novaSenha);
  return db.updateUserPassword(matricula, novoHash);
}