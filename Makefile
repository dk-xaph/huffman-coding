CC = g++
CFLAGS = -std=c99

all: huffman

huffman: main.o huffman.o
	$(CC) -Wall -std=c++11 main.o huffman.o -o huffman

main.o: main.cpp
	$(CC) -std=c++11 -c main.cpp

huffman.o: huffman.cpp
	$(CC) -std=c++11 -c huffman.cpp

clean:
	rm -rf *.o huffman
	