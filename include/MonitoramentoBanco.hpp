#ifndef MONITORAMENTOBANCO_HPP
#define MONITORAMENTOBANCO_HPP

#include <unordered_map>
#include <vector>
#include <algorithm>

class MonitoramentoBanco {
public:
    struct RegistroSHA {
        std::vector<int> leituras; // Histórico de leituras absolutas
        long long somaTotal = 0; // Consumo acumulado
    };

    void registrarLeitura(int idSHA, int valor); 
    void adicionarVolumeGasto(int idSHA, int deltaVolume); 
    int getUltimaLeituraAbsoluta(int idSHA) const; 
    long long getTotal(int idSHA) const;
    const std::vector<int>& getLeituras(int idSHA) const;
    std::vector<int> listarHidrometros() const;

private:
    std::unordered_map<int, RegistroSHA> dados;
};

#endif