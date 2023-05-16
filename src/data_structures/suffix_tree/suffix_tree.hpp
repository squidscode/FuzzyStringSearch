#pragma once

#include "../trie.hpp"
#include <string>
#include <list>
#include <unordered_map>
#include <stdio.h>

#define min(x, y)       x < y ? x : y

typedef struct _position_t_ {
    ll index;
    ll line;
    ll column;
} doc_position_t;

bool operator==(const doc_position_t& p1, const doc_position_t& p2) {
    return (p1.index == p2.index);
}

namespace std {
    template <> struct hash<doc_position_t>
    {
        size_t operator()(const doc_position_t& x) const
        {
            return x.index;
        }
    };
}

class compressed_suffix_tree : public DFA<ll, char> {
protected:
    std::unordered_multimap<ll, doc_position_t> position_map;
public:
    compressed_suffix_tree() : DFA<ll,char>{} {}

    compressed_suffix_tree(DFA<std::string, char>& trie, std::unordered_multimap<std::string, doc_position_t>& positions) : DFA<ll,char>{trie.compress_dfa()} {
        for(auto s : positions){
            ll ind;
            position_map.insert({(ind = this->follow(s.first)), s.second});
        }
    }

    std::unordered_set<ll> get_indices(std::string s){
        ll ind = this->follow(s);
        auto range = this->position_map.equal_range(ind);
        std::unordered_set<ll> ret;
        for(auto it = range.first; it != range.second; ++it){
            ret.insert(it->second.index);
        }
        return ret;
    }

    std::unordered_set<std::pair<ll,ll> > get_lc(std::string s){
        ll ind = this->follow(s);
        auto range = this->position_map.equal_range(ind);
        std::unordered_set<std::pair<ll,ll> > ret;
        for(auto it = range.first; it != range.second; ++it){
            ret.insert({it->second.line, it->second.column});
        }
        return ret;
    }

    friend std::ostream& serialize_suffix_tree(std::ostream& os, compressed_suffix_tree& dt);
    friend std::istream& deserialize_suffix_tree(std::istream& is, compressed_suffix_tree& dt);
};


class suffix_tree : public trie {
protected:
    std::unordered_multimap<std::string, doc_position_t> position_map;
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
            // add all chunks
            while(!feof(f)){
                char c = fgetc(f);
                l.push_back(c);
                if(l.size() == max_suffix){
                    std::string s(l.begin(), l.end());
                    if(app_insert) this->insert(s);
                    this->position_map.insert({s,{ind, line, col}});
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
        auto range = this->position_map.equal_range(s);
        std::unordered_set<ll> ret;
        for(auto it = range.first; it != range.second; ++it){
            ret.insert(it->second.index);
        }
        return ret;
    }

    std::unordered_set<std::pair<ll,ll> > get_lc(std::string s){
        auto range = this->position_map.equal_range(s);
        std::unordered_set<std::pair<ll,ll> > ret;
        for(auto it = range.first; it != range.second; ++it){
            ret.insert({it->second.line, it->second.column});
        }
        return ret;
    }

    std::unordered_set<doc_position_t> get_doc_positions(std::string s){
        auto range = this->position_map.equal_range(s);
        auto ret = std::unordered_set<doc_position_t>();
        for(auto it = range.first; it != range.second; ++it){
            ret.insert(it->second);
        }
        return ret;
    }

    compressed_suffix_tree compress_dfa(){
        return this->compress_dfa(*this);
    }

    static compressed_suffix_tree compress_dfa(suffix_tree& st) {
        return compressed_suffix_tree(st, st.position_map);
    }
};


