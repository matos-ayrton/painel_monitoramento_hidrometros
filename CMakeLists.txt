# Define a versão mínima do CMake (Versão 3.10 ou mais recente)
cmake_minimum_required(VERSION 3.10)

# Nomeia o seu projeto (o executável final será 'monitoramento')
project(MonitoramentoOCR CXX)

# Adiciona a pasta 'include' ao caminho de busca de headers
include_directories(include)

find_package(PkgConfig REQUIRED)
pkg_check_modules(TESSERACT REQUIRED tesseract)
# Usa as variáveis geradas pelo pkg-config
include_directories(${TESSERACT_INCLUDE_DIRS})

# 2. ENCONTRAR O OPENCV
find_package(OpenCV REQUIRED)

# 3. ENCONTRAR LIBCURL (Novo)
find_package(CURL REQUIRED)

# Define o nome do executável e lista explicitamente todos os arquivos .cpp
# Listagem explícita evita problemas com glob e garante inclusão dos novos arquivos
set(SOURCE_FILES
    src/main.cpp
    src/CLI.cpp
    src/Fachada.cpp
    src/MonitoramentoConcretoMediator.cpp
    src/OcrStrategyFactory.cpp
    src/LeituraEstaticaStrategy.cpp
    src/TesseractOcrAdapter.cpp
    src/AlertService.cpp
    src/Logger.cpp
    src/MonitoramentoBanco.cpp
    src/MonitoramentoService.cpp
    src/NotificacaoService.cpp
    src/EmailNotificacaoStrategy.cpp
    src/SMSNotificacaoStrategy.cpp
    src/WebhookNotificacaoStrategy.cpp
    src/PerfilManager.cpp
    src/UsuarioService.cpp
    src/DB.cpp
    src/UserBuilder.cpp
    src/UsuarioService.cpp
)

add_executable(monitoramento ${SOURCE_FILES})

# Linka o executável com as bibliotecas
# O alvo 'monitoramento' precisa dessas bibliotecas para funcionar
target_link_libraries(monitoramento 
    ${TESSERACT_LIBRARIES}
    ${OpenCV_LIBS}
    pthread # Essencial para std::thread
    sqlite3
    CURL::libcurl
)

# Executável utilitário para encontrar ROI
add_executable(find_roi src/find_roi.cpp)
target_link_libraries(find_roi ${OpenCV_LIBS})