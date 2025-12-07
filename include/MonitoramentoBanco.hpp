#ifndef MONITORAMENTOBANCO_HPP
#define MONITORAMENTOBANCO_HPP

#include <unordered_map>
#include <vector>

class MonitoramentoBanco {
public:
    struct RegistroSHA {
        std::vector<int> leituras;
        long long somaTotal = 0;
    };

private:
    std::unordered_map<int, RegistroSHA> dados;

public:
    void registrarLeitura(int idSHA, int valor);

    long long getTotal(int idSHA) const;

    const std::vector<int>& getLeituras(int idSHA) const;
};

#endif