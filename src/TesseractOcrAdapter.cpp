#include "TesseractOcrAdapter.hpp"
#include <cctype>
#include <stdexcept>
#include <iostream>

TesseractOcrAdapter::TesseractOcrAdapter()
    : tess(std::make_unique<tesseract::TessBaseAPI>()), inicializado(false) {}

TesseractOcrAdapter::~TesseractOcrAdapter() {
    finalizar();
}

void TesseractOcrAdapter::inicializar() {
    if (tess->Init(nullptr, "eng")) {
        throw std::runtime_error("Erro ao iniciar Tesseract OCR");
    }
    inicializado = true;
}

void TesseractOcrAdapter::finalizar() {
    if (inicializado && tess) {
        tess->End();
        inicializado = false;
    }
}

std::string TesseractOcrAdapter::extrairTexto(const cv::Mat& imagem,
                                             int roiX, int roiY,
                                             int roiWidth, int roiHeight) {
    if (!inicializado) {
        throw std::runtime_error("TesseractOcrAdapter não foi inicializado");
    }

    if (imagem.empty()) {
        throw std::runtime_error("Imagem vazia fornecida ao OCR");
    }

    // Configura a imagem para o Tesseract
    // A imagem deve estar em escala de cinza (1 canal)
    cv::Mat imagemGray;
    if (imagem.channels() == 3) {
        cv::cvtColor(imagem, imagemGray, cv::COLOR_BGR2GRAY);
    } else {
        imagemGray = imagem.clone();
    }

    // Aplica binarização para melhorar o OCR
    cv::Mat imagemBinaria;
    cv::threshold(imagemGray, imagemBinaria, 100, 255, cv::THRESH_BINARY);

    // Define a imagem e a ROI
    tess->SetImage(imagemBinaria.data, imagemBinaria.cols, imagemBinaria.rows, 
                   1, imagemBinaria.cols);
    tess->SetRectangle(roiX, roiY, roiWidth, roiHeight);

    // Extrai o texto
    char* resultado = tess->GetUTF8Text();
    std::string texto(resultado);
    delete[] resultado;

    // Filtra apenas dígitos
    std::string digitos = filtrarDigitos(texto);

    if (digitos.empty()) {
        throw std::runtime_error("OCR: Nenhum dígito válido encontrado");
    }

    return digitos;
}

void TesseractOcrAdapter::definirWhitelist(const std::string& whitelist) {
    if (!inicializado) {
        throw std::runtime_error("TesseractOcrAdapter não foi inicializado");
    }
    tess->SetVariable("tessedit_char_whitelist", whitelist.c_str());
}

std::string TesseractOcrAdapter::filtrarDigitos(const std::string& texto) const {
    std::string digitos;
    for (char c : texto) {
        if (std::isdigit(c)) {
            digitos += c;
        }
    }
    return digitos;
}
