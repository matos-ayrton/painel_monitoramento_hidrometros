#include "MonitoramentoBanco.hpp"
#include <algorithm> // Para std::max_element
#include <stdexcept>

void MonitoramentoBanco::registrarLeitura(int idSHA, int valor) {
  dados[idSHA].leituras.push_back(valor);
}

void MonitoramentoBanco::adicionarVolumeGasto(int idSHA, int deltaVolume) {
  if (deltaVolume > 0) {
    dados[idSHA].somaTotal += deltaVolume;
  }
}

int MonitoramentoBanco::getUltimaLeituraAbsoluta(int idSHA) const {
  auto it = dados.find(idSHA);
  if (it != dados.end() && !it->second.leituras.empty()) {
    const auto &vec = it->second.leituras;
    // Retorna o MAIOR valor já lido, ignorando flutuações para baixo (erros de
    // OCR ou troca de imagem)
    return *std::max_element(vec.begin(), vec.end());
  }
  return 0;
}

long long MonitoramentoBanco::getTotal(int idSHA) const {
  auto it = dados.find(idSHA);
  if (it != dados.end()) {
    return it->second.somaTotal;
  }
  return 0;
}

const std::vector<int> &MonitoramentoBanco::getLeituras(int idSHA) const {
  static const std::vector<int> empty;
  auto it = dados.find(idSHA);
  if (it != dados.end()) {
    return it->second.leituras;
  }
  return empty;
}

std::vector<int> MonitoramentoBanco::listarHidrometros() const {
  std::vector<int> ids;
  for (const auto &pair : dados) {
    ids.push_back(pair.first);
  }
  std::sort(ids.begin(), ids.end());
  return ids;
}