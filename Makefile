CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude

SRC = src/core/Memory.cpp src/core/CPU.cpp src/core/OS.cpp src/ui/main.cpp
BIN = build/kaal_os

all: $(BIN)

$(BIN):
	$(CXX) $(CXXFLAGS) $(SRC) -o $(BIN)

clean:
	rm -f $(BIN) build/output.txt

run: all
	cd build && ../$(BIN)
