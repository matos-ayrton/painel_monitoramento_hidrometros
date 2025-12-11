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

Este projeto utiliza vários padrões de design de forma integrada para garantir flexibilidade, manutenibilidade e separação de responsabilidades:

### 1. **Singleton (Fachada)**
**Localização:** `include/Fachada.hpp`, `src/Fachada.cpp`

**O que é:** Garante que apenas uma instância da classe existe em toda a aplicação.

**Por que foi usado:**
- A `Fachada` é o ponto central de orquestração do sistema (inicializa adaptador OCR, mediator, serviço de monitoramento).
- Múltiplas threads e componentes precisam acessar a mesma instância para coordenar leituras.
- Evita duplicação de recursos custosos (adaptador Tesseract, banco de dados).

**Exemplo de uso:**
```cpp
Fachada& fachada = Fachada::getInstance();
fachada.iniciarMonitoramentoBackground();
```

---

### 2. **Strategy Pattern (Leitura de Valores)**
**Localização:** `include/ILeituraStrategy.hpp`, `src/LeituraEstaticaStrategy.cpp`

**O que é:** Define uma família de algoritmos, encapsula cada um e os torna intercambiáveis.

**Por que foi usado:**
- Diferentes estratégias de leitura podem ser implementadas (estática, dinâmica, adaptativa).
- Atualmente usa `LeituraEstaticaStrategy`, mas permite adicionar novas estratégias sem alterar código existente.
- O ROI (Region of Interest) e parâmetros da estratégia são configuráveis por hidrômetro.

**Exemplo:**
```cpp
auto strategy = OcrStrategyFactory::createStrategy(
    StrategyType::ESTATICA, 
    270, 312, 550, 410,  // ROI específico para SHA 1
    ocrAdapter
);
```

---

### 3. **Factory Pattern (Criação de Strategies)**
**Localização:** `include/OcrStrategyFactory.hpp`, `src/OcrStrategyFactory.cpp`

**O que é:** Encapsula a criação de objetos, centralizando a lógica de instanciação.

**Por que foi usado:**
- Desacopla o código que cria strategies do código que as usa.
- Facilita adicionar novos tipos de estratégias sem modificar o cliente.
- Centraliza a passagem do adaptador OCR para as strategies.

**Exemplo:**
```cpp
std::unique_ptr<ILeituraStrategy> strategy = 
    OcrStrategyFactory::createStrategy(StrategyType::ESTATICA, x, y, w, h, adapter);
```

---

### 4. **Adapter Pattern (OCR Abstraction)**
**Localização:** `include/IOcrAdapter.hpp`, `include/TesseractOcrAdapter.hpp`, `src/TesseractOcrAdapter.cpp`

**O que é:** Adapta uma interface existente para outra interface esperada pelo cliente.

**Por que foi usado:**
- **Desacoplamento da dependência Tesseract:** A estratégia não conhece diretamente Tesseract; interage via `IOcrAdapter`.
- **Flexibilidade futura:** Permite substituir Tesseract por outro OCR (Google Vision, EasyOCR) sem alterar `LeituraEstaticaStrategy`.
- **Controle centralizado:** Whitelist, configurações de OCR e tratamento de erros ficam localizados no adapter.

**Responsabilidades do Adapter:**
- Inicializar/finalizar Tesseract
- Definir whitelist de caracteres permitidos
- Extrair texto de ROI específico em imagem

**Exemplo:**
```cpp
auto ocrAdapter = std::make_shared<TesseractOcrAdapter>();
ocrAdapter->inicializar();
ocrAdapter->definirWhitelist("0123456789");
std::string digitos = ocrAdapter->extrairTexto(imagem, x, y, w, h);
```

---

### 5. **Mediator Pattern (Coordenação de Leituras)**
**Localização:** `include/IMonitoramentoMediator.hpp`, `include/MonitoramentoConcretoMediator.hpp`, `src/MonitoramentoConcretoMediator.cpp`

**O que é:** Define um objeto que encapsula como um conjunto de objetos interagem.

**Por que foi usado:**
- **Desacoplamento entre componentes:** `MonitoramentoService` e `MonitoramentoBanco` não se comunicam diretamente.
- **Lógica centralizada:** O mediator controla quando uma leitura é registrada, calcula deltas e notifica o banco.
- **Facilita manutenção:** Mudanças na lógica de coordenação ficam em um único lugar.

**Responsabilidades do Mediator:**
- Receber notificação de nova leitura do `MonitoramentoService`
- Calcular delta (diferença) em relação à leitura anterior
- Registrar no banco de dados
- Notificar serviços de alerta (se necessário)

**Exemplo:**
```cpp
mediator.notificarLeitura(idSHA, valorAtual, ultimaLeitura);
// Internamente: calcula delta, registra no banco, dispara alertas
```

---

### 6. **Observer Pattern (Implícito via Mediator)**
**Localização:** Implementado através do mediator e possíveis serviços de alerta.

**O que é:** Define uma relação um-para-muitos onde mudanças em um objeto notificam automaticamente seus dependentes.

**Por que foi usado:**
- Permite que múltiplos serviços (alertas, logs, notificações) reajam a novas leituras.
- O `MonitoramentoService` não precisa conhecer todos os "observadores".

---

## 📊 Fluxo Arquitetural

```
main()
  ↓
CLI (interface do usuário)
  ↓
Fachada (orquestrador central - Singleton)
  ├─→ MonitoramentoService (inicia em thread)
  │    ├─→ Itera pastas SHA
  │    ├─→ Chama Strategy (LeituraEstaticaStrategy)
  │    │    └─→ Usa IOcrAdapter (TesseractOcrAdapter)
  │    └─→ Notifica Mediator
  │
  ├─→ MonitoramentoConcretoMediator
  │    └─→ Registra no MonitoramentoBanco
  │
  └─→ OcrStrategyFactory (cria strategies)
```

---

## 🔄 Por Que Esses Padrões Juntos?

1. **Manutenibilidade:** Cada componente tem uma responsabilidade clara.
2. **Testabilidade:** Interfaces bem definidas facilitam testes unitários com mocks.
3. **Extensibilidade:** Novos hidrômetros, estratégias e OCRs podem ser adicionados sem recompilação de código existente.
4. **Reutilização:** A `Fachada` encapsula complexidade; o CLI fica simples.
5. **Escalabilidade:** Possibilita adicionar mais pastas, mediators e serviços de alerta futuramente.

---

## 📝 Como Usar

```bash
# Clone o repositório
git clone https://github.com/SEU_USUARIO/painel_monitoramento_hidrometros.git

# Entre na pasta
cd painel_monitoramento_hidrometros
```


**Última atualização:** ![GitHub last commit](https://img.shields.io/github/last-commit/matos-ayrton/painel_monitoramento_hidrometros?style=flat-square)
