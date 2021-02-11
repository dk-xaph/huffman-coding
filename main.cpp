#include <cstring>
#include <exception>
#include <fstream>
#include <iostream>
#include <string>

#include "huffman.hpp"

using namespace std;

int main(int argc, char *argv[]) {
  try {
    if (argc != 6) {
      throw new Huffman::MyException("Incorrect number of command.");
    }
    if (strcmp(argv[1], "-u") != 0 && strcmp(argv[1], "-c") != 0) {
      throw new Huffman::MyException(
          "Unidentified command.\nUse -c for compress and -u for uncompress.");
    }

    Huffman a;
    string infile;
    string outfile;
	
    if (strcmp(argv[2], "-f") == 0 && strcmp(argv[4], "-o") == 0) {
      infile = argv[3];
      outfile = argv[5];
    } else if (strcmp(argv[2], "-o") == 0 && strcmp(argv[4], "-f") == 0) {
      outfile = argv[3];
      infile = argv[5];
    } else {
      throw new Huffman::MyException(
          "Unidentified command.\nUse -f for input file and -o for output "
          "file.");
    }
    if (strcmp(argv[1], "-c") == 0) {
      a.archiving(infile, outfile);
    } else {
      a.decompression(infile, outfile);
    }
  } catch (const exception *e) {
    cerr << e->what();
    return 1;
  }

  return 0;
}