CXX := g++
CXXFLAGS := -std=c++20 -O2 -Wall -Wextra
BUILD_DIR := build

ifndef DAY
$(error DAY is not set. Example: make run DAY=01 PART=1 INPUT=in.txt)
endif

SRC := src/day$(DAY)/main.cpp src/aux.hpp
BIN := $(BUILD_DIR)/day$(DAY)
TEST_BIN := $(BUILD_DIR)/day$(DAY)_test

ifeq ($(MAKECMDGOALS),run)
ifndef PART
$(error PART missing: make run DAY=01 PART=1 INPUT=in.txt)
endif
endif

all:
	@echo "Usage:"
	@echo "  make run  DAY=01 PART=1 INPUT=in.txt"
	@echo "  make test DAY=01"

$(BIN): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I./src -o $@ $<

run: $(BIN)
ifndef INPUT
	$(error INPUT missing: make run DAY=01 PART=1 INPUT=in.txt)
endif
	./$(BIN) $(PART) $(INPUT)

$(TEST_BIN): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I./src -DTESTING -o $@ $<

test: $(TEST_BIN)
	./$(TEST_BIN)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all run test clean
