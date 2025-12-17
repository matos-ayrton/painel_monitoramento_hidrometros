#include "Fachada.hpp"
#include "DB.hpp"
#include "Logger.hpp" // Inclusão correta
#include "MonitoramentoConcretoMediator.hpp"
#include "OcrStrategyFactory.hpp"
#include "PerfilManager.hpp"
#include "TesseractOcrAdapter.hpp"
#include "UsuarioService.hpp"
#include <iomanip>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <vector>

// Inicialização da instância estática
Fachada *Fachada::instance = nullptr;

Fachada &Fachada::getInstance() {
  if (instance == nullptr) {
    instance = new Fachada();
  }
  return *instance;
}

// Configuração das Strategies (Factory Method)
std::vector<MonitoramentoService::ShaFolder> Fachada::configurarPastas() {
  // [Configuração de Pastas mantida inalterada]
  std::vector<MonitoramentoService::ShaFolder> config;

  // SHA 1: pasta do simulador principal e ROI ajustado
  config.push_back(
      {1,
       "/home/ayrton/Documentos/simulador_hidrometro_real/"
       "Medicoes_202311250031",
       OcrStrategyFactory::createStrategy(StrategyType::ESTATICA, 260, 313, 545,
                                          400, ocrAdapter)});

  // SHA 2: pasta do simulador analógico e ROI ajustado
  config.push_back(
      {2,
       "/home/ayrton/Documentos/Simulador-Hidrometro-Analogico/"
       "Medições_202311250039",
       OcrStrategyFactory::createStrategy(StrategyType::ESTATICA, 204, 131, 360,
                                          224, ocrAdapter)});

  return config;
}

// Construtor Privado (Singleton)
Fachada::Fachada()
    // 1. Inicializa o Banco e dependências
    : banco(std::make_unique<MonitoramentoBanco>()),
      ocrAdapter(std::make_shared<TesseractOcrAdapter>()),
      db(std::make_unique<DB>("data/painel.db")), // DB é inicializado
      logger(
          Logger::getInstance(*db.get())), // Inicializa a referência do Logger
      perfilManager(
          std::make_unique<PerfilManager>(*db)), // PerfilManager usa DB
      userManager(
          std::make_unique<UsuarioService>(*db)) // UsuarioService usa DB
{
  // Opcional: Criar tabelas no DB se elas não existirem
  db->ensureSchema();
  db->ensureLogSchema(); // Garante a tabela de logs

  // Inicializa o adaptador OCR
  try {
    ocrAdapter->inicializar();
    ocrAdapter->definirWhitelist("0123456789");
    logger.logSystemError(
        LogLevel::INFO, "Fachada",
        "OCR Adapter inicializado com sucesso."); // USANDO LogLevel
  } catch (const std::exception &e) {
    std::cerr << "[ERRO] Falha ao inicializar OCR Adapter: " << e.what()
              << std::endl;
    logger.logSystemError(LogLevel::CRITICAL, "Fachada",
                          "Falha ao inicializar OCR Adapter: " +
                              std::string(e.what())); // USANDO LogLevel
    throw;
  }

  auto pastas = configurarPastas();

  // 2. Criar o Mediator, passando a referência do Banco
  mediator = std::make_unique<MonitoramentoConcretoMediator>(*banco);

  // 3. Criar o MonitoramentoService
  monitor = std::make_unique<MonitoramentoService>(std::move(pastas), *mediator,
                                                   *banco);

  try {
    setVerbose(true);
  } catch (...) {
  }

  ativo = false;
  logger.logSystemError(
      LogLevel::INFO, "Fachada",
      "Sistema completamente inicializado."); // USANDO LogLevel
}

// ===============================================
// SUBSISTEMA DE PERFIS (RF01, RF02, RF03, RF04, RF14, RF15)
// ===============================================

// Método de autenticação unificado (RF01 e RF03)
std::optional<std::string> Fachada::autenticar(const std::string &matricula,
                                               const std::string &senha) {
  if (!perfilManager)
    return std::nullopt;

  auto loginResult = perfilManager->autenticar(matricula, senha);

  // RF14: Loga o evento de autenticação
  // Simulando IP local
  logger.logAuthentication(matricula, loginResult.has_value(), "127.0.0.1");

  return loginResult;
}

