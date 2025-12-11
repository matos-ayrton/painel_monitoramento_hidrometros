#ifndef OCR_STRATEGY_FACTORY_HPP
#define OCR_STRATEGY_FACTORY_HPP

#include "ILeituraStrategy.hpp"
#include "IOcrAdapter.hpp"
#include <memory>
#include <string>
#include <stdexcept>

enum class StrategyType {
    ESTATICA 
};

// Padrão Factory Method
class OcrStrategyFactory {
public:
    static std::unique_ptr<ILeituraStrategy> createStrategy(
        StrategyType type, 
        int roiX, 
        int roiY, 
        int roiWidth, 
        int roiHeight,
        std::shared_ptr<IOcrAdapter> adapter
    );
};

#endif