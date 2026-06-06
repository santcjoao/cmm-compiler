# Compilador C--

Implementação de um compilador para a linguagem C--. O processo inclui análise léxica, sintática e semântica, com emissão de diagnósticos detalhados.

## Estrutura do Projeto

```
.
├── Main.cpp                    # Ponto de entrada do programa
├── Makefile                    # Build system (Linux / macOS / Windows)
├── include/                    # Headers (.h)
│   ├── Token.h                 # Definição dos tokens e tipos
│   ├── SourceFile.h            # Carregamento do arquivo fonte
│   ├── Logger.h                # Sistema de diagnósticos (erros/avisos)
│   ├── Scanner.h               # Analisador léxico
│   ├── SymbolTable.h           # Tabela de símbolos
│   └── Parser.h                # Analisador sintático/semântico
├── src/                        # Implementações (.cpp)
│   ├── SourceFile.cpp
│   ├── Logger.cpp
│   ├── Scanner.cpp
│   ├── SymbolTable.cpp
│   └── Parser.cpp
├── tests/                      # Programas de teste em C--
│   ├── valid/                  # Programas válidos (devem compilar sem erros)
│   └── invalid/                # Programas inválidos (devem gerar erros)
└── compiler                    # Executável gerado pelo make (Linux/macOS)
    (ou compiler.exe no Windows)
```
## Como Compilar

A compilação é gerenciada via `make`. É necessário um compilador compatível com C++17. O Makefile é universal e compatível com Linux, macOS e Windows (nativo, MSYS2 ou WSL).

- **Gerar o executável:**
  ```bash
  make
  ```
- **Limpar artefatos de compilação:**
  ```bash
  make clean
  ```

## Como Executar

A chamada do compilador exige o caminho para o arquivo fonte alvo:

```bash
./compiler <arquivo.cmm>
```
(No Windows, utilize `compiler.exe <arquivo.cmm>`)


## Saídas

A saída relata sucesso ou exibe diagnósticos indicando arquivo, linha, coluna e trecho de código afetado (em formato semelhante ao GCC).

**Execução bem-sucedida:**
```text
Compilação encerrada com sucesso.
```

**Execução com falhas:**
```text
tests/invalid/chaos.cmm:15:8: erro: esperado '('
   15 |     if a == 5) { 
      |        ^
Compilação falhou com 1 erro(s).
```
