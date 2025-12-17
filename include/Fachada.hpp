#ifndef FACHADA_HPP
#define FACHADA_HPP

#include "DB.hpp" // Contém DB::SystemLog
#include "IMonitoramentoMediator.hpp"
#include "IOcrAdapter.hpp"
#include "Logger.hpp" // CORRETO: Inclusão do Logger (que contém LogLevel)
#include "MonitoramentoBanco.hpp"
#include "MonitoramentoService.hpp"
#include "PerfilManager.hpp"
#include "User.hpp"
#include "UsuarioService.hpp"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

class Fachada {
private:
  // Padrão Singleton
  static Fachada *instance;
  Fachada(); // Construtor privado
  Fachada(const Fachada &) = delete;
  Fachada &operator=(const Fachada &) = delete;

  std::unique_ptr<MonitoramentoBanco> banco;
  std::unique_ptr<IMonitoramentoMediator> mediator;
  std::shared_ptr<IOcrAdapter> ocrAdapter;
  std::unique_ptr<MonitoramentoService> monitor;

  // Core do Subsistema de Persistência e Gerenciamento
  std::unique_ptr<DB> db;
  std::unique_ptr<PerfilManager> perfilManager;
  std::unique_ptr<UsuarioService> userManager; // Gerenciador de Clientes

  // NOVO: Subsistema de Logs e Auditoria
  Logger &logger; // Logger é um Singleton acessível globalmente, mantemos a
                  // referência

  std::unique_ptr<std::thread> threadMonitoramento;
  bool ativo;

  std::vector<MonitoramentoService::ShaFolder> configurarPastas();

  // Funções de autenticação antigas, mantidas privadas ou obsoletas
  bool autenticarGerente(const std::string &usuario, const std::string &senha);
  bool autenticarAdministrador(const std::string &matricula,
                               const std::string &senha);

public:
  // Método de acesso estático
  static Fachada &getInstance();

  ~Fachada();

  // ===============================================
  // SUBSISTEMA DE PERFIS (RF01, RF02, RF03, RF04, RF14, RF15)
  // ===============================================

  std::optional<std::string> autenticar(const std::string &matricula,
                                        const std::string &senha);
  bool registrarAdministrador(const std::string &matricula);
  bool desligarAdministrador(const std::string &matricula);

  // ===============================================
  // SUBSISTEMA DE USUÁRIOS (Clientes - RF05 a RF09 e RF15)
  // ===============================================

  // RF05: Cadastro
  bool cadastrarUsuario(const std::string &nome, const std::string &cpf,
                        const std::string &endereco,
                        const std::string &telefone, const std::string &email);

  // RF06: Consulta
  std::optional<User> consultarUsuario(int id);

  // RF07: Atualização (Simplificado para os campos principais)
  bool atualizarUsuario(int id, const std::string &novoEmail,
                        const std::string &novoEndereco,
                        const std::string &novoTelefone);

  // RF08: Remoção
  bool removerUsuario(int id);

  // RF09: Vinculação de hidrômetro
  bool vincularHidrometro(int userId, int hidrometroId);

  // ===============================================
  // SUBSISTEMA DE MONITORAMENTO E DADOS (RF17)
  // ===============================================

  void iniciarMonitoramentoBackground();
  void pararMonitoramento();
  void setVerbose(bool ativo);
  long long obterConsumo(int idSHA);
  int obterUltimaLeitura(int idSHA);
  std::vector<int> listarHidrometros();
  bool estaAtivo();

  // NOVO: Funções de Log e Status (RF16, RF17)
  // Agora usa LogLevel (do Logger.hpp)
  void logSystemError(LogLevel level, const std::string &origem,
                      const std::string &mensagem);      // RF16
  std::vector<DB::SystemLog> consultarLogsDeAuditoria(); // RF17
  void consultarStatus();                                // RF17
};

#endif