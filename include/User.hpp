#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <optional>
#include <vector>

// Estrutura para simplificar a passagem de dados de histórico
struct UserBill {
    std::string dataVencimento;
    double valorGasto;
};

class User {
private:
    // RF05: Atributos principais para cadastro
    int id; // Chave primária (ID definido)
    std::string nome;
    std::string cpf;
    std::string endereco;
    std::string telefone;
    std::string email;

    // RF06: Atributos de monitoramento (preenchidos por consulta ao DB/outros subsistemas)
    std::vector<int> hidrometrosVinculados; 
    std::vector<UserBill> historicoContas;
    double gastoAguaTotal; 

public:
    // Construtor privado: Garante que apenas o UserBuilder pode criar um User
    User(int id, std::string nome, std::string cpf, std::string endereco, 
         std::string telefone, std::string email);

    // Getters para RF06 (Consulta)
    int getId() const { return id; }
    std::string getNome() const { return nome; }
    std::string getCpf() const { return cpf; }
    std::string getEndereco() const { return endereco; }
    std::string getTelefone() const { return telefone; }
    std::string getEmail() const { return email; }
    
    const std::vector<int>& getHidrometrosVinculados() const { return hidrometrosVinculados; }
    const std::vector<UserBill>& getHistoricoContas() const { return historicoContas; }
    double getGastoAguaTotal() const { return gastoAguaTotal; }
    
    // Setters (RF07: Atualização)
    void setEndereco(const std::string& novoEndereco) { endereco = novoEndereco; }
    void setTelefone(const std::string& novoTelefone) { telefone = novoTelefone; }
    void setEmail(const std::string& novoEmail) { email = novoEmail; }

    // Métodos para RF06/RF09 (Preenchimento posterior)
    void setHidrometrosVinculados(std::vector<int> h) { hidrometrosVinculados = std::move(h); }
    void setHistoricoContas(std::vector<UserBill> h) { historicoContas = std::move(h); }
    void setGastoAguaTotal(double g) { gastoAguaTotal = g; }

    // Friend class para dar permissão de acesso ao construtor privado
    friend class UserBuilder;
};

#endif // USER_HPP