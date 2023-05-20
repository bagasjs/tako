CFLAGS = -Wall -Wextra -pedantic -Isrc -g

all: ./build ./build/tako ./build/takocc ./example/variable.tako

./build/takocc: ./src/takocc.c
	$(CC) $(CFLAGS) -o $@ $^	

./build/tako: ./src/takort.c
	$(CC) $(CFLAGS) -o $@ $^

./example/variable.tako: ./example/variable.tc
	./build/takocc $@ $^

./build:
	mkdir $@

./example:
	mkdir $@