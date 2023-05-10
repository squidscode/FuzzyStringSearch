#pragma once

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <exception>
#include "FA.hpp"
#include <functional>
#include <iostream>
#include <iterator>
#include <fstream>
#include <list>
#include "DFA.hpp"

// The types of state.
enum ENCODING_VERSION:char {V1_1=2, NORMALIZED=1};
enum STATE_TYPE:char {reject=0b0, accept=0b1, start=0b10, end_read=0b100};
const ll eos = LONG_MAX;

// Serialize and deserialize for compressed DFAs 
template <class V>
std::ostream& serialize(std::ostream& os, DFA<long long, V>& dt){
    /**
     * The idea:
     *  - We want to run a BFS on the DFA until we see each one of the states.
     *  - Any states we do not see in the BFS are ignored because ~we cannot get to these
     *  states!
     *  - A byte at the beginning of the file indicates what the version of the file is.
     *  - A byte at the beginning of the state adj list indicates whether the state is
     *  a normal state (aka. a reject state), a start state, or an accept state.
     */
    os.put(ENCODING_VERSION::V1_1);   // write the current encoding version
    ll start = dt.get_start(); // eos is the end of state adj list.
    std::list<ll> q{dt.get_start()};
    std::unordered_set<ll> seen;
    bool first{true};
    while(q.size()){
        first = false;
        ll cur = q.front(); q.pop_front();
        if(seen.count(cur)) continue;
        seen.insert(cur);
        os.put((cur == start ? STATE_TYPE::start : 0) + (dt.is_accept(cur) ? STATE_TYPE::accept : STATE_TYPE::reject));
        os.write((char*)&cur, sizeof(ll));
        for(std::pair<V,ll> p : dt.transitions(cur)){
            os.write((char*)&p.second, sizeof(ll));
            os.write((char*)&p.first, sizeof(V));
            q.push_back(p.second);
        }
        os.write((char*)&eos, sizeof(ll));
    }
    os.put(STATE_TYPE::end_read); // write end_read
    return os;
}

// Deserialize using the above convention.
template <class V>
std::istream& deserialize(std::istream& is, DFA<long long, V>& dt){
    dt = DFA<ll, V>(); // create a new DFA.
    char st;           // the state_type
    assert((is.get(st), st) & ENCODING_VERSION::V1_1);   // we only support encoding version 1.1
    while(is.get(st) && st != STATE_TYPE::end_read){ // while we can get a state_type character and it is not end_read
        ll v, t; is.read((char*) &v, sizeof(ll));
        if(st & STATE_TYPE::start) dt.add_start(v);
        while((is.read((char*) &t, sizeof(ll)), t != eos)){
            if(is.eof() || !is.good()) break;
            V trans; is.read((char*)&trans, sizeof(V));
            dt.add_transition(v, trans, t);
        }
        if(st & STATE_TYPE::accept) dt.add_final_state(v);
        if(is.eof() || !is.good()) break;
    }
    return is;
}

/**
 * @brief "Normalizes" the serialized DFA at the given location. This function is strongly coupled with the 
 * serialize and deserialize functions, and, if a new serialze protocol is used to encode DFAs, then this function
 * must be changed as well.
 * 
 * @tparam V the type of the DFA transition
 * @param fs the fstream of the file, assume that the fstream has been correctly positioned at the beginning of the
 * serialized DFA.
 * @return the fstream.
 */
template <class V>
std::fstream& normalize(std::fstream& fs) {
  std::unordered_map<ll, ll> position_map;
  char ch;
  ll beginning = fs.tellg();
  ll pos = beginning;
  fs.get(ch);
  assert(ch & ENCODING_VERSION::V1_1); // only works with V1.1
  while(fs.get(ch)) { // the type of the node
    ll v, t; fs.read((char*) &v, sizeof(ll));
    position_map[v] = pos;
    while((fs.read((char*) &t, sizeof(ll)), t != eos)){
      if(fs.eof() || !fs.good()) break;
      V trans; fs.read((char*)&trans, sizeof(V));
    }
    pos = fs.tellg();
  }

  // fill in the holes
  for(auto position_pair : position_map){
    fs.seekg(position_pair.second);
    fs.get(ch);
    fs.write((char*) &position_pair.second, sizeof(ll));
  }

  fs.seekg(beginning);    // move to beginning
  fs.put(ENCODING_VERSION::V1_1 | ENCODING_VERSION::NORMALIZED); // indicate that we have normalized the file

  return fs;
}

