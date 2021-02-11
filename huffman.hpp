#pragma once
#include <fstream>
#include <queue>
#include <string>
#include <vector>

class Huffman {
 public:
  void archiving(std::string const &infile, std::string const &outfile);
  void decompression(std::string const &infile, std::string const &outfile);

  class MyException : public std::exception {
    const char *exc_txt;

   public:
    MyException(const char *str) : exc_txt(str){};
    virtual const char *what() const throw() { return exc_txt; };
  };
  void check_file(std::string const &infile, std::string const &outfile);

 private:
  int bits_in_code;
  int char_in_txt;
  int *table_of_frequencies;

  std::string final_code[256];

  struct Node {
    explicit Node() : freq(0), s(0), left(NULL), right(NULL){};
    int freq;
    char s;
    Node *left;
    Node *right;
    ~Node();
  };

  struct comp {
    bool operator()(const Node *x, const Node *y) const {
      return x->freq > y->freq;
    }
  };

  void assign();

  void make_table_of_frequencies(std::ifstream &in);
  void walk_tree(Node *head, const std::string &str);
  void print_code(std::ifstream &in, std::ofstream &out);

  Node *make_tree();

  void read_table_of_frequencies(std::ifstream &, size_t &sub_data);
  void read_code(std::ifstream &in, std::string &str);
  void print_txt(std::ofstream &out, std::string &str, Node *root);
};