# Compiler
CC=gcc -Wall

# Globals-
SRC=src
BIN=bin
TESTS=test-dist

# CICD
clean:
	rm -rf $(BIN) $(TESTS) cmp copy codecA codecB encode decode stshell

# dist folder
$(BIN):
	mkdir $(BIN)

$(TESTS):
	rm -rf $(TESTS)
	mkdir $(TESTS)

# unit
$(BIN)/array.o: $(BIN) $(SRC)/array.c $(SRC)/array.h
	$(CC) -c "$(SRC)/array.c" -o $(BIN)/array.o

# applications
cmp: $(BIN) $(SRC)/cmp.c
	$(CC) -c "$(SRC)/cmp.c" -o $(BIN)/cmp.o
	$(CC) -o cmp $(BIN)/cmp.o

copy: $(BIN) $(SRC)/copy.c
	$(CC) -c "$(SRC)/copy.c" -o $(BIN)/copy.o
	$(CC) -o copy $(BIN)/copy.o

codecA: $(SRC)/codecA.c $(SRC)/codec.h
	$(CC) -c "$(SRC)/codecA.c" -o $(BIN)/codecA.o
	$(CC) -shared -fPIC -o codecA $(BIN)/codecA.o

codecB: $(SRC)/codecB.c $(SRC)/codec.h
	$(CC) -c "$(SRC)/codecB.c" -o $(BIN)/codecB.o
	$(CC) -shared -fPIC -o codecB $(BIN)/codecB.o

encode: $(SRC)/encode.c $(SRC)/codec.h
	$(CC) -c "$(SRC)/encode.c" -o $(BIN)/encode.o
	$(CC) -o encode $(BIN)/encode.o

decode: $(SRC)/decode.c $(SRC)/codec.h
	$(CC) -c "$(SRC)/decode.c" -o $(BIN)/decode.o
	$(CC) -o decode $(BIN)/decode.o

stshell: $(SRC)/stshell.c $(SRC)/stshell.h $(SRC)/array.h $(BIN)/array.o
	$(CC) -c "$(SRC)/stshell.c" -o $(BIN)/stshell.o
	$(CC) -o stshell $(BIN)/stshell.o $(BIN)/array.o

# build
all: cmp copy codecA codecB encode decode stshell

rebuild: clean all

# tests
test-1: rebuild $(TESTS)
	./stshell < ./test/stshell-simple.txt

test-2: rebuild $(TESTS)
	./stshell -v < ./test/stshell-simple.txt

test-3: rebuild $(TESTS)
	./stshell < ./test/stshell.txt

test-4: rebuild $(TESTS)
	./stshell -v < ./test/stshell.txt

.PHONY: test-dist