// Funções de autenticação antigas (Não utilizadas externamente)
bool Fachada::autenticarGerente(const std::string &usuario,
                                const std::string &senha) {
  return false;
}

bool Fachada::autenticarAdministrador(const std::string &matricula,
                                      const std::string &senha) {
  return false;
}

bool Fachada::registrarAdministrador(
    const std::string &matricula) { // RF02, RF15
  if (!perfilManager)
    return false;

  bool sucesso = perfilManager->registrarAdministrador(matricula);
  if (sucesso) {
    // RF15: Loga a operação CRUD de administrador
    logger.logUserOperation("GERENTE", "CADASTRO",
                            "Admin Matrícula: " + matricula);
  } else {
    logger.logSystemError(LogLevel::WARNING, "PerfilManager",
                          "Falha ao registrar administrador: " +
                              matricula); // USANDO LogLevel
  }
  return sucesso;
}

bool Fachada::desligarAdministrador(
    const std::string &matricula) { // RF04, RF15
  if (!perfilManager)
    return false;

  bool sucesso = perfilManager->desligarAdministrador(matricula);
  if (sucesso) {
    // RF15: Loga a operação CRUD de administrador
    logger.logUserOperation("GERENTE", "REMOCAO",
                            "Admin Matrícula: " + matricula);
  } else {
    logger.logSystemError(LogLevel::WARNING, "PerfilManager",
                          "Falha ao desligar administrador: " +
                              matricula); // USANDO LogLevel
  }
  return sucesso;
}

// ===============================================
// SUBSISTEMA DE USUÁRIOS (Clientes - RF05 a RF09 e RF15)
// ===============================================

// RF05: Cadastro (RF15)
bool Fachada::cadastrarUsuario(const std::string &nome, const std::string &cpf,
                               const std::string &endereco,
                               const std::string &telefone,
                               const std::string &email) {
  if (!userManager)
    return false;
  bool sucesso =
      userManager->registerUser(nome, cpf, endereco, telefone, email);
  if (sucesso) {
    logger.logUserOperation("ADMIN/GERENTE", "CADASTRO", "Cliente: " + cpf);
  }
  return sucesso;
}

// RF06/RF12: Consulta
std::optional<User> Fachada::consultarUsuario(int id) {
  if (!userManager)
    return std::nullopt;
  return userManager->getUserData(id);
}

// RF07: Atualização (RF15)
bool Fachada::atualizarUsuario(int id, const std::string &novoEmail,
                               const std::string &novoEndereco,
                               const std::string &novoTelefone) {
  if (!userManager)
    return false;
  bool sucesso =
      userManager->updateUserInfo(id, novoEmail, novoEndereco, novoTelefone);
  if (sucesso) {
    logger.logUserOperation("ADMIN/GERENTE", "ATUALIZACAO",
                            "Cliente ID: " + std::to_string(id));
  }
  return sucesso;
}

// RF08: Remoção (RF15)
bool Fachada::removerUsuario(int id) {
  if (!userManager)
    return false;
  bool sucesso = userManager->removeUser(id);
  if (sucesso) {
    logger.logUserOperation("ADMIN/GERENTE", "REMOCAO",
                            "Cliente ID: " + std::to_string(id));
  }
  return sucesso;
}

// RF09: Vinculação de hidrômetro (RF15)
bool Fachada::vincularHidrometro(int userId, int hidrometroId) {
  if (!userManager)
    return false;
  bool sucesso = userManager->linkHydrometerToUser(userId, hidrometroId);
  if (sucesso) {
    logger.logUserOperation("ADMIN/GERENTE", "VINCULACAO",
                            "Cliente " + std::to_string(userId) + " com HM " +
                                std::to_string(hidrometroId));
  }
  return sucesso;
}

