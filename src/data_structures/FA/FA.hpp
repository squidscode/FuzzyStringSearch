#pragma once

#include <unordered_set>
#include <vector>
#include <iterator>

template <typename N, typename V>
class FA{
public:
    virtual void add_start(N node) = 0;
    virtual void add_transition(N node1, V val, N node2) = 0;
    virtual bool has_transition(N node, V val) = 0;
    virtual void add_final_state(N node) = 0;
    virtual std::unordered_set<N> vertices() = 0;
    virtual std::unordered_set<std::pair<V, N> > edges(N node) = 0;
    virtual N next_state(N node, V val) = 0;
    virtual bool is_final(N node) = 0;
    virtual N get_start() = 0;
};