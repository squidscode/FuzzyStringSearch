#pragma once

#include "trie.hpp"
#include <string>
#include <list>
#include <unordered_map>
#include <stdio.h>

#define min(x, y)       x < y ? x : y

// TODO: This implementation of suffix tree is incredibly slow.
class suffix_tree : public trie {
private:
    std::unordered_multimap<std::string, ll> index_map;
public:
        
    template<class col>
    suffix_tree(std::string path, ll max_suffix) : trie{} {
        FILE *f = fopen(path.c_str(), "r");
        if(f == NULL){
            throw std::runtime_error("Cannot open file!");
        }else{
            std::list<char> l;
            ll ind = 0;
            while(!feof(f)){
                l.push_back(fgetc(f));
                if(l.size() == max_suffix){
                    std::string s(l.begin(), l.end());
                    this->insert(s);
                    this->index_map({s,ind});
                    l.pop_front();
                }
                ind++;
            }
        }
    }
};