CC = g++
CFLAGS = -std=c++11 -pedantic -Wall

all: huffman

huffman: main.o huffman.o
	$(CC) $(CFLAGS) main.o huffman.o -o huffman

main.o: main.cpp
	$(CC) -c main.cpp

huffman.o: huffman.cpp
	$(CC) -c huffman.cpp

clean:
	rm -rf *.o huffman
	