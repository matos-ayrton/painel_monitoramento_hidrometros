#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
#include "MonitoramentoService.hpp"

MonitoramentoService::MonitoramentoService(const std::vector<std::pair<int,std::string>>& pastas,
                       MonitoramentoBanco& bancoRef)
    : banco(bancoRef)
{
    for (auto& p : pastas)
        pastasSHA.push_back({ p.first, p.second });

    if (tess.Init(nullptr, "eng")) {
        throw std::runtime_error("Erro ao iniciar Tesseract.");
    }

    tess.SetVariable("tessedit_char_whitelist", "0123456789");
}