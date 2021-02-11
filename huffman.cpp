#include "huffman.hpp"

#include <ctime>
#include <fstream>
#include <iostream>
#include <queue>
#include <string>

using namespace std;
namespace {
size_t const num_chars = 256;
size_t const num_bits = 8;
}

void Huffman::archiving(string const &infile, string const &outfile) {
  ifstream in(infile, ifstream::binary);

  if (!in.is_open()) {
    throw new Huffman::MyException("Input file not found.");
  }
  ofstream out(outfile, ofstream::binary);

  assign();

  make_table_of_frequencies(in);

  Node *root = make_tree();

  string str;
  walk_tree(root, str);

  print_code(in, out);

  delete[] table_of_frequencies;
}

void Huffman::make_table_of_frequencies(ifstream &in) {
  in.clear();
  in.seekg(0, in.beg);
  char s = 0;
  size_t num_char_in_text = 0;
  while (in.read((char *)&s, 1)) {
    table_of_frequencies[(unsigned char)s]++;
    ++num_char_in_text;
  }
  cout << num_char_in_text << endl;
}

void Huffman::walk_tree(Node *char_node, const string &str) {
  if (char_node->left == NULL && char_node->right == NULL) {
    if (str == "") {
      final_code[(unsigned char)char_node->s] = "0";
      bits_in_code += table_of_frequencies[(unsigned char)char_node->s];
    } else {
      final_code[(unsigned char)char_node->s] = str;
      bits_in_code +=
          str.size() * table_of_frequencies[(unsigned char)char_node->s];
    }
  } else {
    walk_tree(char_node->left, str + '0');
    walk_tree(char_node->right, str + '1');
  }
}

void Huffman::print_code(ifstream &in, ofstream &out) {
  string buffer;
  in.clear();
  in.seekg(0, in.beg);

  char s;

  buffer.reserve(bits_in_code);
  while (in.read((char *)&s, 1)) {
    buffer += final_code[(unsigned char)s];
  }

  cout << bits_in_code / 8 + (bits_in_code % 8 != 0) << endl;

  size_t sub_data = 0;

  if (bits_in_code != 0) {
    out.write((char *)&bits_in_code, sizeof bits_in_code);
    sub_data += 4;
  }
  if (char_in_txt != 0) {
    out.write((char *)&char_in_txt, sizeof char_in_txt);
    sub_data += 4;
  }

  for (size_t i = 0; i < num_chars; ++i) {
    if (table_of_frequencies[i] != 0) {
      out.write((char *)&i, 1);
      out.write((char *)&table_of_frequencies[i], 4);
      sub_data += 5;
    }
  }

  cout << sub_data;
  unsigned char c = 0;
  int num = 1;
  for (int i = 0; i < bits_in_code; ++i) {
    c += buffer[i] - '0';
    if (num == num_bits) {
      out.write((char *)&c, 1);
      num = 1;
      c = 0;
    } else {
      c = c << 1;
      ++num;
    }
  }
  if (num != 1) {
    c = c << (num_bits - num);
    out.write((char *)&c, sizeof(c));
  }
}

Huffman::Node *Huffman::make_tree() {
  priority_queue<Node *, vector<Node *>, comp> node_queue;

  for (size_t i = 0; i < num_chars; ++i) {
    if (table_of_frequencies[i] != 0) {
      ++char_in_txt;
      Node *char_node = new Node();
      char_node->freq = table_of_frequencies[i];
      char_node->s = (unsigned char)i;
      node_queue.push(char_node);
    }
  }

  while (node_queue.size() > 1) {
    Node *first = node_queue.top();
    node_queue.pop();
    Node *second = node_queue.top();
    node_queue.pop();

    Node *dad = new Node();
    dad->left = first;
    dad->right = second;
    dad->freq = first->freq + second->freq;
    node_queue.push(dad);
  }

  Node *head;
  if (!node_queue.empty()) {
    head = node_queue.top();
    node_queue.pop();
  } else {
    head = new Node();
  }
  return head;
}

