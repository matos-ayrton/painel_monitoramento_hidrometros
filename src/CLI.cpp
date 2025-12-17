#include "CLI.hpp"
#include "DB.hpp" // Para DB::SystemLog
#include "User.hpp"
#include <algorithm>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

// ======================= VARIÁVEIS GLOBAIS E CONSTRUTOR
// =======================

CLI::CLI(Fachada &fachada)
    : fachada(fachada), usuarioLogado(""), roleLogada("") {}

// ======================= FUNÇÕES AUXILIARES =======================

// Função auxiliar para obter entrada de linha
std::string getLine(const std::string &prompt) {
  std::cout << prompt;
  std::string line;
  std::getline(std::cin, line);
  return line;
}

// Função auxiliar para obter entrada numérica (int)
int getInt(const std::string &prompt) {
  std::cout << prompt;
  int value;
  while (!(std::cin >> value)) {
    std::cout << "Entrada invalida. Por favor, digite um numero inteiro: ";
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  }
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(),
                  '\n'); // Limpar o buffer
  return value;
}

// ======================= FLUXO PRINCIPAL =======================

void CLI::executar() {
  std::cout << "╔════════════════════════════════════════════════════╗"
            << std::endl;
  std::cout << "║   SISTEMA DE MONITORAMENTO DE HIDRÔMETROS (CLIENTE) ║"
            << std::endl;
  std::cout << "╚════════════════════════════════════════════════════╝"
            << std::endl
            << std::endl;

  fachada.iniciarMonitoramentoBackground();
  std::cout << std::endl;

  // 1. Tela de Login - Autenticação obrigatória
  exibirTelaLogin();

  if (roleLogada.empty() || roleLogada == "SAIR") {
    std::cout << "\nEncerrando sistema..." << std::endl;
    fachada.pararMonitoramento();
    return;
  }

  // 2. Loop do Menu Pós-Login
  lidarComMenu();

  std::cout << "\nEncerrando sistema..." << std::endl;
  fachada.pararMonitoramento();
}

// ======================= AUTENTICAÇÃO E LOGIN =======================

void CLI::exibirTelaLogin() {
  std::string matricula, senha;

  std::cout << "\n======================================" << std::endl;
  std::cout << "          TELA DE AUTENTICAÇÃO        " << std::endl;
  std::cout << "======================================" << std::endl;

  for (int tentativas = 3; tentativas > 0; --tentativas) {
    std::cout << "Matricula/Usuario: ";
    std::getline(std::cin, matricula);
    std::cout << "Senha: ";
    std::getline(std::cin, senha);

    auto loginResult = fachada.autenticar(matricula, senha);

    if (loginResult.has_value()) {
      usuarioLogado = matricula;
      roleLogada = loginResult.value();
      std::cout << "\n✓ Autenticação bem-sucedida! Perfil: " << roleLogada
                << std::endl;
      return;
    }

    std::cout << "\nLogin falhou. Credenciais inválidas ou usuário inativo. "
              << (tentativas - 1) << " tentativas restantes." << std::endl;
    if (tentativas == 1) {
      std::cout << "Máximo de tentativas excedido." << std::endl;
      roleLogada = "SAIR";
      return;
    }
  }
}

// ======================= GERENCIAMENTO DE MENUS =======================

void CLI::lidarComMenu() {
  std::string opcao;
  while (true) {
    std::cout << "\n======================================" << std::endl;
    std::cout << " USUÁRIO: " << usuarioLogado << " | ROLE: " << roleLogada
              << std::endl;

    if (roleLogada == "gerente") {
      exibirMenuGerente();
    } else if (roleLogada == "administrador") {
      exibirMenuAdministrador();
    } else {
      std::cout << "Erro de permissão. Saindo." << std::endl;
      break;
    }

    std::cout << "Escolha uma opção: ";
    if (!std::getline(std::cin, opcao)) {
      break;
    }

    // Opção 5 é o menu de Usuários/Clientes para ambos os perfis
    if (opcao == "5") {
      exibirMenuUsuario();
      continue;
    }

    // Condições de Saída
    if ((opcao == "6" && roleLogada == "administrador") ||
        (opcao == "10" && roleLogada == "gerente")) {
      std::cout << "\nEncerrando sessão para " << usuarioLogado << "..."
                << std::endl;
      break;
    }

    // Processa opções exclusivas do Gerente (RF02/RF04 e Logs)
    if (roleLogada == "gerente" && processarOpcaoGerente(opcao)) {
      continue;
    }

    // Processa opções Comuns (Monitoramento)
    if (processarOpcaoMenuPrincipal(opcao)) {
      continue;
    }

    std::cout << "\nOpção inválida. Tente novamente.\n" << std::endl;
  }
}

