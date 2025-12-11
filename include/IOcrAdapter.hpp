#ifndef IOCRADAPTER_HPP
#define IOCRADAPTER_HPP

#include <string>
#include <opencv2/opencv.hpp>

/**
 * Interface para adaptar diferentes engines de OCR
 * Define o contrato para processamento de imagens e extração de texto
 */
class IOcrAdapter {
public:
    virtual ~IOcrAdapter() = default;

    /**
     * Inicializa o adaptador OCR
     * @throws std::runtime_error se a inicialização falhar
     */
    virtual void inicializar() = 0;

    /**
     * Finaliza e libera recursos do adaptador
     */
    virtual void finalizar() = 0;

    /**
     * Processa uma imagem e extrai texto
     * @param imagem Mat do OpenCV contendo a imagem
     * @param roiX Coordenada X da Region of Interest
     * @param roiY Coordenada Y da Region of Interest
     * @param roiWidth Largura da Region of Interest
     * @param roiHeight Altura da Region of Interest
     * @return String com o texto extraído
     * @throws std::runtime_error se o processamento falhar
     */
    virtual std::string extrairTexto(const cv::Mat& imagem,
                                    int roiX, int roiY,
                                    int roiWidth, int roiHeight) = 0;

    /**
     * Configura caracteres permitidos para OCR
     * @param whitelist String com caracteres válidos (ex: "0123456789")
     */
    virtual void definirWhitelist(const std::string& whitelist) = 0;
};

#endif
