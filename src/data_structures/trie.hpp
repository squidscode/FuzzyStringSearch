#pragma once

#include "FA/DFA.hpp"
#include <unordered_set>
#include <string>

/**
 * @brief A prefix tree, also known as a trie, is a tree that stores the prefixes of the inserted items. It can
 * check if the word is in the prefix tree in O(n) time, where n is the # of letters in the search word.
 * 
 */
class trie : protected DFA<std::string, char> {
public:
    /**
     * @brief Construct a new trie object
     * 
     */
    trie() : DFA<std::string, char>{} {
        this->add_start("");
    }

    /**
     * @brief Inserts the string into the trie.
     * 
     * @param s the string we wish to insert.
     */
    void insert(std::string s){
        std::string acc = "";
        for(char c : s){
            this->add_transition(acc, c, acc + c);
            acc += c;
        }
        this->add_final_state(acc);
    }

    /**
     * @brief Does this trie contain the given string?
     * 
     * @param s the string
     * @return true if the string is contained in the trie
     * @return false if the string is not contained in the trie.
     */
    bool contains(std::string s){
        return this->run(s);
    }

    /**
     * @brief Cast convertion to a DFA.
     * 
     * @return DFA<std::string, char> the DFA of the given trie.
     */
    DFA<std::string, char> operator()(){
        return *this;
    }
};