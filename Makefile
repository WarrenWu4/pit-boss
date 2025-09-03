CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude
WINFLAGS = -mwindows -municode
LIBFLAGS = -lsetupapi
SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, $(BUILD_DIR)/%.o, $(SRC))
BUILD_DIR = build
RESOURCE_DIR = resources
EXE = pitboss.exe

# entry point
all: $(BUILD_DIR) $(EXE)

# ensure that build directory exists
$(BUILD_DIR):
	powershell -Command "if (!(Test-Path '$(BUILD_DIR)')) { New-Item -ItemType Directory -Path '$(BUILD_DIR)' }"

# make a copy of resources in build
$(BUILD_DIR)/$(RESOURCE_DIR): $(BUILD_DIR)
	powershell -Command "if (!(Test-Path '$(BUILD_DIR)/$(RESOURCE_DIR)')) { New-Item -ItemType Directory -Path '$(BUILD_DIR)/$(RESOURCE_DIR)' }"
	powershell -Command "Copy-Item -Path '$(RESOURCE_DIR)/*' -Destination '$(BUILD_DIR)/$(RESOURCE_DIR)' -Recurse -Force"

# ensure that resources are compiled
$(BUILD_DIR)/app.res:
	windres app.rc -O coff -o $(BUILD_DIR)/app.res -Iinclude

# compile each source file to object file
$(BUILD_DIR)/%.o: src/%.cpp | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(WINFLAGS) -c $< -o $@

# link object files into executable
$(EXE): $(OBJ) $(BUILD_DIR)/app.res $(BUILD_DIR)/$(RESOURCE_DIR)
	$(CXX) $(OBJ) $(BUILD_DIR)/app.res $(CXXFLAGS) $(WINFLAGS) $(LIBFLAGS) -o $(BUILD_DIR)/$(EXE)

# remove build artifacts
clean:
	powershell -Command "if (Test-Path '$(BUILD_DIR)') { Remove-Item -Recurse -Force '$(BUILD_DIR)' }"

# run the executable
run: $(BUILD_DIR) $(EXE)
	./$(BUILD_DIR)/$(EXE)

.PHONY: all run clean