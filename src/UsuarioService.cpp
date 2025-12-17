// UsuarioService.cpp (FINAL: Chamadas REAIS ao DB, sem Mocks)

#include "UsuarioService.hpp"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <utility>

UsuarioService::UsuarioService(DB &dbRef) : db(dbRef) {}

// RF06/Auxiliar: Constrói o objeto User a partir dos dados do BD
std::unique_ptr<User> UsuarioService::buildUserFromDB(int id) {
  // 1. Consulta ao DB pelos dados básicos (RF06)
  auto baseDataOpt = db.queryUserBaseData(id);

  if (!baseDataOpt.has_value()) {
    return nullptr;
  }

  // Desempacota a tupla (nome, cpf, endereco, telefone, email)
  auto &[nome, cpf, endereco, telefone, email] = baseDataOpt.value();

  // 2. Cria o Builder e constrói o objeto User base
  UserBuilder builder;
  std::unique_ptr<User> user = builder.setId(id)
                                   .setNome(nome)
                                   .setCpf(cpf)
                                   .setEndereco(endereco)
                                   .setTelefone(telefone)
                                   .setEmail(email)
                                   .build();

  // 3. Consulta e Preenche dados de outros subsistemas/tabelas (RF06, RF09,
  // RF12)

  // CHAMADA REAL 1: Consulta hidrômetros vinculados (RF09)
  std::vector<int> hydrometers = db.queryUserHydrometers(id);
  user->setHidrometrosVinculados(std::move(hydrometers));

  // CHAMADA REAL 2: Consulta Histórico de Contas (RF06)
  std::vector<UserBill> bills = db.queryUserBills(id);
  user->setHistoricoContas(std::move(bills));

  // CHAMADA REAL 3: Consulta Gasto Total de Água (RF12)
  double totalGasto = db.queryUserGastoTotal(id);
  user->setGastoAguaTotal(totalGasto);

  return user;
}

// RF05: Cria e registra um novo usuário
bool UsuarioService::registerUser(const std::string &nome,
                                  const std::string &cpf,
                                  const std::string &endereco,
                                  const std::string &telefone,
                                  const std::string &email) {

  // 1. Obtém o próximo ID real do BD
  int newId = db.getNewUserId();

  if (newId <= 0) {
    std::cerr
        << "[UsuarioService ERROR] Falha ao obter novo ID para cadastro (RF05)."
        << std::endl;
    return false;
  }

  // 2. Usa o Builder para criar a instância (objeto completo)
  try {
    UserBuilder builder;
    std::unique_ptr<User> newUser = builder.setId(newId)
                                        .setNome(nome)
                                        .setCpf(cpf)
                                        .setEndereco(endereco)
                                        .setTelefone(telefone)
                                        .setEmail(email)
                                        .build();

    // 3. Persiste o objeto no BD (CHAMADA REAL ATIVA)
    if (db.insertUser(newUser)) {
      std::cout << "[UsuarioService] Usuário ID " << newUser->getId()
                << " cadastrado com sucesso (RF05)." << std::endl;
      return true;
    }

  } catch (const std::invalid_argument &e) {
    std::cerr << "[UsuarioService ERROR] Dados inválidos para cadastro: "
              << e.what() << " (RF05)" << std::endl;
    return false;
  }

  std::cerr
      << "[UsuarioService ERROR] Falha na persistência do cadastro. (RF05)"
      << std::endl;
  return false;
}

// RF06: Consulta os dados completos do usuário
std::optional<User> UsuarioService::getUserData(int id) {
  std::unique_ptr<User> userPtr = buildUserFromDB(id);
  if (userPtr) {
    return std::optional<User>(std::move(*userPtr));
  }
  std::cerr << "[UsuarioService ERROR] Usuário ID " << id
            << " não encontrado (RF06)." << std::endl;
  return std::nullopt;
}

// RF07: Atualização de informações
bool UsuarioService::updateUserInfo(int id, const std::string &novoEmail,
                                    const std::string &novoEndereco,
                                    const std::string &novoTelefone) {

  if (novoEmail.empty() && novoEndereco.empty() && novoTelefone.empty()) {
    std::cout << "[UsuarioService] Nenhuma informação válida fornecida para "
                 "atualização (RF07)."
              << std::endl;
    return false;
  }

  // Delega a operação complexa de UPDATE ao DB (CHAMADA REAL ATIVA)
  if (db.updateUserInfo(id, novoEmail, novoEndereco, novoTelefone)) {
    std::cout << "[UsuarioService] Usuário ID " << id
              << " atualizado com sucesso (RF07)." << std::endl;
    return true;
  }

  // Verifica se a falha ocorreu por ID não encontrado
  if (!db.clientExists(id)) {
    std::cerr << "[UsuarioService ERROR] Usuário ID " << id
              << " não encontrado para atualização. (RF07)" << std::endl;
    return false;
  }

  std::cerr << "[UsuarioService ERROR] Falha na atualização do usuário ID "
            << id << ". (RF07)" << std::endl;
  return false;
}

// RF08: Remoção de usuário e dados vinculados
bool UsuarioService::removeUser(int id) {
  // 1. Verifica se o usuário existe
  if (!db.clientExists(id)) {
    std::cerr << "[UsuarioService ERROR] Usuário ID " << id
              << " não encontrado para remoção (RF08)." << std::endl;
    return false;
  }

  // 2. Delega a remoção ao DB (CHAMADA REAL ATIVA)
  if (db.deleteUser(id)) {
    std::cout
        << "[UsuarioService] Usuário ID " << id
        << " e seus dados vinculados (cascade) removidos com sucesso (RF08)."
        << std::endl;
    return true;
  }

  std::cerr << "[UsuarioService ERROR] Falha na remoção do usuário ID " << id
            << ". (RF08)" << std::endl;
  return false;
}

// RF09: Vinculação de hidrômetro
bool UsuarioService::linkHydrometerToUser(int userId, int hidrometroId) {
  // 1. Verifica existência do usuário
  if (!db.clientExists(userId)) {
    std::cerr << "[UsuarioService ERROR] Usuário ID " << userId
              << " não encontrado para vinculação (RF09)." << std::endl;
    return false;
  }

  // 2. Persiste a vinculação (CHAMADA REAL ATIVA)
  if (db.linkHydrometer(userId, hidrometroId)) {
    std::cout << "[UsuarioService] Hidrômetro ID " << hidrometroId
              << " vinculado ao Usuário ID " << userId << " com sucesso (RF09)."
              << std::endl;
    return true;
  }

  std::cerr << "[UsuarioService ERROR] Falha na vinculação (pode ser vínculo "
               "duplicado ou erro de DB). (RF09)"
            << std::endl;
  return false;
}

// RF09 (Estendido): Desvinculação
bool UsuarioService::unlinkHydrometerFromUser(int userId, int hidrometroId) {
  if (!db.clientExists(userId)) {
    std::cerr << "[UsuarioService ERROR] Usuário ID " << userId
              << " não encontrado para desvinculação." << std::endl;
    return false;
  }

  if (db.unlinkHydrometer(userId, hidrometroId)) {
    std::cout << "[UsuarioService] Hidrômetro ID " << hidrometroId
              << " desvinculado do Usuário ID " << userId << " com sucesso."
              << std::endl;
    return true;
  }

  std::cerr
      << "[UsuarioService ERROR] Falha na desvinculação (vínculo inexistente?)."
      << std::endl;
  return false;
}