#include "Logger.hpp"
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

// ==================== IMPLEMENTAÇÃO DO LOGGER (SINGLETON) ====================

// 1. Inicialização do membro estático (Variável global da instância)
Logger *Logger::instance = nullptr;

// 2. Implementação do Construtor Privado: Injeta a dependência do DB
Logger::Logger(DB &dbInstance) : db(dbInstance) {
  // Se quiser garantir o schema aqui, embora a Fachada já faça:
  // db.ensureLogSchema();
}

// 3. Implementação do Método de Acesso Singleton (getInstance)
// ESTA ERA A DEFINIÇÃO QUE ESTAVA FALTANDO AO LINKER:
Logger &Logger::getInstance(DB &dbInstance) {
  if (instance == nullptr) {
    // Chamada ao construtor privado
    instance = new Logger(dbInstance);
  }
  return *instance;
}

Logger &Logger::getInstance() {
  if (instance == nullptr) {
    throw std::runtime_error("Logger instance not initialized!");
  }
  return *instance;
}

// 4. Funções Auxiliares

std::string Logger::getCurrentTimestamp() {
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::stringstream ss;
  // Formato YYYY-MM-DD HH:MM:SS
  ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %X");
  return ss.str();
}

std::string Logger::getLogTypeString(LogType type) {
  switch (type) {
  case LogType::AUDIT_AUTH:
    return "AUDIT_AUTH";
  case LogType::AUDIT_CRUD:
    return "AUDIT_CRUD";
  case LogType::SYSTEM_ERROR:
    return "SYSTEM_ERROR";
  }
  return "UNKNOWN";
}

std::string Logger::getLogLevelString(LogLevel level) {
  switch (level) {
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARNING:
    return "WARNING";
  case LogLevel::ERROR:
    return "ERROR";
  case LogLevel::CRITICAL:
    return "CRITICAL";
  }
  return "UNKNOWN";
}

// 5. Implementação dos Métodos de Log (RF14, RF15, RF16)

// 5. Implementação dos Métodos de Log (RF14, RF15, RF16)

// RF14: Registrar tentativa/sucesso de autenticação
void Logger::logAuthentication(const std::string &adminMatricula, bool success,
                               const std::string &ipOrigem) {
  std::string status = success ? "SUCESSO" : "FALHA";
  std::string mensagem = "Tentativa de login por " + adminMatricula +
                         ". Status: " + status + ". IP: " + ipOrigem;
  std::string nivel = success ? "INFO" : "WARNING";

  db.saveLog("AUDIT_AUTH", nivel, getCurrentTimestamp(), adminMatricula,
             "Autenticacao", mensagem);
}

// RF15: Registrar operações CRUD de usuários
void Logger::logUserOperation(const std::string &adminMatricula,
                              const std::string &operacao,
                              const std::string &detalhesAfetados) {
  std::string mensagem =
      "Operação CRUD: " + operacao + ". Detalhes afetados: " + detalhesAfetados;

  db.saveLog("AUDIT_CRUD", "INFO", getCurrentTimestamp(), adminMatricula,
             "Gerenciamento_Usuario", mensagem);
}

// RF16: Capturar e armazenar logs de erro
void Logger::logSystemError(LogLevel level, const std::string &origem,
                            const std::string &mensagem) {
  std::string levelStr = getLogLevelString(level);

  // Saída imediata para stderr
  std::cerr << "[" << levelStr << " - " << origem << "] " << mensagem
            << std::endl;

  db.saveLog("SYSTEM_ERROR", levelStr, getCurrentTimestamp(), "SISTEMA", origem,
             mensagem);
}