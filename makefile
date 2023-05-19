CFLAGS = -Wall -Wextra -pedantic -Isrc

all: ./build ./build/tako ./build/takocc

./build/takocc: ./src/takocc.c
	$(CC) $(CFLAGS) -o $@ $^	

./build/tako: ./src/tako.c
	$(CC) $(CFLAGS) -o $@ $^

./build:
	mkdir $@