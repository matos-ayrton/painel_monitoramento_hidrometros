#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "DB.hpp" // Depende do DB para persistência
#include <memory>
#include <string>

// Nível do Log de Erro (RF16) - Mantenho a estrutura que você usou
enum class LogLevel {
  WARNING,
  ERROR,
  CRITICAL,
  INFO // Adicionei INFO para logs de sucesso (RF14, RF15)
};

// Tipo de Log (RF14 e RF15) - Mantenho a estrutura que você usou
enum class LogType { AUDIT_AUTH, AUDIT_CRUD, SYSTEM_ERROR };

class Logger {
private:
  static Logger *instance;
  DB &db; // CORRIGIDO: Referência à instância do DB, não unique_ptr

  // Construtor privado para Singleton
  Logger(DB &dbInstance); // CORRIGIDO: Recebe DB&

  // Funções auxiliares
  std::string getCurrentTimestamp();
  std::string getLogTypeString(LogType type);
  std::string getLogLevelString(LogLevel level);

public:
  // Destrutor e exclusão de cópia para garantir Singleton
  ~Logger() = default;
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  // Método de acesso Singleton
  static Logger &getInstance(DB &dbInstance); // CORRIGIDO: Recebe DB&
  static Logger &getInstance();               // Parameterless accessor

  // RF14: Registrar tentativa/sucesso de autenticação
  void logAuthentication(const std::string &adminMatricula, bool success,
                         const std::string &ipOrigem = "");

  // RF15: Registrar operações CRUD de usuários
  void logUserOperation(const std::string &adminMatricula,
                        const std::string &operacao,
                        const std::string &detalhesAfetados);

  // RF16: Capturar e armazenar logs de erro
  void logSystemError(LogLevel level, const std::string &origem,
                      const std::string &mensagem);
};

#endif