void Huffman::decompression(string const &infile, string const &outfile) {
  ifstream in(infile, ifstream::binary);
  if (!in.is_open()) {
    throw new Huffman::MyException("Input file not found.");
  }
  ofstream out(outfile, ofstream::binary);
  assign();

  in.read((char *)&bits_in_code, 4);
  in.read((char *)&char_in_txt, 4);

  size_t sub_data = 0;
  sub_data += (bits_in_code != 0) ? 4 : 0;
  sub_data += (char_in_txt != 0) ? 4 : 0;

  read_table_of_frequencies(in, sub_data);
  Node *root = make_tree();

  string buffer;
  read_code(in, buffer);

  print_txt(out, buffer, root);

  cout << sub_data;
  delete[] table_of_frequencies;
}

void Huffman::read_table_of_frequencies(ifstream &in, size_t &sub_data) {
  for (size_t i = 0; i < (size_t)char_in_txt; ++i) {
    char s = 0;
    int num = 0;
    in.read((char *)&s, 1);
    in.read((char *)&num, 4);
    table_of_frequencies[(unsigned char)s] = num;
    sub_data += 5;
  }
}

void Huffman::read_code(ifstream &in, string &str) {
  size_t bytes_in_code = bits_in_code / num_bits;
  size_t residue_bits = bits_in_code % num_bits;

  str.resize(bits_in_code);
  size_t it = 0;

  cout << bits_in_code / 8 + (bits_in_code % 8 != 0) << endl;

  char *s = new char[bytes_in_code + (residue_bits != 0)];
  in.read(s, bytes_in_code + (residue_bits != 0));

  for (size_t i = 0; i < bytes_in_code; ++i) {
    unsigned char check = 128;
    for (int j = 0; j < num_bits; j++) {
      if (check & s[i]) {
        str[it] = '1';
      } else {
        str[it] = '0';
      }
      ++it;
      check = check >> 1;
    }
  }
  if (residue_bits != 0) {
    unsigned char check = 128;
    for (size_t i = 0; i < residue_bits; i++) {
      if (check & s[bytes_in_code]) {
        str[it] = '1';
      } else {
        str[it] = '0';
      }
      ++it;
      check = check >> 1;
    }
  }
  delete[] s;
}

void Huffman::print_txt(ofstream &out, string &str, Node *root) {
  Node *char_node = new Node();
  char_node = root;
  size_t i = 0;
  if (char_node->left == NULL && char_node->right == NULL) {
    for (size_t i = 0; i < (size_t)bits_in_code; ++i)
      out.write((char *)&char_node->s, 1);
    cout << bits_in_code << endl;
  } else {
    size_t num_char_in_text = 0;

    while (i <= (size_t)bits_in_code) {
      if (char_node->left == NULL && char_node->right == NULL) {
        out.write((char *)&char_node->s, 1);
        num_char_in_text++;
        char_node = root;
      } else {
        if (str[i] == '0') {
          char_node = char_node->left;
        } else {
          char_node = char_node->right;
        }
        ++i;
      }
    }
    cout << num_char_in_text << endl;
  }
}

void Huffman::assign() {
  table_of_frequencies = new int[num_chars];
  char_in_txt = 0;
  bits_in_code = 0;

  for (size_t i = 0; i < num_chars; ++i) {
    table_of_frequencies[i] = 0;
  }
}

Huffman::Node::~Node() {
  if (this->left != NULL) {
    delete this->left;
  }
  if (this->right != NULL) {
    delete this->right;
  }
  delete this;
}

void Huffman::check_file(string const &infile, string const &outfile) {
  ifstream in(infile, ifstream::binary);
  if (!in.is_open()) {
    throw new Huffman::MyException("Input file not found.");
  }
  ofstream out(outfile, ofstream::binary);
  if (!out.is_open()) {
    throw new Huffman::MyException("Input file not found.");
  }
};