// ===============================================
// SUBSISTEMA DE MONITORAMENTO E DADOS (RF16, RF17)
// ===============================================
Fachada::~Fachada() {
  pararMonitoramento();
  if (ocrAdapter) {
    ocrAdapter->finalizar();
  }
}

void Fachada::iniciarMonitoramentoBackground() {
  if (!ativo) {
    std::cout
        << "[FACHADA] Inicializando Monitoramento de SHAs em thread separada..."
        << std::endl;
    threadMonitoramento = std::make_unique<std::thread>(
        &MonitoramentoService::iniciar, monitor.get());
    ativo = true;
    logger.logSystemError(
        LogLevel::INFO, "Fachada",
        "Monitoramento em background iniciado."); // USANDO LogLevel
  }
}

void Fachada::setVerbose(bool ativoVerbose) {
  if (monitor) {
    monitor->setVerbose(ativoVerbose);
    // Não logamos no DB para não sobrecarregar
    std::cout << "[FACHADA] Verbose set to " << (ativoVerbose ? "ON" : "OFF")
              << std::endl;
  }
}

void Fachada::pararMonitoramento() {
  if (ativo) {
    std::cout << "[FACHADA] Sinalizando encerramento do Monitoramento..."
              << std::endl;
    if (monitor) {
      monitor->parar();
    }
    if (threadMonitoramento && threadMonitoramento->joinable()) {
      threadMonitoramento->join();
    }
    ativo = false;
    logger.logSystemError(
        LogLevel::INFO, "Fachada",
        "Monitoramento em background parado."); // USANDO LogLevel
  }
}

long long Fachada::obterConsumo(int idSHA) { return banco->getTotal(idSHA); }

int Fachada::obterUltimaLeitura(int idSHA) {
  return banco->getUltimaLeituraAbsoluta(idSHA);
}

std::vector<int> Fachada::listarHidrometros() {
  return banco->listarHidrometros();
}

bool Fachada::estaAtivo() { return ativo; }

// RF16: Registra um erro de sistema (para ser chamado por outros subsistemas)
void Fachada::logSystemError(LogLevel level, const std::string &origem,
                             const std::string &mensagem) {
  logger.logSystemError(level, origem, mensagem);
}

// RF17: Consulta logs (Delega para o DB)
std::vector<DB::SystemLog> Fachada::consultarLogsDeAuditoria() {
  return db->fetchLogs(50); // Consulta os 50 logs mais recentes
}

// RF17: Exibir Status Operacional dos Subsistemas
void Fachada::consultarStatus() {
  std::cout << "\n╔═══════════════════════════════════════╗" << std::endl;
  std::cout << "║    STATUS OPERACIONAL DO SISTEMA (RF17)   ║" << std::endl;
  std::cout << "╠═══════════════════════════════════════╣" << std::endl;
  std::cout << "║ Monitoramento de Hidrômetros: "
            << (estaAtivo() ? "ATIVO (✓)" : "INATIVO (✗)") << " ║" << std::endl;

  // Supondo que DB::isReady() existe
  // NOTE: A implementação real de DB::isReady() precisa estar no seu DB.cpp
  bool dbOk = db->isReady();
  std::cout << "║ Conexão com Banco de Dados: "
            << (dbOk ? "OK (✓)" : "FALHA (✗)") << " ║" << std::endl;

  // Supondo que IOcrAdapter::isInitialized() exista
  // NOTE: A implementação real de IOcrAdapter::isInitialized() precisa estar no
  // seu TesseractOcrAdapter.cpp
  bool ocrOk = ocrAdapter->isInitialized();
  std::cout << "║ Subsistema de OCR: " << (ocrOk ? "OK (✓)" : "FALHA (✗)")
            << " ║" << std::endl;

  std::cout << "╚═══════════════════════════════════════╝" << std::endl;

  if (!dbOk) {
    logSystemError(LogLevel::CRITICAL, "Fachada",
                   "Conexão com o Banco de Dados falhou."); // USANDO LogLevel
  }
  if (!ocrOk) {
    logSystemError(
        LogLevel::CRITICAL, "Fachada",
        "Subsistema de OCR não está inicializado."); // USANDO LogLevel
  }
}