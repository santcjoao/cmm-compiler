CXX = g++
CXXFLAGS = -Iinclude -I. -std=c++17 -Wall -Wextra -pedantic -O2

SRCS = Main.cpp $(wildcard src/*.cpp)

ifeq ($(OS),Windows_NT)
	EXEC = compiler.exe
	RUN_CMD = compiler.exe
else
	EXEC = compiler
	RUN_CMD = ./compiler
endif

all: $(EXEC)

$(EXEC): $(SRCS)
	@echo "Compilando $(EXEC)..."
	@$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRCS)

clean:
	@echo "Limpando..."
	-@rm -f $(EXEC) compiler.exe

run: all
ifndef FILE
	@echo "Erro: especifique o arquivo de entrada."
	@echo "Uso: make run FILE=<caminho/para/arquivo.cmm>"
	@exit 1
endif
	$(RUN_CMD) $(FILE)

test: all
	@echo "=== Executando Bateria de Testes Válidos ==="
	@for f in tests/valid/*.cmm; do \
		echo "-> Testando $$f"; \
		$(RUN_CMD) "$$f" || exit 1; \
	done
	@echo "Todos os programas válidos compilaram com sucesso."

.PHONY: all clean run test
