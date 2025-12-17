#ifndef CLI_HPP
#define CLI_HPP

#include "Fachada.hpp"
#include <string>

class CLI {
private:
  Fachada &fachada; // Referência à instância Singleton da Fachada
  std::string usuarioLogado;
  std::string roleLogada;

  // Métodos de fluxo e autenticação
  void exibirTelaLogin();
  void lidarComMenu();
  bool
  processarOpcaoMenuPrincipal(const std::string &opcao); // Processa 1, 2, 3, 4
  bool processarOpcaoGerente(const std::string &opcao);  // Processa 6, 7, 8

  // Métodos de Menu
  void exibirMenuAdministrador(); // Menu principal
  void exibirMenuGerente();       // Menu expandido com opções administrativas
  void exibirMenuUsuario(); // Exibe e lida com o menu de Clientes/Usuários
                            // (RF05-RF09)

  // Métodos de Ação Comuns (Monitoramento)
  void consultarConsumo();
  void consultarUltimaLeitura();
  void listarHidrometros();
  void exibirStatus();

  // Métodos de Ação Exclusivas (RF02 e RF04)
  void registrarAdministrador();
  void desligarAdministrador();

  // Novos Métodos de Cliente (RF05 a RF09 e RF12)
  void cadastrarUsuario();
  void consultarUsuario(); // Agora exibe RF06 (Histórico) e RF12 (Gasto Total)
  void atualizarUsuario();
  void removerUsuario();
  void vincularHidrometro();
  void consultarLogsDeAuditoriaCLI();

public:
  CLI(Fachada &fachada);
  void executar();
};

#endif