CFLAGS = -Wall -Wextra -pedantic -Isrc -Iinclude -g

all: ./build/tako ./build/ramen

./build/ramen: ./src/common.c ./src/ramen_lexer.c ./src/ramen_parser.c ./src/ramen.c
	$(CC) $(CFLAGS) -o $@ $^

./build/tako: ./src/common.c ./src/tako.c
	$(CC) $(CFLAGS) -o $@ $^