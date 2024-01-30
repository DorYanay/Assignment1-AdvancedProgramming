# Globals-
SRC=src
BIN=bin

# Compiler
CXX=g++

# CI/CD
clean:
	rm -rf $(BIN) myshell

# dist folders
$(BIN):
	mkdir $(BIN)

# units
$(BIN)/array.o: $(SRC)/array.cpp
	g++ -fPIC -c $(SRC)/array.cpp -o $(BIN)/array.o

$(BIN)/myshell.o: $(SRC)/myshell.cpp
	g++ -fPIC -c $(SRC)/myshell.cpp -o $(BIN)/myshell.o

# applications
myshell: $(BIN) $(BIN)/myshell.o $(BIN)/array.o
	g++ -fPIC -o myshell $(BIN)/myshell.o $(BIN)/array.o

# build
all: myshell

rebuild: clean all

# tests
test-1: rebuild
	./myshell
