# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Wpedantic -Werror

# Source files and output
SRC := $(wildcard *.cpp)
OBJ := $(SRC:.cpp=.o)
BIN := a.exe

# Debug build (includes sanitizers and debug symbols)
DEBUG_CXXFLAGS := $(CXXFLAGS) -g -O1 -fsanitize=address,undefined -fno-omit-frame-pointer -D DEBUG
DEBUG_LDFLAGS := -fsanitize=address,undefined 

# Release build (optimized, no sanitizers)
RELEASE_CXXFLAGS := $(CXXFLAGS) -O2
RELEASE_LDFLAGS := -pthread

# Valgrind mode (debug build, but runs with Valgrind)
VALGRIND := valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose

.PHONY: all debug release valgrind clean

all: debug

debug: CXXFLAGS := $(DEBUG_CXXFLAGS)
debug: LDFLAGS := $(DEBUG_LDFLAGS)
debug: $(BIN)

release: CXXFLAGS := $(RELEASE_CXXFLAGS)
release: LDFLAGS := $(RELEASE_LDFLAGS)
release: $(BIN)

valgrind: debug
	$(VALGRIND) ./$(BIN)

$(BIN): $(OBJ)
	$(CXX) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(BIN) $(OBJ)
