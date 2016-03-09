CC = clang 

default: main test

test: test.c bloomfilter.h bloomfilter.c
	$(CC) -fopenmp -O3 -o test -Wall test.c bloomfilter.c murmur3.c -lm

main: main.c bloomfilter.h bloomfilter.c
	$(CC) -O3 -o main -Wall main.c bloomfilter.c murmur3.c -lm

clean:
	rm main test
