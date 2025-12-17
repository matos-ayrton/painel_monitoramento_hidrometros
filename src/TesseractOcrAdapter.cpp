#include "TesseractOcrAdapter.hpp"
#include <cctype>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <stdexcept>
#include <vector>

TesseractOcrAdapter::TesseractOcrAdapter()
    : tess(std::make_unique<tesseract::TessBaseAPI>()), inicializado(false) {}

TesseractOcrAdapter::~TesseractOcrAdapter() { finalizar(); }

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

std::string TesseractOcrAdapter::extrairTexto(const cv::Mat &imagem, int roiX,
                                              int roiY, int roiWidth,
                                              int roiHeight) {
  if (!inicializado) {
    throw std::runtime_error("TesseractOcrAdapter não foi inicializado");
  }

  if (imagem.empty()) {
    throw std::runtime_error("Imagem vazia fornecida ao OCR");
  }

  // 1. Definição e extração da ROI para evitar processar a imagem inteira
  cv::Rect areaInteresse(roiX, roiY, roiWidth, roiHeight);
  // Garantir que a ROI está dentro dos limites da imagem original
  areaInteresse &= cv::Rect(0, 0, imagem.cols, imagem.rows);
  cv::Mat imagemRoi = imagem(areaInteresse).clone();

  // 2. Pré-processamento: Escala de cinza
  cv::Mat imagemGray;
  if (imagemRoi.channels() == 3) {
    cv::cvtColor(imagemRoi, imagemGray, cv::COLOR_BGR2GRAY);
  } else {
    imagemGray = imagemRoi.clone();
  }

  // 3. Binarização Inversa (Números brancos, fundo preto)
  // Isso é essencial para a detecção de contornos
  cv::Mat imagemBinaria;
  cv::threshold(imagemGray, imagemBinaria, 0, 255,
                cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

  // 4. Limpeza Morfológica (Remove ruídos e suaviza os dígitos)
  cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
  cv::morphologyEx(imagemBinaria, imagemBinaria, cv::MORPH_OPEN, kernel);

  // 5. Filtragem de Contornos para remover as divisórias verticais
  std::vector<std::vector<cv::Point>> contornos;
  cv::findContours(imagemBinaria, contornos, cv::RETR_EXTERNAL,
                   cv::CHAIN_APPROX_SIMPLE);

  for (const auto &contorno : contornos) {
    cv::Rect rect = cv::boundingRect(contorno);

    // Proporção largura/altura
    double aspect_ratio = static_cast<double>(rect.width) / rect.height;

    // Critérios para remover (divisórias são muito finas ou muito pequenas)
    // Se largura for menor que 15% da altura, pintamos de preto (removemos)
    if (aspect_ratio < 0.15 || rect.height < (imagemRoi.rows * 0.4)) {
      cv::rectangle(imagemBinaria, rect, cv::Scalar(0),
                    -1); // Preenche com preto
    }
  }

  // 6. Inverter para o padrão Tesseract (Fundo branco, dígitos pretos)
  cv::Mat imagemFinal;
  cv::bitwise_not(imagemBinaria, imagemFinal);

  // 7. Configuração do Tesseract
  // PSM_SINGLE_LINE (7) ou PSM_SINGLE_BLOCK (6) costumam ser melhores aqui
  tess->SetPageSegMode(tesseract::PSM_SINGLE_LINE);
  tess->SetVariable("tessedit_char_whitelist", "0123456789");

  // Define a imagem final processada
  tess->SetImage(imagemFinal.data, imagemFinal.cols, imagemFinal.rows, 1,
                 imagemFinal.cols);

  // 8. Extração e filtragem final
  char *resultado = tess->GetUTF8Text();
  std::string texto(resultado ? resultado : "");
  delete[] resultado;

  std::string digitos = filtrarDigitos(texto);

  if (digitos.empty()) {
    throw std::runtime_error(
        "OCR: Nenhum dígito válido encontrado após processamento");
  }

  return digitos;
}

void TesseractOcrAdapter::definirWhitelist(const std::string &whitelist) {
  if (!inicializado) {
    throw std::runtime_error("TesseractOcrAdapter não foi inicializado");
  }
  tess->SetVariable("tessedit_char_whitelist", whitelist.c_str());
}

std::string
TesseractOcrAdapter::filtrarDigitos(const std::string &texto) const {
  std::string digitos;
  for (char c : texto) {
    if (std::isdigit(c)) {
      digitos += c;
    }
  }
  return digitos;
}

bool TesseractOcrAdapter::isInitialized() { return inicializado; }