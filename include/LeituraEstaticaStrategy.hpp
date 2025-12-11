#ifndef LEITURA_ESTATICA_STRATEGY_HPP
#define LEITURA_ESTATICA_STRATEGY_HPP

#include "ILeituraStrategy.hpp"
#include "IOcrAdapter.hpp"
#include <memory>

class LeituraEstaticaStrategy : public ILeituraStrategy {
private:
    int roiX, roiY, roiWidth, roiHeight;
    std::shared_ptr<IOcrAdapter> ocrAdapter;

public:
    LeituraEstaticaStrategy(int x, int y, int w, int h, std::shared_ptr<IOcrAdapter> adapter) 
        : roiX(x), roiY(y), roiWidth(w), roiHeight(h), ocrAdapter(adapter) {}
    
    int extrairValor(const std::string& caminhoImg) const override;
};

#endif