#include "LeituraEstaticaStrategy.hpp"
#include <iostream>
#include <stdexcept>
#include <opencv2/opencv.hpp>

int LeituraEstaticaStrategy::extrairValor(const std::string& caminhoImg) const {
    if (!ocrAdapter) {
        throw std::runtime_error("Adaptador OCR não foi fornecido");
    }

    // Carrega a imagem
    cv::Mat imagem = cv::imread(caminhoImg, cv::IMREAD_GRAYSCALE);
    if (imagem.empty()) {
        throw std::runtime_error("Não foi possível carregar a imagem: " + caminhoImg);
    }

    // Utiliza o adaptador para extrair o texto
    std::string digitos = ocrAdapter->extrairTexto(imagem, roiX, roiY, roiWidth, roiHeight);

    try {
        return std::stoi(digitos);
    } catch (const std::invalid_argument&) {
        throw std::runtime_error("Valor OCR inválido para conversão: " + digitos);
    }
}