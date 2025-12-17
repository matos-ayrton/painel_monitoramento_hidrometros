#ifndef USUARIOSERVICE_HPP
#define USUARIOSERVICE_HPP

#include "DB.hpp" // Assumindo que você tem uma classe DB para persistência
#include "User.hpp"
#include "UserBuilder.hpp"
#include <memory>
#include <optional>

class UsuarioService {
private:
  DB &db;
  // Função auxiliar que cria o objeto User a partir dos dados do BD
  std::unique_ptr<User> buildUserFromDB(int id);

public:
  UsuarioService(DB &dbRef);

  // RF05: Cria e registra um novo usuário (usa o Builder internamente)
  bool registerUser(const std::string &nome, const std::string &cpf,
                    const std::string &endereco, const std::string &telefone,
                    const std::string &email);

  // RF06: Consulta os dados completos do usuário
  std::optional<User> getUserData(int id);

  // RF07: Atualização de informações
  bool updateUserInfo(int id, const std::string &novoEmail,
                      const std::string &novoEndereco,
                      const std::string &novoTelefone);

  // RF08: Remoção de usuário e dados vinculados
  bool removeUser(int id);

  // RF09: Vinculação de hidrômetro
  bool linkHydrometerToUser(int userId, int hidrometroId);

  // RF09 (Estendido): Desvinculação de hidrômetro
  bool unlinkHydrometerFromUser(int userId, int hidrometroId);
};

#endif // USERMANAGER_HPP