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

# Define o nome do executável e lista todos os arquivos .cpp
# Isso encontra todos os arquivos *.cpp na pasta 'src'
file(GLOB SOURCE_FILES "src/*.cpp")
add_executable(monitoramento ${SOURCE_FILES})

# Linka o executável com as bibliotecas
# O alvo 'monitoramento' precisa dessas bibliotecas para funcionar
target_link_libraries(monitoramento 
    ${TESSERACT_LIBRARIES}
    ${OpenCV_LIBS}
    pthread # Essencial para std::thread
)