bool CLI::processarOpcaoMenuPrincipal(const std::string &opcao) {
  if (opcao == "1") {
    consultarConsumo();
  } else if (opcao == "2") {
    consultarUltimaLeitura();
  } else if (opcao == "3") {
    listarHidrometros();
  } else if (opcao == "4") {
    exibirStatus(); // RF17
  } else {
    return false;
  }
  return true;
}

bool CLI::processarOpcaoGerente(const std::string &opcao) {
  if (opcao == "6") {
    registrarAdministrador(); // RF02, RF15
    return true;
  } else if (opcao == "7") {
    desligarAdministrador(); // RF04, RF15
    return true;
  } else if (opcao == "8") {
    consultarLogsDeAuditoriaCLI(); // RF17
    return true;
  } else if (opcao == "9") {
    std::cout << "\nFuncionalidade de Gerenciamento de Senha/Role ainda não "
                 "implementada.\n"
              << std::endl;
    return true;
  }
  return false;
}

void CLI::exibirMenuAdministrador() {
  std::cout << "┌────────────────────────────────────────────────────┐"
            << std::endl;
  std::cout << "│ 1. Consultar consumo total por ID                 │"
            << std::endl;
  std::cout << "│ 2. Consultar última leitura absoluta              │"
            << std::endl;
  std::cout << "│ 3. Listar hidrometros disponíveis                 │"
            << std::endl;
  std::cout << "│ 4. Exibir status do monitoramento (RF17)          │"
            << std::endl;
  std::cout << "├────────────────────────────────────────────────────┤"
            << std::endl;
  std::cout << "│ 5. Gerenciar Clientes (RF05 a RF09)               │"
            << std::endl;
  std::cout << "├────────────────────────────────────────────────────┤"
            << std::endl;
  std::cout << "│ 6. Sair                                           │"
            << std::endl;
  std::cout << "└────────────────────────────────────────────────────┘"
            << std::endl;
}

void CLI::exibirMenuGerente() {
  std::cout << "┌────────────────────────────────────────────────────┐"
            << std::endl;
  std::cout << "│ 1. Consultar consumo total por ID                 │"
            << std::endl;
  std::cout << "│ 2. Consultar última leitura absoluta              │"
            << std::endl;
  std::cout << "│ 3. Listar hidrometros disponíveis                 │"
            << std::endl;
  std::cout << "│ 4. Exibir status do monitoramento (RF17)          │"
            << std::endl;
  std::cout << "├────────────────────────────────────────────────────┤"
            << std::endl;
  std::cout << "│ 5. Gerenciar Clientes (RF05 a RF09)               │"
            << std::endl;
  std::cout << "├────────────────────────────────────────────────────┤"
            << std::endl;
  std::cout << "│        OPÇÕES DE GERENCIAMENTO E AUDITORIA         │"
            << std::endl;
  std::cout << "│ 6. Registrar novo Administrador (RF02)            │"
            << std::endl;
  std::cout << "│ 7. Desligar acesso de Administrador (RF04)        │"
            << std::endl;
  std::cout << "│ 8. Consultar Logs de Auditoria/Erro (RF17)        │"
            << std::endl;
  std::cout << "│ 9. Gerenciar Senhas/Roles (N/I)                   │"
            << std::endl;
  std::cout << "├────────────────────────────────────────────────────┤"
            << std::endl;
  std::cout << "│ 10. Sair                                          │"
            << std::endl;
  std::cout << "└────────────────────────────────────────────────────┘"
            << std::endl;
}

// ======================= OPÇÕES EXCLUSIVAS DO GERENTE (RF02 e RF04)
// =======================

void CLI::registrarAdministrador() {
  std::string matricula;
  std::cout << "\n--- REGISTRAR ADMINISTRADOR (RF02/RF15) ---" << std::endl;
  std::cout << "Digite a matricula do novo administrador: ";
  std::getline(std::cin, matricula);

  if (fachada.registrarAdministrador(matricula)) {
    std::cout << "\n✓ Administrador " << matricula
              << " registrado com sucesso! (Senha padrão: a própria matricula)"
              << std::endl;
  } else {
    std::cout << "\n✗ Falha ao registrar administrador. Verifique se a "
                 "matrícula é válida e se o BD está acessível."
              << std::endl;
  }
}

