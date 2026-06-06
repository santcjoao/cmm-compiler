CXX = g++
CXXFLAGS = -I. -std=c++17 -Wall

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
	@echo "Compilando tudo..."
	@$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRCS)

clean:
	@echo "Limpando..."
ifeq ($(OS),Windows_NT)
	@-del /Q $(EXEC) 2>nul || true
else
	@-rm -f $(EXEC)
endif

run: all
	$(RUN_CMD) $(FILE)

.PHONY: all clean run
