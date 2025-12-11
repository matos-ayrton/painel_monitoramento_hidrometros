#ifndef ILEITURA_STRATEGY_HPP
#define ILEITURA_STRATEGY_HPP

#include <string>

// Padrão Strategy: Interface para diferentes métodos de leitura OCR
class ILeituraStrategy {
public:
    virtual ~ILeituraStrategy() = default;
    virtual int extrairValor(const std::string& caminhoImg) const = 0;
};

#endif