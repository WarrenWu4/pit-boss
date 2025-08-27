CXX = g++
CXXFLAGS = -Iinclude
WINFLAGS = -mwindows -municode
LIBFLAGS = -lsetupapi
SRC = src/*.cpp
EXE = build/pitboss.exe

all: $(EXE)

$(MKDIR_P):
	mkdir -p build

$(EXE): $(MKDIR_P) $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(WINFLAGS) $(LIBFLAGS) -o $(EXE)

clean:
	if exist $(EXE) del /Q $(EXE)

run: $(EXE)
	./$(EXE)