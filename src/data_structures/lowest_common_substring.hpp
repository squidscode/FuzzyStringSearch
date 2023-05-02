#pragma once

#include "FA/NFA.hpp"
#include <string>

class lcs_nfa : public NFA<std::pair<std::string, int>, char>{
public:
  /**
   * @brief Construct a new levenshtein_nfa object
   * 
   * @param s the string we wish to search.
   * @param error the allowed deletes/insertions/substitutions.
   */
  lcs_nfa(std::string s, int error){
    std::string acc = "";
    this->add_start({acc,0});
    for(int i = 0; i < error; ++i){
      acc = "";
      for(char c : s){
        this->add_transition({acc,i}, c, {acc + c,i});
        this->add_transition({acc,i}, nfa_val<char>::EPSILON, {acc+c,i+1}); // delete
        this->add_transition({acc,i}, nfa_val<char>::STAR, {acc,i+1}); // insertion
        this->add_transition({acc,i}, nfa_val<char>::STAR, {acc+c,i+1}); // subst
        acc += c;
      }
      this->add_transition({acc,i}, nfa_val<char>::STAR, {acc,i+1}); // insertion (at the end of the word)
    }
    acc = "";
    for(char c : s){
      this->add_transition({acc,error}, c, {acc + c,error});
      acc += c;
    }
    for(int i = 0; i <= error; ++i){
      this->add_final_state({s, i});
    }
  }

  NFA<std::pair<std::string, int>, char> operator()(){
    return *this;
  }
};