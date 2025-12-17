#include "DB.hpp"
#include "User.hpp"
#include <algorithm>
#include <cmath> // Para operações matemáticas
#include <iostream>
#include <sqlite3.h>
#include <sstream>
#include <stdexcept>
#include <utility>
#include <vector>

DB::DB(const std::string &path) {
  if (sqlite3_open_v2(path.c_str(), &db_,
                      SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                      nullptr) != SQLITE_OK) {
    std::cerr << "[DB] Falha ao abrir DB: " << sqlite3_errmsg(db_) << std::endl;
    sqlite3_close(db_);
    db_ = nullptr;
  } else {
    exec("PRAGMA foreign_keys = ON;");
  }
}

DB::~DB() {
  if (db_)
    sqlite3_close(db_);
}

bool DB::exec(const std::string &sql) {
  if (!db_)
    return false;
  char *err = nullptr;
  int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &err);
  if (rc != SQLITE_OK) {
    std::cerr << "[DB] SQL error: " << (err ? err : "unknown") << " | Query: "
              << sql.substr(0, std::min(sql.length(), (size_t)80)) << "..."
              << std::endl;
    sqlite3_free(err);
    return false;
  }
  return true;
}

std::optional<std::string> DB::querySingle(const std::string &sql) {
  if (!db_)
    return std::nullopt;
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    return std::nullopt;
  }
  std::optional<std::string> res;
  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    const unsigned char *text = sqlite3_column_text(stmt, 0);
    if (text)
      res = std::string(reinterpret_cast<const char *>(text));
  }
  sqlite3_finalize(stmt);
  return res;
}

// ===============================================
// Funções de Esquema (RF05, RF09, RF06/RF12)
// ===============================================

bool DB::ensureSchema() {
  // Tabela 1: Perfis (Admin/Gerente)
  const char *sql_users = "CREATE TABLE IF NOT EXISTS users ("
                          "matricula TEXT PRIMARY KEY,"
                          "senha_hash TEXT NOT NULL,"
                          "role TEXT NOT NULL,"
                          "ativo INTEGER NOT NULL DEFAULT 1"
                          ");";

  // Tabela 2: Clientes (Usuários Finais) - RF05
  const char *sql_clients = "CREATE TABLE IF NOT EXISTS clients ("
                            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                            "nome TEXT NOT NULL,"
                            "cpf TEXT UNIQUE NOT NULL,"
                            "endereco TEXT NOT NULL,"
                            "telefone TEXT NOT NULL,"
                            "email TEXT UNIQUE NOT NULL"
                            ");";

  // Tabela 3: Ligação Clientes-Hidrômetros - RF09
  const char *sql_links =
      "CREATE TABLE IF NOT EXISTS user_hydrometer_links ("
      "client_id INTEGER NOT NULL,"
      "hydrometer_id INTEGER NOT NULL,"
      "PRIMARY KEY (client_id, hydrometer_id),"
      "FOREIGN KEY (client_id) REFERENCES clients(id) ON DELETE CASCADE"
      ");";

  // Tabela 4: Leituras do Hidrômetro (Base para Gasto Total - RF12)
  const char *sql_readings =
      "CREATE TABLE IF NOT EXISTS hydrometer_readings ("
      "reading_id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "hydrometer_id INTEGER NOT NULL,"
      "reading_date TEXT NOT NULL,"
      "reading_value REAL NOT NULL,"
      "FOREIGN KEY (hydrometer_id) REFERENCES hydrometers(id) ON DELETE CASCADE"
      ");";

  // Tabela 5: Histórico de Contas (Base para UserBill - RF06)
  const char *sql_billing =
      "CREATE TABLE IF NOT EXISTS billing_history ("
      "bill_id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "client_id INTEGER NOT NULL,"
      "due_date TEXT NOT NULL,"
      "amount REAL NOT NULL,"
      "FOREIGN KEY (client_id) REFERENCES clients(id) ON DELETE CASCADE"
      ");";

  // Executa as queries
  return exec(sql_users) && exec(sql_clients) && exec(sql_links) &&
         exec(sql_readings) && exec(sql_billing);
}

