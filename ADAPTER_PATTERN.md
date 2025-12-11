## 🎯 Adapter Pattern - Tesseract OCR

### Resumo da Implementação

Um **Adapter** foi implementado para adaptar a API do Tesseract OCR à sua arquitetura, permitindo:
- ✅ Isolamento da dependência de Tesseract
- ✅ Facilidade para trocar ou adicionar novos OCRs no futuro
- ✅ Interface limpa e simples para processamento de imagens

---

### 📋 Arquivos Criados

#### 1. **IOcrAdapter.hpp** (Interface/Contrato)
- Define a interface que todo adaptador OCR deve seguir
- Métodos principais:
  - `inicializar()` - Prepara o adaptador
  - `finalizar()` - Libera recursos
  - `extrairTexto()` - Processa imagem e extrai texto
  - `definirWhitelist()` - Configura caracteres permitidos

#### 2. **TesseractOcrAdapter.hpp / TesseractOcrAdapter.cpp**
- Implementação concreta do adaptador para Tesseract
- Encapsula toda a lógica do Tesseract
- Métodos privados auxiliares como `filtrarDigitos()`

---

### 🔄 Fluxo de Funcionamento

```
┌─────────────────┐
│   Fachada       │  Cria e inicializa
└────────┬────────┘
         │
         ↓
┌─────────────────────────────┐
│ TesseractOcrAdapter         │  Implementação concreta
│ (IOcrAdapter)              │
└────────┬────────────────────┘
         │
         ↓
┌─────────────────────────────┐
│ LeituraEstaticaStrategy     │  Usa o adapter para extrair valor
│ (ILeituraStrategy)          │
└────────┬────────────────────┘
         │
         ↓
┌─────────────────────────────┐
│ MonitoramentoService        │  Chama a strategy
└─────────────────────────────┘
```

---

### 📝 Arquivos Modificados

#### **include/Fachada.hpp**
- Adicionado: `std::shared_ptr<IOcrAdapter> ocrAdapter;`
- O adaptador é compartilhado com as strategies

#### **include/LeituraEstaticaStrategy.hpp**
- Adicionado parâmetro `std::shared_ptr<IOcrAdapter> adapter` ao construtor
- Membro privado: `ocrAdapter`

#### **src/LeituraEstaticaStrategy.cpp**
- Simplificado drasticamente (removeu lógica do Tesseract)
- Agora apenas carrega a imagem e delega OCR ao adapter

#### **include/OcrStrategyFactory.hpp**
- Novo parâmetro: `std::shared_ptr<IOcrAdapter> adapter`

#### **src/Fachada.cpp**
- Inicializa o `TesseractOcrAdapter` no construtor
- Configura whitelist para dígitos
- Finaliza o adapter no destrutor
- Passa o adapter ao factory

#### **CMakeLists.txt**
- Adicionado: `src/TesseractOcrAdapter.cpp`

---

### ✨ Benefícios

1. **Separação de Responsabilidades**: OCR isolado em seu próprio componente
2. **Reutilização**: O mesmo adapter pode ser usado por múltiplas strategies
3. **Manutenibilidade**: Alterações no Tesseract ficam localizadas
4. **Extensibilidade**: Adicionar novos OCRs é simples (criar novo adapter)
5. **Testabilidade**: Pode-se mockar o adaptador para testes unitários

---

### 🚀 Como Estender

Para adicionar um novo OCR (ex: EasyOCR), basta:

```cpp
// 1. Criar nova implementação
class EasyOcrAdapter : public IOcrAdapter {
    // Implementar métodos da interface
};

// 2. Usar da mesma forma
auto adapter = std::make_shared<EasyOcrAdapter>();
adapter->inicializar();
```

A estratégia e o resto do código continuam funcionando sem mudanças! 🎉
