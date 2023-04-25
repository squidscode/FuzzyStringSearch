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
    std::unordered_multimap<std::string, std::pair<ll,ll> > line_col_map;
public:
    suffix_tree() : trie{} {}

    void load_file(std::string path, ll max_suffix, bool app_insert=true) {
        FILE *f = fopen(path.c_str(), "r");
        if(f == NULL){
            throw std::runtime_error("Cannot open file!");
        }else{ 
            std::list<char> l;
            ll ind = 0;
            ll line = 1;
            ll col = 1;
            while(!feof(f)){
                char c = fgetc(f);
                l.push_back(c);
                if(l.size() == max_suffix){
                    std::string s(l.begin(), l.end());
                    if(app_insert) this->insert(s);
                    this->index_map.insert({s,ind});
                    this->line_col_map.insert({s, {line,col}});
                    this->alphabet.insert(c);
                    l.front() == '\n' ? col = 1 : col++;
                    l.front() == '\n' ? line++ : line;
                    l.pop_front();
                }
                ind++;
            }
        }
    }

    std::unordered_set<ll> get_indices(std::string s){
        auto range = this->index_map.equal_range(s);
        std::unordered_set<ll> ret;
        for(auto it = range.first; it != range.second; ++it){
            ret.insert(it->second);
        }
        return ret;
    }

    std::unordered_set<std::pair<ll,ll> > get_lc(std::string s){
        auto range = this->line_col_map.equal_range(s);
        std::unordered_set<std::pair<ll,ll> > ret;
        for(auto it = range.first; it != range.second; ++it){
            ret.insert(it->second);
        }
        return ret;
    }
};