// ===============================================
// Funções de Perfis (Admin/Gerente)
// ===============================================

std::optional<std::tuple<std::string, std::string>>
DB::getUserHashAndRole(const std::string &matricula) {
  if (!db_)
    return std::nullopt;
  std::string sql = "SELECT senha_hash, role FROM users WHERE matricula = ? "
                    "AND ativo = 1 LIMIT 1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "[DB] Prepare error (getUserHashAndRole): "
              << sqlite3_errmsg(db_) << std::endl;
    return std::nullopt;
  }
  sqlite3_bind_text(stmt, 1, matricula.c_str(), -1, SQLITE_TRANSIENT);
  std::optional<std::tuple<std::string, std::string>> res;
  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    const unsigned char *hashText = sqlite3_column_text(stmt, 0);
    const unsigned char *roleText = sqlite3_column_text(stmt, 1);
    if (hashText && roleText) {
      res = std::make_tuple(
          std::string(reinterpret_cast<const char *>(hashText)),
          std::string(reinterpret_cast<const char *>(roleText)));
    }
  }
  sqlite3_finalize(stmt);
  return res;
}

bool DB::addUser(const std::string &matricula, const std::string &senha_hash,
                 const std::string &role, bool ativo) {
  if (!db_)
    return false;
  const char *sql = "INSERT OR REPLACE INTO users(matricula, senha_hash, role, "
                    "ativo) VALUES(?, ?, ?, ?);";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(stmt, 1, matricula.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 2, senha_hash.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, role.c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_int(stmt, 4, ativo ? 1 : 0);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE;
}

bool DB::removeUser(const std::string &matricula) {
  if (!db_)
    return false;
  const char *sql = "DELETE FROM users WHERE matricula = ?;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(stmt, 1, matricula.c_str(), -1, SQLITE_TRANSIENT);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE;
}

bool DB::setUserActive(const std::string &matricula, bool ativo) {
  if (!db_)
    return false;
  const char *sql = "UPDATE users SET ativo = ? WHERE matricula = ?;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_int(stmt, 1, ativo ? 1 : 0);
  sqlite3_bind_text(stmt, 2, matricula.c_str(), -1, SQLITE_TRANSIENT);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE;
}

bool DB::userExists(const std::string &matricula) {
  if (!db_)
    return false;
  const char *sql = "SELECT 1 FROM users WHERE matricula = ? LIMIT 1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_text(stmt, 1, matricula.c_str(), -1, SQLITE_TRANSIENT);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_ROW;
}

// ===============================================
// Funções de Usuários/Clientes (RF05 a RF09 e RF06/RF12)
// ===============================================

// RF05: Retorna o próximo ID auto-incremento
int DB::getNewUserId() {
  const char *sql = "SELECT MAX(id) FROM clients;";
  sqlite3_stmt *stmt = nullptr;
  int nextId = 1;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) == SQLITE_OK) {
    if (sqlite3_step(stmt) == SQLITE_ROW) {
      if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
        nextId = sqlite3_column_int(stmt, 0) + 1;
      } else {
        nextId = 1;
      }
    }
  }
  sqlite3_finalize(stmt);
  return nextId;
}

// RF05: Insere novo usuário/cliente
bool DB::insertUser(const std::unique_ptr<User> &user) {
  if (!db_ || !user)
    return false;
  const char *sql = "INSERT INTO clients (id, nome, cpf, endereco, telefone, "
                    "email) VALUES (?, ?, ?, ?, ?, ?);";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "[DB] Prepare error (insertUser): " << sqlite3_errmsg(db_)
              << std::endl;
    return false;
  }
  sqlite3_bind_int(stmt, 1, user->getId());
  sqlite3_bind_text(stmt, 2, user->getNome().c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 3, user->getCpf().c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 4, user->getEndereco().c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 5, user->getTelefone().c_str(), -1, SQLITE_TRANSIENT);
  sqlite3_bind_text(stmt, 6, user->getEmail().c_str(), -1, SQLITE_TRANSIENT);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  if (rc != SQLITE_DONE) {
    std::cerr << "[DB] Insert error (insertUser): " << sqlite3_errmsg(db_)
              << " (CPF/Email duplicado?) " << std::endl;
    return false;
  }
  return true;
}

