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

std::ostream& serialize_doc_position(std::ostream& os, doc_position_t& pos);
std::istream& deserialize_doc_position(std::istream& is, doc_position_t& pos);

// Serialize and deserialize for compressed DFAs
std::ostream& serialize_suffix_tree(std::ostream& os, compressed_suffix_tree& dt){
  serialize<char>(os, dt);
  for(auto p : dt.position_map) {
    os.write((char*) &p.first, sizeof(ll));
    serialize_doc_position(os, p.second);
  }
  printf("FINISHED!\n");
  os.write((char*) &eos, sizeof(ll));
  return os;
}

// Deserialize using the above convention.
std::istream& deserialize_suffix_tree(std::istream& is, compressed_suffix_tree& dt){
  deserialize<char>(is, dt);
  ll state_num; doc_position_t pos;
  while(is.good() && is.read((char*) &state_num, sizeof(ll)) && state_num != eos) {
    deserialize_doc_position(is, pos);
    dt.position_map.insert({state_num, pos});
  }
  return is;
}