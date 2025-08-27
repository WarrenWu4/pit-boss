CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude
WINFLAGS = -mwindows -municode
LIBFLAGS = -lsetupapi
SRC = $(wildcard src/*.cpp)
BUILD_DIR = build
EXE = pitboss.exe

# entry point
all: $(BUILD_DIR) $(EXE)

# ensure that build directory exists
$(BUILD_DIR):
	powershell -Command "if (!(Test-Path '$(BUILD_DIR)')) { New-Item -ItemType Directory -Path '$(BUILD_DIR)' }"

# just recompile everything
# don't give a shit anymore
$(EXE): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(WINFLAGS) $(LIBFLAGS) -o $(BUILD_DIR)/$(EXE)

# remove build artifacts
clean:
	powershell -Command "if (Test-Path '$(BUILD_DIR)') { Remove-Item -Recurse -Force '$(BUILD_DIR)' }"

# run the executable
run: $(BUILD_DIR) $(EXE)
	./$(BUILD_DIR)/$(EXE)

.PHONY: all run clean