// RF06: Consulta a existência do cliente
bool DB::clientExists(int id) {
  if (!db_)
    return false;
  const char *sql = "SELECT 1 FROM clients WHERE id = ? LIMIT 1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_int(stmt, 1, id);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_ROW;
}

// RF06: Consulta dados básicos do cliente
std::optional<
    std::tuple<std::string, std::string, std::string, std::string, std::string>>
DB::queryUserBaseData(int id) {
  if (!db_)
    return std::nullopt;
  const char *sql = "SELECT nome, cpf, endereco, telefone, email FROM clients "
                    "WHERE id = ? LIMIT 1;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "[DB] Prepare error (queryUserBaseData): "
              << sqlite3_errmsg(db_) << std::endl;
    return std::nullopt;
  }
  sqlite3_bind_int(stmt, 1, id);
  std::optional<std::tuple<std::string, std::string, std::string, std::string,
                           std::string>>
      res;
  int rc = sqlite3_step(stmt);
  if (rc == SQLITE_ROW) {
    res = std::make_tuple(std::string(reinterpret_cast<const char *>(
                              sqlite3_column_text(stmt, 0))), // nome
                          std::string(reinterpret_cast<const char *>(
                              sqlite3_column_text(stmt, 1))), // cpf
                          std::string(reinterpret_cast<const char *>(
                              sqlite3_column_text(stmt, 2))), // endereco
                          std::string(reinterpret_cast<const char *>(
                              sqlite3_column_text(stmt, 3))), // telefone
                          std::string(reinterpret_cast<const char *>(
                              sqlite3_column_text(stmt, 4))) // email
    );
  }
  sqlite3_finalize(stmt);
  return res;
}

// RF06: Consulta os IDs de hidrômetros vinculados
std::vector<int> DB::queryUserHydrometers(int userId) {
  std::vector<int> hydrometerIds;
  if (!db_)
    return hydrometerIds;
  const char *sql =
      "SELECT hydrometer_id FROM user_hydrometer_links WHERE client_id = ?;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "[DB] Prepare error (queryUserHydrometers): "
              << sqlite3_errmsg(db_) << std::endl;
    return hydrometerIds;
  }
  sqlite3_bind_int(stmt, 1, userId);
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int hId = sqlite3_column_int(stmt, 0);
    hydrometerIds.push_back(hId);
  }
  sqlite3_finalize(stmt);
  return hydrometerIds;
}

// RF06: Consulta o histórico de faturas/contas
std::vector<UserBill> DB::queryUserBills(int userId) {
  std::vector<UserBill> bills;
  if (!db_)
    return bills;

  const char *sql = "SELECT due_date, amount FROM billing_history WHERE "
                    "client_id = ? ORDER BY due_date DESC;";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "[DB] Prepare error (queryUserBills): " << sqlite3_errmsg(db_)
              << std::endl;
    return bills;
  }

  sqlite3_bind_int(stmt, 1, userId);

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    UserBill bill;

    const unsigned char *dateText = sqlite3_column_text(stmt, 0);
    if (dateText)
      bill.dataVencimento =
          std::string(reinterpret_cast<const char *>(dateText));

    bill.valorGasto = sqlite3_column_double(stmt, 1);

    bills.push_back(std::move(bill));
  }

  sqlite3_finalize(stmt);
  return bills;
}

