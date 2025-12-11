# Painel de Monitoramento de Hidrômetros 💧

Sistema de monitoramento e gestão de dados de hidrômetros.

---

## 📊 Evolução Semanal do Projeto

### Estatísticas de Commits
![GitHub commit activity](https://img.shields.io/github/commit-activity/w/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square&label=Commits%20esta%20semana&color=blue)
![GitHub last commit](https://img.shields.io/github/last-commit/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square&label=Último%20commit&color=green)

### Atividade de Código
![GitHub code size](https://img.shields.io/github/languages/code-size/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square&label=Tamanho%20do%20código)
![GitHub repo size](https://img.shields.io/github/repo-size/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square&label=Tamanho%20do%20repositório)

---

## 📈 Gráfico de Atividade Semanal

![Estatísticas do Repositório](https://github-readme-stats.vercel.app/api/pin/?username=matos-ayrton&repo=painel_monitoramento_hidrometros&show_owner=true)

---

## 🏆 Métricas do Projeto

| Métrica | Status |
|---------|--------|
| **Total de Commits** | ![GitHub commits](https://img.shields.io/github/commit-activity/t/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square) |
| **Commits (Mês)** | ![GitHub commits month](https://img.shields.io/github/commit-activity/m/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square&color=orange) |
| **Commits (Semana)** | ![GitHub commits week](https://img.shields.io/github/commit-activity/w/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square&color=brightgreen) |
| **Linguagem Principal** | ![GitHub top language](https://img.shields.io/github/languages/top/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square) |
| **Issues Abertas** | ![GitHub issues](https://img.shields.io/github/issues/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square) |
| **Pull Requests** | ![GitHub pull requests](https://img.shields.io/github/issues-pr/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square) |

## 🚀 Sobre o Projeto

Sistema desenvolvido para monitoramento inteligente de hidrômetros, permitindo:
- 📈 Visualização em tempo real do consumo
- 📊 Relatórios detalhados
- 🔔 Alertas de consumo anormal
- 💾 Histórico de leituras

---

## 🛠️ Tecnologias

- **C++ 17** - Linguagem principal
- **OpenCV 4.6** - Processamento de imagens
- **Tesseract 5.3** - OCR para extração de dígitos
- **CMake** - Build system
- **SQLite** - Persistência de dados (banco local)

---

## 🏗️ Padrões de Projeto Utilizados

Este projeto utiliza vários padrões de design de forma integrada para garantir flexibilidade, manutenibilidade e separação de responsabilidades, conforme as especificações do projeto:

### 1. **Singleton (Fachada - Nível Geral)**
**Localização:** `include/Fachada.hpp`, `src/Fachada.cpp`
**O que é:** Garante que apenas uma instância da classe existe em toda a aplicação.
**Por que foi usado:** A `Fachada` é o ponto central de orquestração do sistema.

**Exemplo de uso:**
```cpp
Fachada& fachada = Fachada::getInstance();
fachada.iniciarMonitoramentoBackground();
```

### 3. **Builder Pattern (Subsistema de Usuários)**
**Localização:** `include/usuario/UsuarioBuilder.hpp`, `src/usuario/UsuarioBuilder.cpp`
**O que é:** Separa a construção de um objeto complexo de sua representação.
**Por que foi usado:** O Builder é usado para o cadastro de novos usuários (RF05) e vinculação de Contas (RF09) de forma controlada.


### 4. **Singleton (Subsistema de Logs)**
**Localização:** `include/log/Logger.hpp`, `src/log/Logger.cpp`
**O que é:** Garante que a classe responsável por registrar logs tenha apenas uma instância.
**Por que foi usado:** O `Logger` é o único ponto de acesso para registro de eventos (RF14, RF15) e logs de erro (RF16), garantindo integridade.

### 5. **Strategy Pattern (Monitoramento e Alertas)**
**Localização:** `include/ILeituraStrategy.hpp`, `src/LeituraEstaticaStrategy.cpp`
**O que é:** Define uma família de algoritmos, encapsula cada um e os torna intercambiáveis.
**Por que foi usado:**
* **Monitoramento:** Implementa o monitoramento individualizado (RF11) e em grupo (RF12) através da troca de estratégias de consulta e cálculo de consumo.
* **Alertas:** Permite configurar limites de consumo a nível individual ou por grupo de hidrômetros (RF18).

**Exemplo:**
```cpp
auto strategy = OcrStrategyFactory::createStrategy(
    StrategyType::ESTATICA, 
    270, 312, 550, 410,  // ROI específico para SHA 1
    ocrAdapter
);
```
Padrão 6 (Factory Pattern - Strategies)


### 6. **Factory Pattern (Criação de Strategies - Subsistema de Monitoramento)**
**Localização:** `include/OcrStrategyFactory.hpp`, `src/OcrStrategyFactory.cpp`
**O que é:** Encapsula a criação de objetos.
**Por que foi usado:** Desacopla o código que cria estratégias de leitura e relatórios (RF13) do código que os utiliza.

**Exemplo:**
```cpp
std::unique_ptr<ILeituraStrategy> strategy = 
    OcrStrategyFactory::createStrategy(StrategyType::ESTATICA, x, y, w, h, adapter);
```

### Padrão 7 (Adapter Pattern)

### 7. **Adapter Pattern (Monitoramento e Alertas)**
**Localização:** `include/IOcrAdapter.hpp`, `include/TesseractOcrAdapter.hpp`, `src/TesseractOcrAdapter.cpp`
**O que é:** Adapta uma interface existente para outra interface esperada pelo cliente.
**Por que foi usado:**
* **Monitoramento:** Garante que a fonte de dados do processamento de imagens (SHA) seja convertida para o formato `Consumo Detalhado` esperado (RF10).
* **Alertas:** Permite o envio de notificações por serviços externos (e-mail, SMS, API - RF21 a RF23) através de uma interface unificada.

**Exemplo:**
```cpp
auto ocrAdapter = std::make_shared<TesseractOcrAdapter>();
ocrAdapter->inicializar();
ocrAdapter->definirWhitelist("0123456789");
std::string digitos = ocrAdapter->extrairTexto(imagem, x, y, w, h);
```

### Padrão 8 (Mediator Pattern)

### 8. **Mediator Pattern (Coordenação de Leituras - Subsistema de Monitoramento)**
**Localização:** `include/IMonitoramentoMediator.hpp`, `include/MonitoramentoConcretoMediator.hpp`, `src/MonitoramentoConcretoMediator.cpp`
**O que é:** Define um objeto que encapsula como um conjunto de objetos interagem.
**Por que foi usado:** O mediator controla o fluxo da leitura (coleta -> cálculo delta -> notificação do banco), desacoplando o `MonitoramentoService` do `MonitoramentoBanco`.

**Exemplo:**
```cpp
mediator.notificarLeitura(idSHA, valorAtual, ultimaLeitura);
// Internamente: calcula delta, registra no banco, dispara alertas
```

### Padrão 9 (Observer Pattern)


### 9. **Observer Pattern (Subsistema de Alertas)**
**Localização:** `include/alerta/`, `include/notificacao/`
**O que é:** Define uma relação de dependência um-para-muitos.
**Por que foi usado:** Permite que múltiplos serviços (Alertas, Logs) reajam ao evento de **consumo excessivo** (RF19), sem acoplar o serviço de monitoramento.
