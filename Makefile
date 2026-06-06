CXX = g++
CXXFLAGS = -I. -g -std=c++17 -Wall -MMD -MP

SRC_DIR = src
BUILD_DIR = build

SRCS = Main.cpp $(wildcard $(SRC_DIR)/*.cpp)

OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

EXEC = $(BUILD_DIR)/compiler

ifeq ($(OS),Windows_NT)
    MKDIR = if not exist "$(subst /,\,$1)" mkdir "$(subst /,\,$1)"
    RM = if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
    EXEC := $(EXEC).exe
else
    MKDIR = mkdir -p $1
    RM = rm -rf $(BUILD_DIR)
endif

all: $(EXEC)

$(EXEC): $(OBJS)
	@$(call MKDIR,$(dir $@))
	@$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	@echo "Compilando $<..."
	@$(call MKDIR,$(dir $@))
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Limpando..."
	@$(RM)

run: $(EXEC)
	@if [ -z "$(FILE)" ]; then \
		echo "Erro: Informe o arquivo (ex: make run FILE=teste.cmm)"; \
	else \
		./$(EXEC) $(FILE); \
	fi

.PHONY: all clean run

-include $(DEPS)
