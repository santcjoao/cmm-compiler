# Compilador C--

**Autores:**
- Nathan de Almeida Rezende
- Thiago Fernandes Guimarães Silva
- João Pedro Sant Ana de Carvalho

Compilador para a linguagem C-- com análise léxica, sintática e semântica (apenas diferenciação
entre IDs e palavras reservadas e criação de escopos).

## Estrutura do Código-Fonte

```text
.
├── Main.cpp                    # Ponto de entrada
├── Makefile                    # Script de compilação
├── include/                    # Arquivos de cabeçalho (.h)
│   ├── Logger.h                # Sistema de diagnósticos
│   ├── Parser.h                # Analisador sintático/semântico
│   ├── Scanner.h               # Analisador léxico
│   ├── SourceFile.h            # Leitura do arquivo de entrada
│   ├── SymbolTable.h           # Tabela de símbolos e escopos
│   └── Token.h                 # Definição da estrutura de tokens
└── src/                        # Implementações (.cpp)
    ├── Logger.cpp
    ├── Parser.cpp
    ├── Scanner.cpp
    ├── SourceFile.cpp
    └── SymbolTable.cpp
```

## Compilação

O build é gerenciado pelo utilitário `make` e exige suporte a **C++17**.

- **Linux / macOS**: Execute `make` no terminal. O executável `compiler` será gerado.

Para remover os artefatos de compilação, utilize `make clean`.

## Execução

```bash
./compiler <arquivo.cmm>
```

Em caso de falha, os diagnósticos são exibidos no formato padrão GCC (`arquivo:linha:coluna: erro/aviso`), apontando a posição exata do código afetado.
