#ifndef PERFIL_FACTORY_HPP
#define PERFIL_FACTORY_HPP

#include "IPerfil.hpp"
#include "PerfisConcretos.hpp"
#include <memory>
#include <string>

// Creator/Factory
class PerfilFactory {
public:
  static std::unique_ptr<IPerfil> criarPerfil(const std::string &tipo) {
    if (tipo == "gerente") {
      return std::make_unique<Gerente>();
    } else if (tipo == "administrador") {
      return std::make_unique<Administrador>();
    }
    return nullptr;
  }
};

#endif // PERFIL_FACTORY_HPP
