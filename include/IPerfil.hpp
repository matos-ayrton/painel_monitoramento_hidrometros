#ifndef IPERFIL_HPP
#define IPERFIL_HPP

#include <string>
#include <vector>

// Interface comum para os perfis (Produto do Factory Method)
class IPerfil {
public:
  virtual ~IPerfil() = default;

  virtual std::string getRole() const = 0;
  virtual bool podeGerenciarAdministradores() const = 0;
  virtual bool podeVisualizarLogs() const = 0;
};

#endif // IPERFIL_HPP
