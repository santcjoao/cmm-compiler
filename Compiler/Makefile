CXX = g++
CXXFLAGS = -I. -g -std=c++20

SRC = Main.cpp Lexer/Scanner.cpp
BUILD_DIR = Build
OBJ = $(SRC:%.cpp=$(BUILD_DIR)/%.o)
EXEC = $(BUILD_DIR)/compiler

all: $(EXEC)

$(EXEC): $(OBJ)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

run: $(EXEC)
	./$(EXEC) ./Testes/test.cmm
