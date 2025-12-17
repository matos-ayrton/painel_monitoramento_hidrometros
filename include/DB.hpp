#ifndef DB_HPP
#define DB_HPP

#include "User.hpp" // Necessário para User, UserBill e unique_ptr<User>
#include <memory>
#include <optional>
#include <sqlite3.h>
#include <string>
#include <tuple>
#include <vector>

class DB {
private:
  sqlite3 *db_;
  std::optional<std::string> querySingle(const std::string &sql);

public:
  DB(const std::string &path);
  ~DB();
  bool exec(const std::string &sql);

  // ===============================================
  // Funções de Esquema
  // ===============================================
  bool ensureSchema();

  // ===============================================
  // Funções de Perfis (Administrativo)
  // ===============================================
  bool addUser(const std::string &matricula, const std::string &senha_hash,
               const std::string &role, bool ativo);
  bool removeUser(const std::string &matricula);
  bool setUserActive(const std::string &matricula, bool ativo);
  bool userExists(const std::string &matricula);
  std::optional<std::tuple<std::string, std::string>>
  getUserHashAndRole(const std::string &matricula);

  // ===============================================
  // Funções de Usuários/Clientes (RF05 a RF09 e RF06/RF12)
  // ===============================================

  // RF05: Cadastro
  int getNewUserId();
  bool insertUser(const std::unique_ptr<User> &user);

  // RF06: Consulta
  bool clientExists(int id);
  std::vector<int> queryUserHydrometers(int userId);
  // Retorna (nome, cpf, endereco, telefone, email)
  std::optional<std::tuple<std::string, std::string, std::string, std::string,
                           std::string>>
  queryUserBaseData(int id);

  // RF06/RF12: CONSULTAS DE DADOS VINCULADOS
  std::vector<UserBill> queryUserBills(int userId);
  double queryUserGastoTotal(int userId);

  // RF07: Atualização
  bool updateUserInfo(int id, const std::string &novoEmail,
                      const std::string &novoEndereco,
                      const std::string &novoTelefone);

  // RF08: Remoção
  bool deleteUser(int id);

  // RF09: Vinculação
  bool linkHydrometer(int userId, int hidrometroId);

  bool isReady() const {
    return db_ != nullptr;
  } // Implementação real depende do seu DB.cpp

  // RF16: Garante que a tabela de logs exista
  bool ensureLogSchema();

  // RF14, RF15, RF16: Salva um registro de log
  bool saveLog(const std::string &tipo, const std::string &nivel,
               const std::string &timestamp, const std::string &responsavel,
               const std::string &origem, const std::string &mensagem);

  struct SystemLog {
    int id;
    std::string tipo;
    std::string nivel;
    std::string timestamp;
    std::string responsavel;
    std::string origem;
    std::string mensagem;
  };

  // RF17: Consulta logs
  std::vector<SystemLog> fetchLogs(int limit = 20);
};

#endif // DB_HPP