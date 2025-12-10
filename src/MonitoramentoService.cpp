#include "MonitoramentoService.hpp"
#include <thread>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <sstream>
#include <cctype> 
#include <opencv2/opencv.hpp> 
#include <tesseract/baseapi.h> 

using namespace std::chrono_literals; 
namespace fs = std::filesystem;

// Construtor - Inicialização do Tesseract removida daqui
MonitoramentoService::MonitoramentoService(const std::vector<ShaFolder>& pastas,
                       MonitoramentoBanco& bancoRef)
    : banco(bancoRef)
{
    for (const auto& p : pastas)
        pastasSHA.push_back(p);
}


// Função de Extração de Volume com Limpeza do Tesseract
std::string MonitoramentoService::extrairVolume(const std::string& caminhoImg, int roiX, int roiY, int roiWidth, int roiHeight) {
    
    // 1. INICIALIZAÇÃO POR IMAGEM (ESTADO LIMPO)
    if (tess.Init(nullptr, "eng")) {
        throw std::runtime_error("Erro ao iniciar Tesseract para leitura.");
    }
    tess.SetVariable("tessedit_char_whitelist", "0123456789"); 
    
    // 2. Carregar a imagem
    cv::Mat imagem = cv::imread(caminhoImg, cv::IMREAD_GRAYSCALE);

    if (imagem.empty()) {
        tess.End(); // Finaliza o Tesseract antes de lançar a exceção
        throw std::runtime_error("Não foi possível carregar a imagem ou o caminho está incorreto.");
    }
    
    // 3. Pré-processamento simples: Binarização
    cv::Mat processada;
    cv::threshold(imagem, processada, 100, 255, cv::THRESH_BINARY);
    
    // 4. Execução do Tesseract OCR
    tess.SetImage(processada.data, processada.cols, processada.rows, 1, processada.cols);
    tess.SetRectangle(roiX, roiY, roiWidth, roiHeight); // ROI dinâmico
    
    char* out = tess.GetUTF8Text();

    // 5. Limpeza e Filtragem
    std::string resultado(out);
    delete[] out; 
    
    std::string digitos_filtrados;
    for (char c : resultado) {
        if (std::isdigit(c)) {
            digitos_filtrados += c;
        }
    }
    
    // 6. ENCERRAMENTO (Libera o estado para a próxima leitura)
    tess.End(); 

    if (digitos_filtrados.empty()) {
        throw std::runtime_error("OCR: Não foram encontrados dígitos válidos para conversão.");
    }
    
    return digitos_filtrados; 
}


// Loop Principal de Monitoramento (Lógica Corrigida para Delta e Acumulação)
void MonitoramentoService::iniciar() {
    while (true) {
        for (const auto& pastaInfo : pastasSHA) {
            // Retry com espera se a pasta não existir (race condition com simulador)
            int tentativas = 0;
            const int MAX_TENTATIVAS = 10;
            const int ESPERA_MS = 500; // 500ms entre tentativas
            
            while ((!fs::exists(pastaInfo.pasta) || !fs::is_directory(pastaInfo.pasta)) && tentativas < MAX_TENTATIVAS) {
                tentativas++;
                std::this_thread::sleep_for(std::chrono::milliseconds(ESPERA_MS));
            }
            
            if (tentativas == MAX_TENTATIVAS) {
                continue;
            }

            for (const auto& entry : fs::directory_iterator(pastaInfo.pasta)) {
                
                if (!entry.is_regular_file() || 
                    (entry.path().extension() != ".png" && entry.path().extension() != ".jpg" && entry.path().extension() != ".jpeg" && entry.path().extension() != ".bmp")) {
                    continue;
                }
                
                std::string caminhoArquivo = entry.path().string();
                fs::file_time_type tempoModificacao = fs::last_write_time(entry.path());

                // Verifica se o arquivo é novo OU foi modificado
                if (arquivosLidos.find(caminhoArquivo) == arquivosLidos.end() || 
                    arquivosLidos[caminhoArquivo] < tempoModificacao) {
                    
                    std::string volumeStr; 
                    
                    try {
                        volumeStr = extrairVolume(caminhoArquivo, pastaInfo.roiX, pastaInfo.roiY, pastaInfo.roiWidth, pastaInfo.roiHeight); 
                        
                        if (volumeStr.empty()) {
                            throw std::runtime_error("Conversão: Valor de OCR vazio.");
                        }

                        int volume_atual = std::stoi(volumeStr);
                        int id_sha = pastaInfo.idSHA;
                        
                        // 1. OBTÉM A ÚLTIMA LEITURA ABSOLUTA DO BANCO (Valor antigo)
                        int ultima_leitura = banco.getUltimaLeituraAbsoluta(id_sha); 
                        
                        int delta_volume = 0;
                        
                        // 2. CALCULA O DELTA E ATUALIZA O CONSUMO TOTAL
                        if (volume_atual > ultima_leitura) {
                            delta_volume = volume_atual - ultima_leitura;
                            
                            // 3. ATUALIZA A SOMA TOTAL APENAS COM O DELTA (Consumo)
                            banco.adicionarVolumeGasto(id_sha, delta_volume);
                        }
                        
                        // 4. REGISTRA A NOVA LEITURA ABSOLUTA (Para o próximo cálculo de Delta)
                        banco.registrarLeitura(id_sha, volume_atual);
                        
                        arquivosLidos[caminhoArquivo] = tempoModificacao;

                    } 
                    catch (const std::invalid_argument& e) {
                        // Silenciosamente ignora erros de conversão
                    }
                    catch (const std::exception& e) {
                        // Silenciosamente ignora outros erros
                    }
                }
            }
        }
        
        std::this_thread::sleep_for(10s);
    }
}