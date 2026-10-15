# C-- Compiler: Lexical, Syntactic, and Scoped Semantic Front-End

Front-end completo de compilação desenvolvido em **C++17** para a linguagem **C--** (subconjunto procedural e tipado de C com suporte a funções, tipos primitivos `int`, `char`, `void`, controle de fluxo `if-else`, laços iterativos `while` e `for`, vetores unidimensionais indexados e resolução de escopos aninhados).

O sistema implementa do zero a leitura de fluxo de entrada, análise léxica sob demanda, parsing sintático descendente recursivo $LL(1)$ com recuperação de erros (*Panic-Mode*), tabela de símbolos hierárquica e um subsistema de diagnóstico de erros compatível com o padrão industrial do GCC/Clang.

---

## 1. Arquitetura e Pipeline de Fases

```text
Código Fonte (.cmm)
       │
       ▼
 ┌────────────┐
 │ SourceFile │  Leitura com indexação bidimensional (linha, coluna e offsets)
 └─────┬──────┘
       │
       ▼
 ┌────────────┐
 │  Scanner   │  Autômatos finitos para extração de Tokens ⟨TokenType, Lexeme⟩
 └─────┬──────┘
       │  Token sob demanda (Lookahead)
       ▼
 ┌────────────┐
 │   Parser   │  Gramática LL(1) preditiva descendente recursiva
 └──┬───────┬─┘
    │       │
    │       ▼
    │ ┌─────────────┐
    │ │ SymbolTable │  Árvore hierárquica de escopos léxicos (Global -> Função -> Bloco)
    │ └─────────────┘
    ▼
 ┌────────────┐
 │   Logger   │  Formatador de diagnósticos com linha fonte e caret pointer (^~)
 └────────────┘
```

### Componentes Centrais:
* **`SourceFile` ([`include/SourceFile.h`](include/SourceFile.h)):** Mapeia o arquivo-fonte em memória, provendo indexação por linha e coluna e extração de trechos de código para mensagens de erro contextualizadas.
* **`Scanner` ([`include/Scanner.h`](include/Scanner.h)):** Analisador léxico determinístico com suporte a descarte de comentários (`//` e `/* */`), tratamento de sequências de escape em caracteres (`\n`, `\0`, `\t`) e diferenciação entre palavras-chave reservadas e identificadores.
* **`Parser` ([`include/Parser.h`](include/Parser.h)):** Analisador sintático preditivo descendente recursivo estruturado sobre a gramática livre de contexto $LL(1)$ de C--, equipado com conjunto de sincronização (*Panic-Mode*) para relatar múltiplos erros sem encerrar prematuramente a compilação.
* **`SymbolTable` ([`include/SymbolTable.h`](include/SymbolTable.h)):** Tabela de símbolos encadeada que implementa escopos léxicos estáticos (*Static Scoping*). Cada bloco `{ ... }` instancia um novo escopo com ponteiro para o escopo pai (`parent`), garantindo resolução correta de variáveis locais, parâmetros e variáveis globais, além de detecção de shadowing e redeclarações ilegais.
* **`Logger` ([`include/Logger.h`](include/Logger.h)):** Formatador de diagnósticos no padrão GCC (`arquivo:linha:coluna: erro/aviso`), exibindo o trecho exato de código e o cursor de indicação (`^~`).

---

## 2. Especificação da Linguagem C--

### Tipos e Operadores Suportados:
* **Tipos Primitivos:** `int`, `char`, `void`.
* **Estruturas de Dados:** Vetores unidimensionais indexados (`int arr[10];`).
* **Operadores Aritméticos:** `+`, `-`, `*`, `/`.
* **Operadores de Comparação e Relacionais:** `==`, `!=`, `<`, `<=`, `>`, `>=`.
* **Operadores Lógicos e Atribuição:** `&&`, `||`, `!`, `=`.
* **Controle de Fluxo:** `if (cond) ... else ...`, `while (cond) ...`, `for (init; cond; inc) ...`, `return expr;`.

