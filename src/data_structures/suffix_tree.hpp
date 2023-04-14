#pragma once

#include "trie.hpp"
#include <string>

// TODO: This implementation of suffix tree is incredibly slow.
class suffix_tree : public trie {
public:
    /**
     * @brief Construct a new suffix tree object
     * 
     * @param document the document.
     */
    suffix_tree(std::string document) : trie{} {
        ll sz = document.size();
        for(ll i = sz-1; i >= 0; --i){
            this->insert(document.substr(i, sz - i));
        }
    }
};