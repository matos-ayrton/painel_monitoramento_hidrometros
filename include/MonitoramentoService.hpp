#ifndef MONITORAMENTOSERVICE_HPP
#define MONITORAMENTOSERVICE_HPP

#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>

#include "MonitoramentoBanco.hpp"

namespace fs = std::filesystem;

class MonitoramentoService
{
public:
    struct ShaFolder {
        int idSHA;
        std::string pasta;
        int roiX;
        int roiY;
        int roiWidth;
        int roiHeight;
    };

    MonitoramentoService(const std::vector<ShaFolder>& pastas,
               MonitoramentoBanco& bancoRef);

    std::string extrairVolume(const std::string& caminhoImg, int roiX, int roiY, int roiWidth, int roiHeight);

    void iniciar();

private:
    std::vector<ShaFolder> pastasSHA;
    // Mapa para registrar arquivos já lidos, usando o tempo de última modificação
    std::unordered_map<std::string, fs::file_time_type> arquivosLidos;
    
    // CORREÇÃO CRUCIAL: Uso de REFERÊNCIA para garantir que o serviço altere o banco original.
    MonitoramentoBanco& banco; 

    tesseract::TessBaseAPI tess;
};

#endif