### Exemplo de Programa Válido (`tests/valid/fatorial.cmm`):
```c
int calcular_fatorial(int n) {
    int resultado;
    resultado = 1;
    
    while (n > 1) {
        resultado = resultado * n;
        n = n - 1;
    }
    
    return resultado;
}

void main() {
    int numero;
    int fat;
    
    numero = 5;
    fat = calcular_fatorial(numero);
}
```

---

## 3. Sistema de Diagnósticos e Recuperação de Falhas

O compilador reporta erros léxicos, sintáticos e semânticos de forma unificada, com localização precisa e recuperação de fluxo para identificação de erros subsequentes:

```text
$ ./compiler tests/invalid/syntax_errors.cmm
tests/invalid/syntax_errors.cmm:8:5: erro: esperado ';'
   8 |     if (a > 3 { 
     |     ^~
tests/invalid/syntax_errors.cmm:8:15: erro: esperado ')'
   8 |     if (a > 3 { 
     |               ^
tests/invalid/syntax_errors.cmm:10:17: erro: expressão inválida ou incompleta
   10 |         a = a + * 5; 
      |                 ^
Compilação falhou com 3 erro(s).
```

---

## 4. Estrutura de Diretórios

```text
Compilador/
├── Main.cpp                    # Ponto de entrada do executável
├── Makefile                    # Automação de compilação e suíte de testes
│
├── include/                    # Interfaces e declarações de tipos (.h)
│   ├── Logger.h                # Subsistema de logging e formatação de diagnósticos
│   ├── Parser.h                # Parser preditivo descendente recursivo LL(1)
│   ├── Scanner.h               # Analisador léxico baseado em autômatos
│   ├── SourceFile.h            # Buffer e indexador de coordenadas do arquivo fonte
│   ├── SymbolTable.h           # Tabela de símbolos hierárquica e escopos
│   └── Token.h                 # Estrutura de token e enumerações TokenType
│
├── src/                        # Implementações dos módulos (.cpp)
│   ├── Logger.cpp
│   ├── Parser.cpp
│   ├── Scanner.cpp
│   ├── SourceFile.cpp
│   └── SymbolTable.cpp
│
└── tests/                      # Casos de teste estruturados
    ├── valid/                  # Programas válidos em C-- (fatorial, fibonacci, arrays)
    └── invalid/                # Casos com erros léxicos, sintáticos e semânticos deliberados
```

---

## 5. Compilação e Execução

### Pré-requisitos
* Compilador C++ com suporte a **C++17** (`g++` ou `clang++`).
* GNU Make.

### Compilação do Binário
```bash
make
```
Gera o executável `compiler` (ou `compiler.exe` no Windows).

### Execução da Suíte de Testes
```bash
make test
```
Executa automaticamente a validação de todos os programas presentes em `tests/valid/`.

### Compilar um Arquivo Específico
```bash
make run FILE=tests/valid/fatorial.cmm

# Ou invocação direta:
./compiler tests/valid/fibonacci.cmm
./compiler tests/invalid/semantic_errors.cmm
```

### Limpeza
```bash
make clean
```

---

## 6. Co-autoria e Matriz de Responsabilidade

Projeto desenvolvido em colaboração para a disciplina de Compiladores (UFMS):

| Autor | Responsabilidade Técnica Principal |
| :--- | :--- |
| **João Pedro Sant'Ana de Carvalho** ([@santcjoao](https://github.com/santcjoao)) | Arquitetura do subsistema de diagnósticos e formatação de erros (`Logger` e `SourceFile`) no padrão GCC/Clang, mecanismo de recuperação de erros com múltiplos relatórios antes do aborto (*Panic-Mode*), refatoração da tipagem e enumeração de tokens (`Token.h`), e automação de build multiplataforma (`Makefile`). |
| **Nathan Rezende** ([@nahnn](https://github.com/nahnn)) | Concepção e implementação do Analisador Léxico (`Scanner`), autômatos finitos de palavras reservadas e literais, produções gramaticais iniciais do Analisador Sintático (`Parser`) e estrutura de dados da Tabela de Símbolos (`SymbolTable`). |
| **Thiago Fernandes** ([@thiagofernandesGS](https://github.com/thiagofernandesGS)) | Calibração de tokens e operadores relacionais no scanner, refinamento e correção de produções gramaticais no parser preditivo e métodos de inserção/consulta na tabela de símbolos. |
