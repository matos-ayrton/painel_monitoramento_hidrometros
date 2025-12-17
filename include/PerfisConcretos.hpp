#ifndef PERFIS_CONCRETOS_HPP
#define PERFIS_CONCRETOS_HPP

#include "IPerfil.hpp"

// Concrete Product: Gerente
class Gerente : public IPerfil {
public:
  std::string getRole() const override { return "gerente"; }
  bool podeGerenciarAdministradores() const override {
    return true;
  } // RF02, RF04
  bool podeVisualizarLogs() const override { return true; }
};

// Concrete Product: Administrador
class Administrador : public IPerfil {
public:
  std::string getRole() const override { return "administrador"; }
  bool podeGerenciarAdministradores() const override { return false; }
  bool podeVisualizarLogs() const override {
    return false;
  } // Geralmente admin vê logs operacionais, mas gerente vê tudo. Ajustável.
};

#endif // PERFIS_CONCRETOS_HPP
