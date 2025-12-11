#include "OcrStrategyFactory.hpp"
#include "LeituraEstaticaStrategy.hpp" 

std::unique_ptr<ILeituraStrategy> OcrStrategyFactory::createStrategy(
    StrategyType type, 
    int roiX, 
    int roiY, 
    int roiWidth, 
    int roiHeight,
    std::shared_ptr<IOcrAdapter> adapter
) {
    switch (type) {
        case StrategyType::ESTATICA:
            return std::make_unique<LeituraEstaticaStrategy>(roiX, roiY, roiWidth, roiHeight, adapter);
        
        default:
            throw std::invalid_argument("Tipo de Strategy desconhecido ou não implementado.");
    }
}