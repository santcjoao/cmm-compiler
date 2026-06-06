CXX = g++
CXXFLAGS = -I. -g -std=c++20 -Wall -MMD -MP

SRC_DIR = src
BUILD_DIR = build

SRCS = Main.cpp $(wildcard $(SRC_DIR)/*.cpp)

OBJS = $(SRCS:%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

EXEC = $(BUILD_DIR)/compiler

all: $(EXEC)

$(EXEC): $(OBJS)
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	@$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Limpando..."
	@rm -rf $(BUILD_DIR)

run: $(EXEC)
	@if [ -z "$(FILE)" ]; then \
		echo "Erro: Informe o arquivo (ex: make run FILE=teste.cmm)"; \
	else \
		./$(EXEC) $(FILE); \
	fi

.PHONY: all clean run

-include $(DEPS)