void CLI::desligarAdministrador() {
  std::string matricula;
  std::cout << "\n--- DESLIGAR ADMINISTRADOR (RF04/RF15) ---" << std::endl;
  std::cout << "Digite a matricula do administrador a ser desligado: ";
  std::getline(std::cin, matricula);

  if (fachada.desligarAdministrador(matricula)) {
    std::cout << "\n✓ Administrador " << matricula
              << " teve seu acesso desativado com sucesso." << std::endl;
  } else {
    std::cout
        << "\n✗ Falha ao desligar administrador. Matrícula não encontrada, "
           "erro no BD ou tentativa de desativar o Gerente GERAL."
        << std::endl;
  }
}

// ======================= MENU DE USUÁRIOS/CLIENTES (RF05 a RF09)
// =======================

void CLI::exibirMenuUsuario() {
  std::string opcao;
  while (true) {
    std::cout
        << "\n--- GERENCIAMENTO DE CLIENTES/USUÁRIOS (RF05-RF09, RF12) ---\n";
    std::cout << "1. Cadastrar Novo Usuario (RF05)\n";
    std::cout << "2. Consultar Usuario (RF06/RF12)\n";
    std::cout << "3. Atualizar Dados do Usuario (RF07)\n";
    std::cout << "4. Remover Usuario (RF08)\n";
    std::cout << "5. Vincular Hidrometro ao Usuario (RF09)\n";
    std::cout << "9. Voltar ao Menu Principal\n";

    std::cout << "Escolha uma opção: ";
    if (!std::getline(std::cin, opcao))
      return;

    if (opcao == "9")
      break;

    try {
      int choice = std::stoi(opcao);
      switch (choice) {
      case 1:
        cadastrarUsuario();
        break;
      case 2:
        consultarUsuario();
        break;
      case 3:
        atualizarUsuario();
        break;
      case 4:
        removerUsuario();
        break;
      case 5:
        vincularHidrometro();
        break;
      default:
        std::cout << "\nOpção inválida.\n";
      }
    } catch (...) {
      std::cout << "\nEntrada inválida.\n";
    }
  }
}

// RF05: Cadastro de Usuário (RF15 - Logado na Fachada)
void CLI::cadastrarUsuario() {
  std::cout << "\n--- CADASTRO DE USUARIO (RF05) ---\n";
  std::string nome = getLine("Nome: ");
  std::string cpf = getLine("CPF: ");
  std::string endereco = getLine("Endereco: ");
  std::string telefone = getLine("Telefone: ");
  std::string email = getLine("Email: ");

  if (fachada.cadastrarUsuario(nome, cpf, endereco, telefone, email)) {
    std::cout << "\n✓ Usuario/Cliente cadastrado com sucesso!\n";
  } else {
    std::cout << "\n✗ Falha no cadastro do usuario. Verifique unicidade de "
                 "CPF/Email.\n";
  }
}

