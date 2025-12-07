#include "MonitoramentoBanco.hpp"

void MonitoramentoBanco::registrarLeitura(int idSHA, int valor) {
    dados[idSHA].leituras.push_back(valor);
    dados[idSHA].somaTotal += valor;
}

long long MonitoramentoBanco::getTotal(int idSHA) const {
    if (!dados.count(idSHA)) return 0;
    return dados.at(idSHA).somaTotal;
}

const std::vector<int>& MonitoramentoBanco::getLeituras(int idSHA) const {
    return dados.at(idSHA).leituras;
}