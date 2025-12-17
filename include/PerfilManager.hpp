#ifndef PERFILMANAGER_HPP
#define PERFILMANAGER_HPP

#include "DB.hpp"
#include "IPerfil.hpp"
#include <memory>
#include <optional>
#include <string>
#include <tuple>

class PerfilManager {
private:
  DB &db;
  std::string hashSenha(const std::string &s) const;

public:
  PerfilManager(DB &dbRef);

  // Factory Method usage: Returns an abstract IPerfil
  std::unique_ptr<IPerfil> autenticar(const std::string &login,
                                      const std::string &senha);

  // Funções de gerenciamento
  bool registrarAdministrador(const std::string &matricula,
                              const std::string &senha);    // RF02
  bool desligarAdministrador(const std::string &matricula); // RF04
  bool alterarSenha(const std::string &matricula,
                    const std::string &novaSenha); // Nova
};

#endif