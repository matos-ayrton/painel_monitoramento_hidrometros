#include "MonitoramentoBanco.hpp"
#include <iostream>

// Adiciona o Delta (Volume Gasto) à soma total.
void MonitoramentoBanco::adicionarVolumeGasto(int idSHA, int deltaVolume) {
    dados[idSHA].somaTotal += deltaVolume;
}

// Implementação para buscar a última leitura absoluta registrada.
int MonitoramentoBanco::getUltimaLeituraAbsoluta(int idSHA) const {
    if (!dados.count(idSHA) || dados.at(idSHA).leituras.empty()) {
        return 0; // 0 se não houver leituras anteriores
    }
    // Retorna o último valor absoluto registrado.
    return dados.at(idSHA).leituras.back();
}


void MonitoramentoBanco::registrarLeitura(int idSHA, int valor) {
    // CORRIGIDO: Insere o valor absoluto na lista, mas NÃO soma ao somaTotal.
    dados[idSHA].leituras.push_back(valor);
}

long long MonitoramentoBanco::getTotal(int idSHA) const {
    if (!dados.count(idSHA)) return 0;
    return dados.at(idSHA).somaTotal;
}

const std::vector<int>& MonitoramentoBanco::getLeituras(int idSHA) const {
    // Note: Esta função deve ser chamada apenas se dados[idSHA] existir,
    // pois usa .at() que lança exceção se a chave não existir.
    return dados.at(idSHA).leituras;
}