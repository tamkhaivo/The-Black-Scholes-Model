# OS Detection
ifeq ($(OS),Windows_NT)
	RM = del /Q /S
	RMDIR = rmdir /S /Q
	MKDIR = mkdir
	EXT = .exe
	FIX_PATH = $(subst /,\,$(1))
else
	RM = rm -f
	RMDIR = rm -rf
	MKDIR = mkdir -p
	EXT = 
	FIX_PATH = $(1)
endif

CXX      := g++
CXXFLAGS := -std=c++20 -Wall -Wextra -O3 -march=native -Iinclude
LDFLAGS  := 

# Directories
SRC_DIR       := src
INCLUDE_DIR   := include
TEST_DIR      := tests
BENCH_DIR     := benchmarks
BUILD_DIR     := build

# Targets
TARGET_APP    := $(BUILD_DIR)/black_scholes$(EXT)
TARGET_TEST   := $(BUILD_DIR)/test_suite$(EXT)
TARGET_BENCH  := $(BUILD_DIR)/benchmark$(EXT)

# Source Lists
# Wildcard might be evaluating early or failing on Windows if not careful? 
# Using explicit list for debugging or try different syntax. Usually wildcard works.
SRCS          := src/main.cpp src/Benchmark.cpp src/The_Black_Scholes_Model_Reference.cpp src/The_Black_Scholes_Model_Value.cpp
OBJS          := $(patsubst src/%.cpp, $(BUILD_DIR)/%.o, $(SRCS))

# TEST_SRCS
TEST_SRCS     := $(wildcard $(TEST_DIR)/*.cpp)
TEST_EXES     := $(patsubst $(TEST_DIR)/%.cpp, $(BUILD_DIR)/%$(EXT), $(TEST_SRCS))

# BENCH_SRCS
BENCH_SRCS    := $(wildcard $(BENCH_DIR)/*.cpp)
BENCH_OBJS    := $(patsubst $(BENCH_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(BENCH_SRCS))

# Default Target
all: $(TARGET_APP)

run: $(TARGET_APP)
	$(call FIX_PATH,$(TARGET_APP))

# Main Application
$(TARGET_APP): $(OBJS)
	@if not exist $(BUILD_DIR) $(MKDIR) $(BUILD_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^

# Tests
# Build and run all test executables
test: $(TEST_EXES)
	@$(foreach exe,$(TEST_EXES),$(call FIX_PATH,$(exe)) &&) echo All tests passed.

# Pattern rule for building test executables
$(BUILD_DIR)/%$(EXT): $(TEST_DIR)/%.cpp
	@if not exist $(BUILD_DIR) $(MKDIR) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@

# Benchmarks
benchmark: $(TARGET_BENCH)
	./$(TARGET_BENCH)

$(TARGET_BENCH): $(BENCH_OBJS)
	@if not exist $(BUILD_DIR) $(MKDIR) $(BUILD_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^

# Object Compilation for src
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist $(BUILD_DIR) $(MKDIR) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(BENCH_DIR)/%.cpp
	@if not exist $(BUILD_DIR) $(MKDIR) $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


clean:
	@if exist $(BUILD_DIR) $(RMDIR) $(BUILD_DIR)
	@echo "Cleaned build directory."

.PHONY: all test benchmark clean run
