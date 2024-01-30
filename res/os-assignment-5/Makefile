# Globals-
SRC=src
BIN=bin

# Compiler
CXX=g++

# CI/CD
clean:
	rm -rf $(BIN) st_pipelie

# dist folders
$(BIN):
	mkdir $(BIN)

# units
$(BIN)/active_object.o: $(SRC)/active_object.cpp
	g++ -fPIC -c $(SRC)/active_object.cpp -o $(BIN)/active_object.o

$(BIN)/math.o: $(SRC)/math.cpp
	g++ -fPIC -c $(SRC)/math.cpp -o $(BIN)/math.o

$(BIN)/Queue.o: $(SRC)/Queue.cpp
	g++ -fPIC -c $(SRC)/Queue.cpp -o $(BIN)/Queue.o

$(BIN)/st_pipelie.o: $(SRC)/st_pipelie.cpp
	g++ -fPIC -c $(SRC)/st_pipelie.cpp -o $(BIN)/st_pipelie.o

# applications
reactor.os: $(BIN) $(BIN)/reactor.o
	g++ -fPIC -shared -o reactor.os $(BIN)/reactor.o

st_pipelie: $(BIN) $(BIN)/st_pipelie.o $(BIN)/active_object.o $(BIN)/Queue.o $(BIN)/math.o
	g++ -fPIC -o st_pipelie $(BIN)/st_pipelie.o $(BIN)/active_object.o $(BIN)/Queue.o $(BIN)/math.o

# build
all: st_pipelie

rebuild: clean all

# tests
test-1: rebuild
	./st_pipelie 2

test-2: rebuild
	./st_pipelie 10

test-3: rebuild
	./st_pipelie 10 1111

test-4: rebuild
	./st_pipelie 1000 2222

test-5: rebuild
	./st_pipelie 100000 3333