// RF06: Consulta de Usuário (FINALIZADO: Exibe RF06 e RF12)
void CLI::consultarUsuario() {
  std::cout << "\n--- CONSULTA DE USUARIO (RF06/RF12) ---\n";
  int id = getInt("ID do Usuario/Cliente: ");

  auto userOpt = fachada.consultarUsuario(id);
  if (userOpt.has_value()) {
    const User &user = userOpt.value();
    std::cout << "\n╔════════════════════════════════════════╗\n";
    std::cout << "║ DADOS DO CLIENTE ID: " << std::setw(17) << std::left
              << user.getId() << " ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║ Nome:     " << std::setw(29) << user.getNome().substr(0, 29)
              << " ║\n";
    std::cout << "║ CPF:      " << std::setw(29) << user.getCpf() << " ║\n";
    std::cout << "║ Email:    " << std::setw(29)
              << user.getEmail().substr(0, 29) << " ║\n";
    std::cout << "║ Endereco: " << std::setw(29)
              << user.getEndereco().substr(0, 29) << " ║\n";
    std::cout << "║ Telefone: " << std::setw(29) << user.getTelefone()
              << " ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";

    // RF12: Gasto Total de Água (Consulta Real)
    std::cout << "║ Gasto Total (m³): " << std::setw(23) << std::fixed
              << std::setprecision(2) << user.getGastoAguaTotal() << " ║\n";
    std::cout << "╠════════════════════════════════════════╣\n";

    // RF09: Hidrômetros Vinculados
    std::cout << "║ Hidrometros Vinculados ("
              << user.getHidrometrosVinculados().size() << "):" << std::setw(15)
              << " " << " ║\n";
    std::cout << "║ ";
    int count = 0;
    for (int hId : user.getHidrometrosVinculados()) {
      std::cout << hId
                << (count++ < user.getHidrometrosVinculados().size() - 1 ? ", "
                                                                         : " ");
    }
    // Calcula o preenchimento necessário para fechar a borda
    if (count > 0) {
      std::cout << std::setw(38 -
                             (int)user.getHidrometrosVinculados().size() * 3)
                << std::right << "║\n";
    } else {
      std::cout << std::setw(39) << std::right << "║\n";
    }

    // RF06: Histórico de Contas (Consulta Real)
    std::cout << "╠════════════════════════════════════════╣\n";
    std::cout << "║ HISTÓRICO DE CONTAS (" << user.getHistoricoContas().size()
              << "):" << std::setw(15) << " " << " ║\n";
    if (user.getHistoricoContas().empty()) {
      std::cout << "║ Nenhuma conta registrada." << std::setw(14) << " "
                << " ║\n";
    } else {
      for (const auto &bill : user.getHistoricoContas()) {
        std::cout << "║ Vencimento: " << std::setw(10) << std::left
                  << bill.dataVencimento.substr(0, 10) << " | Valor: R$ "
                  << std::setw(8) << std::right << std::fixed
                  << std::setprecision(2) << bill.valorGasto << " ║\n";
      }
    }
    std::cout << "╚════════════════════════════════════════╝\n";
  } else {
    std::cout << "Usuario ID " << id
              << " nao encontrado ou falha na consulta.\n";
  }
}

// RF07: Atualização de Usuário (RF15 - Logado na Fachada)
void CLI::atualizarUsuario() {
  std::cout << "\n--- ATUALIZACAO DE USUARIO (RF07) ---\n";
  int id = getInt("ID do Usuario a ser atualizado: ");

  std::cout << "Deixe o campo vazio para nao alterar.\n";
  std::string novoEmail = getLine("Novo Email: ");
  std::string novoEndereco = getLine("Novo Endereco: ");
  std::string novoTelefone = getLine("Novo Telefone: ");

  if (fachada.atualizarUsuario(id, novoEmail, novoEndereco, novoTelefone)) {
    std::cout << "\n✓ Dados do Usuario ID " << id
              << " atualizados com sucesso.\n";
  } else {
    std::cout << "\n✗ Falha na atualizacao. Verifique se o ID existe ou se "
                 "forneceu dados validos.\n";
  }
}

// RF08: Remoção de Usuário (RF15 - Logado na Fachada)
void CLI::removerUsuario() {
  std::cout << "\n--- REMOCAO DE USUARIO (RF08) ---\n";
  int id = getInt("ID do Usuario a ser removido: ");
  std::string confirm =
      getLine("Tem certeza que deseja remover o Usuario ID " +
              std::to_string(id) + " e todos os seus vinculos? (S/N): ");

  if (confirm == "S" || confirm == "s") {
    if (fachada.removerUsuario(id)) {
      std::cout << "\n✓ Usuario ID " << id << " removido com sucesso.\n";
    } else {
      std::cout << "\n✗ Falha na remocao. Usuario ID " << id
                << " nao encontrado.\n";
    }
  } else {
    std::cout << "\nOperacao cancelada.\n";
  }
}

// RF09: Vinculação de Hidrômetro (RF15 - Logado na Fachada)
void CLI::vincularHidrometro() {
  std::cout << "\n--- VINCULACAO DE HIDROMETRO (RF09) ---\n";
  int clientId = getInt("ID do Usuario/Cliente: ");
  int hidrometroId = getInt("ID do Hidrometro: ");

  if (fachada.vincularHidrometro(clientId, hidrometroId)) {
    std::cout << "\n✓ Hidrometro " << hidrometroId << " vinculado ao Usuario "
              << clientId << " com sucesso.\n";
  } else {
    std::cout << "\n✗ Falha na vinculacao. Verifique se os IDs existem ou se o "
                 "vinculo ja existe.\n";
  }
}

