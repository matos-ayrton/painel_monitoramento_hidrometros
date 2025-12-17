#ifndef TESSERACTOCRAAAPTER_HPP
#define TESSERACTOCRAAAPTER_HPP

#include "IOcrAdapter.hpp"
#include <tesseract/baseapi.h>
#include <memory>

/**
 * Implementação do adaptador para Tesseract OCR
 * Adapta a API do Tesseract para a interface IOcrAdapter
 */
class TesseractOcrAdapter : public IOcrAdapter {
public:
    TesseractOcrAdapter();
    ~TesseractOcrAdapter() override;

    void inicializar() override;
    void finalizar() override;
    bool isInitialized() override;
    std::string extrairTexto(const cv::Mat& imagem,
                            int roiX, int roiY,
                            int roiWidth, int roiHeight) override;
    void definirWhitelist(const std::string& whitelist) override;

private:
    std::unique_ptr<tesseract::TessBaseAPI> tess;
    bool inicializado;

    /**
     * Filtra apenas dígitos da string de resultado
     * @param texto String com o resultado do OCR
     * @return String contendo apenas dígitos
     */
    std::string filtrarDigitos(const std::string& texto) const;
};

#endif
