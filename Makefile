CXX=g++
CXXFLAGS=-Wall -std=c++17 -Iinclude
EXE=pitboss.exe
SRC_DIR=src
BUILD_DIR=build
SRC=$(wildcard $(SRC_DIR)/*.cpp)
OBJ=$(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o, $(SRC))

# entry point
all: $(BUILD_DIR) $(EXE)

# ensure that build directory exists
$(BUILD_DIR):
	powershell -Command "if (!(Test-Path '$(BUILD_DIR)')) { New-Item -ItemType Directory -Path '$(BUILD_DIR)' }"

# link object files
$(EXE): $(OBJ)
	$(CXX) $^ -o $(BUILD_DIR)/$(EXE)

# build object files
$(OBJ): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# remove build artifacts
clean:
	powershell -Command "if (Test-Path '$(BUILD_DIR)') { Remove-Item -Recurse -Force '$(BUILD_DIR)' }"

# run the executable
run: $(EXE)
	./$(BUILD_DIR)/$(EXE)

.PHONY: all run clean