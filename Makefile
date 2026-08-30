CC = gcc
CFLAGS = -Wall -Wextra
SRC = main.c lexer.c parser.c ast.c codegen.c
BIN = zenc


$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -f $(BIN) out.ll program

.PHONY: clean
