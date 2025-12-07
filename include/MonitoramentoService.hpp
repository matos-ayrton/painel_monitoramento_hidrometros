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
    MonitoramentoService(const std::vector<std::pair<int,std::string>>& pastas,
               MonitoramentoBanco& bancoRef);

    std::string extrairVolume(const std::string& caminhoImg);

    void iniciar();

private:
    struct ShaFolder {
        int idSHA;
        std::string pasta;
    };

    std::vector<ShaFolder> pastasSHA;
    std::unordered_map<std::string, fs::file_time_type> arquivosLidos;
    MonitoramentoBanco banco;

    tesseract::TessBaseAPI tess;
};

#endif