// RF06/RF12: Consulta o gasto total de água (consumo acumulado)
double DB::queryUserGastoTotal(int userId) {
  if (!db_)
    return 0.0;

  const char *sql = "SELECT SUM(T2.max_reading - T2.min_reading) "
                    "FROM user_hydrometer_links T1 "
                    "JOIN ( "
                    "    SELECT hydrometer_id, MAX(reading_value) AS "
                    "max_reading, MIN(reading_value) AS min_reading "
                    "    FROM hydrometer_readings "
                    "    GROUP BY hydrometer_id "
                    ") T2 ON T1.hydrometer_id = T2.hydrometer_id "
                    "WHERE T1.client_id = ?;";

  sqlite3_stmt *stmt = nullptr;
  double totalGasto = 0.0;

  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "[DB] Prepare error (queryUserGastoTotal): "
              << sqlite3_errmsg(db_) << std::endl;
    return 0.0;
  }

  sqlite3_bind_int(stmt, 1, userId);

  if (sqlite3_step(stmt) == SQLITE_ROW) {
    if (sqlite3_column_type(stmt, 0) != SQLITE_NULL) {
      totalGasto = sqlite3_column_double(stmt, 0);
    }
  }

  sqlite3_finalize(stmt);
  return totalGasto;
}

// RF07: Atualiza informações de contato do cliente
bool DB::updateUserInfo(int id, const std::string &novoEmail,
                        const std::string &novoEndereco,
                        const std::string &novoTelefone) {
  if (!db_)
    return false;
  std::string sql = "UPDATE clients SET ";
  std::vector<std::pair<std::string, const std::string *>> updates;

  if (!novoEmail.empty())
    updates.emplace_back("email = ?", &novoEmail);
  if (!novoEndereco.empty())
    updates.emplace_back("endereco = ?", &novoEndereco);
  if (!novoTelefone.empty())
    updates.emplace_back("telefone = ?", &novoTelefone);

  if (updates.empty())
    return false;

  for (size_t i = 0; i < updates.size(); ++i) {
    sql += updates[i].first;
    if (i < updates.size() - 1)
      sql += ", ";
  }
  sql += " WHERE id = ?;";

  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "[DB] Prepare error (updateUserInfo): " << sqlite3_errmsg(db_)
              << std::endl;
    return false;
  }

  int bind_index = 1;
  for (const auto &update : updates) {
    sqlite3_bind_text(stmt, bind_index++, update.second->c_str(), -1,
                      SQLITE_TRANSIENT);
  }
  sqlite3_bind_int(stmt, bind_index, id);

  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if (rc != SQLITE_DONE) {
    std::cerr << "[DB] Update error (updateUserInfo): " << sqlite3_errmsg(db_)
              << std::endl;
    return false;
  }
  return sqlite3_changes(db_) > 0;
}

// RF08: Remove o cliente
bool DB::deleteUser(int id) {
  if (!db_)
    return false;
  const char *sql = "DELETE FROM clients WHERE id = ?;";
  sqlite3_stmt *stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK)
    return false;
  sqlite3_bind_int(stmt, 1, id);
  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);
  return rc == SQLITE_DONE && sqlite3_changes(db_) > 0;
}

// RF09: Cria o vínculo cliente-hidrômetro
bool DB::linkHydrometer(int userId, int hidrometroId) {
  if (!db_)
    return false;
  const char *sql = "INSERT OR IGNORE INTO user_hydrometer_links (client_id, "
                    "hydrometer_id) VALUES (?, ?);";
  sqlite3_stmt *stmt = nullptr;

  if (sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr) != SQLITE_OK) {
    std::cerr << "[DB] Prepare error (linkHydrometer): " << sqlite3_errmsg(db_)
              << std::endl;
    return false;
  }
  sqlite3_bind_int(stmt, 1, userId);
  sqlite3_bind_int(stmt, 2, hidrometroId);

  int rc = sqlite3_step(stmt);
  sqlite3_finalize(stmt);

  if (rc != SQLITE_DONE) {
    std::cerr << "[DB] Insert error (linkHydrometer): " << sqlite3_errmsg(db_)
              << std::endl;
    return false;
  }
  return sqlite3_changes(db_) > 0;
}