// ======================= OPÇÕES DE CONSULTA (Monitoramento)
// =======================

void CLI::consultarConsumo() {
  int id;
  std::cout << "\nDigite o ID do hidrometro: ";
  std::string idStr;
  if (!std::getline(std::cin, idStr))
    return;
  std::stringstream ss(idStr);
  if (!(ss >> id)) {
    std::cout << "Entrada inválida." << std::endl;
    return;
  }

  long long consumo = fachada.obterConsumo(id);
  std::cout << "\n✓ Consumo total do hidrometro " << id << ": " << std::setw(10)
            << consumo << " metros cúbicos\n"
            << std::endl;
}

void CLI::consultarUltimaLeitura() {
  int id;
  std::cout << "\nDigite o ID do hidrometro: ";
  std::string idStr;
  if (!std::getline(std::cin, idStr))
    return;
  std::stringstream ss(idStr);
  if (!(ss >> id)) {
    std::cout << "Entrada inválida." << std::endl;
    return;
  }

  int leitura = fachada.obterUltimaLeitura(id);
  std::cout << "\n✓ Última leitura absoluta do hidrometro " << id << ": "
            << std::setw(10) << leitura << " unidades\n"
            << std::endl;
}

void CLI::listarHidrometros() {
  auto hidrometros = fachada.listarHidrometros();
  std::cout << "\n╔════════════════════════════════════════════════════════════"
               "══════╗"
            << std::endl;
  std::cout
      << "║  Hidrometros Disponíveis e Status Atual                      ║"
      << std::endl;
  std::cout
      << "╠══════════════════════════════════════════════════════════════════╣"
      << std::endl;

  for (int id : hidrometros) {
    long long consumo = fachada.obterConsumo(id);
    int leitura = fachada.obterUltimaLeitura(id);
    std::cout << "║ ID: " << std::setw(2) << id
              << " | Consumo Acumulado: " << std::setw(15) << consumo
              << " | Última Leitura: " << std::setw(15) << leitura << " ║"
              << std::endl;
  }
  std::cout << "╚══════════════════════════════════════════════════════════════"
               "════╝\n"
            << std::endl;
}

// RF17: Exibir Status Operacional (Delegado à Fachada)
void CLI::exibirStatus() {
  // A Fachada fará a impressão formatada e a verificação do OCR/DB
  fachada.consultarStatus();
  // Além do status, podemos mostrar o estado da thread do monitor
  std::cout << "\n┌──────────────────────────────────────┐" << std::endl;
  if (fachada.estaAtivo()) {
    std::cout << "│ Status: ✓ MONITORAMENTO ATIVO        │" << std::endl;
  } else {
    std::cout << "│ Status: ✗ MONITORAMENTO INATIVO      │" << std::endl;
  }
  std::cout << "└──────────────────────────────────────┘\n" << std::endl;
}

// RF17: Consultar Logs de Auditoria (Delegado à Fachada)
void CLI::consultarLogsDeAuditoriaCLI() {
  std::cout << "\n--- CONSULTA DE LOGS DE AUDITORIA E ERRO (RF17) ---\n";
  auto logs = fachada.consultarLogsDeAuditoria();

  if (logs.empty()) {
    std::cout << "Nenhum log encontrado ou falha na consulta ao DB.\n";
    return;
  }

  std::cout << "Exibindo " << logs.size() << " logs mais recentes:\n";
  std::cout << std::left << std::setw(20) << "TIMESTAMP" << std::setw(15)
            << "TIPO" << std::setw(10) << "NÍVEL" << std::setw(15)
            << "RESPONSÁVEL" << std::setw(15) << "ORIGEM"
            << "MENSAGEM" << std::endl;
  std::cout << std::string(90, '=') << std::endl;

  for (const auto &log : logs) {
    std::cout << std::left << std::setw(20) << log.timestamp.substr(0, 19)
              << std::setw(15) << log.tipo << std::setw(10) << log.nivel
              << std::setw(15) << log.responsavel.substr(0, 14) << std::setw(15)
              << log.origem.substr(0, 14) << log.mensagem.substr(0, 40)
              << std::endl;
  }
  std::cout << std::string(90, '=') << std::endl;
}