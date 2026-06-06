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
	@echo "Compilando..."
	@$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRCS)

clean:
	@echo "Limpando..."
	-@rm -f $(EXEC) compiler.exe

run: all
	$(RUN_CMD) $(FILE)

.PHONY: all clean run