bool DB::ensureLogSchema() {
  std::string sql = R"(
        CREATE TABLE IF NOT EXISTS logs (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            tipo TEXT NOT NULL,         -- AUDIT_AUTH, AUDIT_CRUD, SYSTEM_ERROR
            nivel TEXT NOT NULL,        -- INFO, WARNING, ERROR, CRITICAL
            timestamp DATETIME DEFAULT CURRENT_TIMESTAMP,
            responsavel TEXT NOT NULL,  -- Matricula ou 'SISTEMA'
            origem TEXT NOT NULL,       -- Autenticacao, Gerenciamento_Usuario, MonitoramentoService, etc.
            mensagem TEXT               -- Detalhes do evento
        );
    )";
  if (exec(sql)) {
    std::cout << "[DB] Tabela 'logs' verificada/criada com sucesso."
              << std::endl;
    return true;
  }
  std::cerr << "[DB ERROR] Falha ao criar tabela de logs." << std::endl;
  return false;
}

// RF14, RF15, RF16: Salva um registro de log
bool DB::saveLog(const std::string &tipo, const std::string &nivel,
                 const std::string &timestamp, const std::string &responsavel,
                 const std::string &origem, const std::string &mensagem) {

  // Simples prevenção de injeção SQL, idealmente use prepared statements
  // (sqlite3_prepare_v2)
  auto safe_msg = [](const std::string &s) {
    std::string temp = s;
    std::replace(temp.begin(), temp.end(), '\'',
                 ' '); // Substitui aspas simples
    return temp;
  };

  std::stringstream sql;
  sql << "INSERT INTO logs (tipo, nivel, timestamp, responsavel, origem, "
         "mensagem) VALUES ("
      << "'" << safe_msg(tipo) << "', "
      << "'" << safe_msg(nivel) << "', "
      << "'" << safe_msg(timestamp) << "', "
      << "'" << safe_msg(responsavel) << "', "
      << "'" << safe_msg(origem) << "', "
      << "'" << safe_msg(mensagem) << "');";

  bool success = exec(sql.str());

  if (!success) {
    std::cerr << "[DB ERROR] Falha ao salvar log: " << tipo << "/" << origem
              << std::endl;
  }
  return success;
}

// Função de callback para sqlite3_exec (utilizada no fetchLogs)
static int log_callback(void *data, int argc, char **argv, char **azColName) {
  std::vector<DB::SystemLog> *logs =
      static_cast<std::vector<DB::SystemLog> *>(data);
  DB::SystemLog log;

  for (int i = 0; i < argc; i++) {
    std::string colName = azColName[i];
    std::string colValue = (argv[i] ? argv[i] : "");

    if (colName == "id")
      log.id = std::stoi(colValue);
    else if (colName == "tipo")
      log.tipo = colValue;
    else if (colName == "nivel")
      log.nivel = colValue;
    else if (colName == "timestamp")
      log.timestamp = colValue;
    else if (colName == "responsavel")
      log.responsavel = colValue;
    else if (colName == "origem")
      log.origem = colValue;
    else if (colName == "mensagem")
      log.mensagem = colValue;
  }
  logs->push_back(std::move(log));
  return 0;
}

// RF17: Consulta logs (Implementação Real)
std::vector<DB::SystemLog> DB::fetchLogs(int limit) {
  std::vector<SystemLog> logs;
  std::stringstream sql;
  char *zErrMsg = nullptr;

  sql << "SELECT id, tipo, nivel, timestamp, responsavel, origem, mensagem "
         "FROM logs "
      << "ORDER BY timestamp DESC " // Logs mais recentes primeiro
      << "LIMIT " << limit << ";";

  // Assumindo que 'db_' é o ponteiro sqlite3*
  int rc = sqlite3_exec(db_, sql.str().c_str(), log_callback, &logs, &zErrMsg);

  if (rc != SQLITE_OK) {
    std::cerr << "[DB ERROR] Falha na consulta de logs: " << zErrMsg
              << std::endl;
    sqlite3_free(zErrMsg);
    // Em caso de erro, retornar lista vazia
    return {};
  }
  return logs;
}