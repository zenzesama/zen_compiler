CC = gcc
CFLAGS = -Wall -Wextra -Wswitch
SRC = main.c lexer.c parser.c ast.c codegen.c debug.c
BIN = zenc


$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $(BIN) $(SRC)

clean:
	rm -f $(BIN) out.ll program

.PHONY: clean
