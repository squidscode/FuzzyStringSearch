#pragma once

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <exception>
#include "FA.hpp"
#include <functional>
#include <iostream>
#include <iterator>

typedef long long ll;

/**
 * @brief 
 * 
 * @tparam N 
 */
template <typename N>
class dfa_node{
private:
    N name;
    bool accept_flag; // is this node an accept node?
public:
    dfa_node(N name, bool acc){
        this->name = name;
        this->accept_flag = acc;
    }

    dfa_node(){}

    bool is_accept() {
        return this->accept_flag;
    }

    void set_accept(bool acc){
        this->accept_flag = acc;
    }

    N get_name(){
        return this->name;
    }
};

namespace std {
    template <typename V> struct hash<std::pair<char, V> >
    {
        size_t operator()(const std::pair<char, V>& x) const
        {
            return (int)(x.first);
        }
    };
}



/**
 * @brief 
 * 
 * @tparam N 
 * @tparam V 
 */
template <typename N, typename V>
class DFA : public FA<N, V>{
protected:
    N start; bool start_flag{false};
    std::unordered_map< N, dfa_node<N> > name_map;
    std::unordered_map< N, std::unordered_map< V, N > > edge_map;

    void create_dfa(N node){
        if(!this->name_map.count(node)){ // create the node if it doesn't exist
            dfa_node<N> df = dfa_node<N>(node, false);
            this->name_map[node] = df;
        }
    }
public:
    DFA(){
        this->name_map = std::unordered_map< N, dfa_node<N> >();
        this->edge_map = std::unordered_map< N, std::unordered_map< V, N > >();
    }

    ~DFA(){}

    void add_start(N node) override {
        if(this->start_flag){
            throw std::runtime_error("Cannot set two starts!");
        }

        this->start = node;
        this->create_dfa(node);
        this->start_flag = true;
    }

    void add_transition(N node1, V val, N node2) override {
        this->create_dfa(node1);
        this->create_dfa(node2);

        this->edge_map[node1][val] = node2;
    }

    bool has_transition(N node, V val) override {
        return this->name_map.count(node) && this->edge_map[node].count(val);
    }

    void add_final_state(N node) override {
        if(!this->name_map.count(node))
            throw std::runtime_error("The node was not set!");
        this->name_map[node].set_accept(true);
    }

    std::unordered_set<N> vertices() override {
        std::unordered_set<N> ret = std::unordered_set<N>();
        for(auto vertex : this->name_map){
            ret.insert(vertex.first);
        }
        return ret;
    }

    std::unordered_set<std::pair<V, N> > edges(N node) override {
        if(!start_flag)
            throw std::runtime_error("Start not set!");
        if(!this->name_map.count(node))
            throw std::runtime_error("The node was not set!");
        if(!this->edge_map.count(node))
            return std::unordered_set<std::pair<V, N> >();
        std::unordered_set<std::pair<V, N> > ret = std::unordered_set<std::pair<V, N> >();
        for(auto e : this->edge_map[node]){
            ret.insert({e.first, e.second});
        }
        return ret;
    }

    N next_state(N node, V val) override {
        if(!this->name_map.count(node))
            throw std::runtime_error("The node was not set!");
        if(!this->edge_map[node].count(val))
            throw std::runtime_error("The edge does not exist!");
        return this->edge_map[node][val];
    }

    bool is_final(const N node) override {
        if(!this->name_map.count(node))
            throw std::runtime_error("The node was not set!");
        return this->name_map.at(node).is_accept();
    }

    N get_start() override {
        if(!start_flag){
            throw std::runtime_error("Start not set!");
        }
        return this->start;
    }

    template <class col>
    bool run(col c){
        return this->run(c.begin(), c.end());
    }

    template <class it>
    bool run(it begin, it end) {
        N st = this->start;
        while(begin != end){
            if(!this->has_transition(st, *begin)){
                return false;
            }
            st = this->next_state(st, *begin);
            ++begin;
        }
        return this->is_final(st);
    }

    DFA<ll, V> compress_dfa(){
        return this->compress_dfa(*this);
    }

    /**
     * @brief Given some DFA using some naming convention (of type N), convert the DFA 
     * to a DFA<long long, T> by encoding vertices/nodes as numbers.
     * 
     * @tparam N the type of the node names
     * @tparam T the type of the edge names
     * @param dfa the DFA we want to convert
     * @return DFA<ll, T> the encoded DFA.
     */
    template <typename NP, typename TP>
    static DFA<ll, TP> compress_dfa(DFA<NP,TP>& dfa) {
        DFA<ll, TP> comp_dfa = DFA<ll,TP>();
        ll vertex_id = 0;
        std::unordered_map<NP, ll> m = std::unordered_map<NP, ll>();
        for(NP vertex : dfa.vertices()){
            m[vertex] = vertex_id++;
        }
        for(NP vertex : dfa.vertices()){
            for(auto edge : dfa.edges(vertex)){
                comp_dfa.add_transition(m[vertex], edge.first, m[edge.second]);
            }
        }
        for(NP vertex : dfa.vertices()){
            if(dfa.is_final(vertex)) comp_dfa.add_final_state(m[vertex]);
        }
        comp_dfa.add_start(m[dfa.get_start()]);
        return comp_dfa;
    }

    friend std::ostream& operator<<(std::ostream& os, const DFA<N, V>& dt);
    friend std::istream& operator>>(std::istream& os, const DFA<N, V>& dt);
};

