#ifndef MONITORAMENTOBANCO_HPP
#define MONITORAMENTOBANCO_HPP

#include <unordered_map>
#include <vector>

class MonitoramentoBanco {
public:
    struct RegistroSHA {
        // Histórico de leituras absolutas (ex: 4457, 4458)
        std::vector<int> leituras;
        // Total de consumo acumulado (Soma dos Deltas)
        long long somaTotal = 0;
    };

    // Armazena a leitura absoluta (valor).
    void registrarLeitura(int idSHA, int valor); 

    // NOVO: Adiciona o Delta (Volume Gasto) à soma total (Consumo Acumulado).
    void adicionarVolumeGasto(int idSHA, int deltaVolume); 

    // NOVO: Retorna a última leitura absoluta registrada (para cálculo do Delta).
    int getUltimaLeituraAbsoluta(int idSHA) const; 

    // Retorna o volume total ACUMULADO (consumo).
    long long getTotal(int idSHA) const;

    // Retorna o vetor de leituras absolutas.
    const std::vector<int>& getLeituras(int idSHA) const;

private:
    std::unordered_map<int, RegistroSHA> dados;
};

#endif