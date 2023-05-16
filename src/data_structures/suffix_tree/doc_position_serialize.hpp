#pragma once

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <fstream>
#include <list>
#include "suffix_tree.hpp"
#include "../FA/encoding_util.hpp"

// Change this code if you want to decrease the size of your serialized suffix tree file.
std::ostream& serialize_doc_position(std::ostream& os, doc_position_t& pos) {
  os.write((char*) &pos.index, sizeof(ll));
  // os.write((char*) &pos.line, sizeof(ll));
  // os.write((char*) &pos.column, sizeof(ll));
  return os;
}

std::istream& deserialize_doc_position(std::istream& is, doc_position_t& pos) {
  is.read((char*) &pos.index, sizeof(ll));
  // is.read((char*) &pos.line, sizeof(ll));
  // is.read((char*) &pos.column, sizeof(ll));
  return is;
}