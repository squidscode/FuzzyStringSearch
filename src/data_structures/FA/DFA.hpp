#pragma once

#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <exception>
#include "FA.hpp"
#include <functional>
#include <iostream>
#include <iterator>
#include <list>

typedef long long ll;

/**
 * @brief Represents a DFA node. Holds the name of the node, and whether the node represents an accept node. 
 * 
 * @tparam N the type of the name.
 */
template <typename N>
class dfa_node{
private:
    N name;
    bool accept_flag; // is this node an accept node?
public:
    /**
     * @brief Construct a new dfa node object
     * 
     * @param name the name of the node
     * @param acc whether the node is accepted.
     */
    dfa_node(N name, bool acc){
        this->name = name;
        this->accept_flag = acc;
    }

    /**
     * @brief Construct a new dfa node object
     * 
     */
    dfa_node(){}

    /**
     * @brief Whether the node accepts.
     * 
     * @return true if the node accepts.
     * @return false if the node rejects.
     */
    bool is_accept() {
        return this->accept_flag;
    }

    /**
     * @brief Set the accept object
     * 
     * @param acc whether the node accepts or rejects.
     */
    void set_accept(bool acc){
        this->accept_flag = acc;
    }

    /**
     * @brief Get the name object
     * 
     * @return N the name of the node.
     */
    N get_name(){
        return this->name;
    }
};

namespace std {
    /**
     * @brief A hash for a pair
     * 
     * @tparam T the type of the first parameter in the pair
     * @tparam V the type of the second parameter in the pair.
     */
    template <typename T, typename V> struct hash<std::pair<T, V> >
    {
        size_t operator()(const std::pair<T, V>& x) const
        {
            return (std::hash<T>{}(x.first) + 101) ^ std::hash<V>{}(x.second);
        }
    };
}

/**
 * @brief Represents a Deterministic-Finite-Automoton (DFA).
 * 
 * @tparam N the type of the name of the nodes.
 * @tparam V the type of the transitions.
 */
template <typename N, typename V>
class DFA : public FA<N, V>{
protected:
    N start; bool start_flag{false};
    std::unordered_map< N, dfa_node<N> > name_map;
    std::unordered_map< N, std::unordered_map< V, N > > edge_map;
    std::unordered_set<V> alphabet;

    void create_dfa(N node){
        if(!this->name_map.count(node)){ // create the node if it doesn't exist
            dfa_node<N> df = dfa_node<N>(node, false);
            this->name_map[node] = df;
        }
    }
public:
    /**
     * @brief Construct a new DFA object
     * 
     */
    DFA(){
        this->name_map = std::unordered_map< N, dfa_node<N> >();
        this->edge_map = std::unordered_map< N, std::unordered_map< V, N > >();
    }

    /**
     * @brief Destroy the DFA object
     * 
     */
    ~DFA(){}

    /**
     * @brief Adds a start state to the DFA.
     * 
     * @param node the name of the node. 
     */
    void add_start(N node) override {
        if(this->start_flag){
            throw std::runtime_error("Cannot set two starts!");
        }

        this->start = node;
        this->create_dfa(node);
        this->start_flag = true;
    }

    /**
     * @brief Adds a transition from the first node to the second node through the given edge. 
     * 
     * @param node1 the first node.
     * @param val the value that represents the state transition
     * @param node2 the second node.
     */
    void add_transition(N node1, V val, N node2) override {
        this->create_dfa(node1);
        this->create_dfa(node2);

        this->edge_map[node1][val] = node2;
        this->alphabet.insert(val);
    }

    /**
     * @brief Checks if the transition exists on the given node. 
     * 
     * @param node the name of the node.
     * @param val the value that represents the state transition.
     * @return true if the transition exists
     * @return false if the transition does not exist.
     */
    bool has_transition(N node, V val) override {
        return this->name_map.count(node) && this->edge_map[node].count(val);
    }


    V get_transition(N node1, N node2) {
        for(auto p : this->edge_map[node1]){
            if(p.second == node2) return p.first;
        }
        throw std::runtime_error("Transition not found!");
    }

    /**
     * @brief Adds a final state to this DFA. Note that the node must have already been 
     * added to the DFA. 
     * 
     * @param node the name of the node we wish to add the final state to.
     */
    void add_final_state(N node) override {
        if(!this->name_map.count(node))
            throw std::runtime_error("The node was not set!");
        this->name_map[node].set_accept(true);
    }

    /**
     * @brief Returns the states (also known as states) in the DFA.
     * 
     * @return std::unordered_set<N> a set of all of the states.
     */
    std::unordered_set<N> states() override {
        std::unordered_set<N> ret = std::unordered_set<N>();
        for(auto vertex : this->name_map){
            ret.insert(vertex.first);
        }
        return ret;
    }

    /**
     * @brief Returns the out transitions from the given state/node. 
     * 
     * @param node the name of the state/node 
     * @return std::unordered_set<std::pair<V, N>> the set of all of the transitions (represented as
     * a set of pairs of <transition, state>). 
     */
    std::unordered_set<std::pair<V, N> > transitions(N node) override {
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

    /**
     * @brief Returns the state after starting at the given state and taking the specified state transition.
     * This function errors if the node was not set, or if the edge does not exist.
     *  
     * @param node the name of the node.
     * @param val the transition
     * @return N the state after taking the transition
     */
    N next_state(N node, V val) override {
        if(!this->name_map.count(node))
            throw std::runtime_error("The node was not set!");
        if(!this->edge_map[node].count(val))
            throw std::runtime_error("The edge does not exist!");
        return this->edge_map[node][val];
    }

    /**
     * @brief Is the given node final? 
     * 
     * @param node the name of the node/state
     * @return true if the node is a final node.
     * @return false if the node is not a final node.
     */
    bool is_accept(const N node) override {
        if(!this->name_map.count(node))
            throw std::runtime_error("The node was not set!");
        return this->name_map.at(node).is_accept();
    }

    /**
     * @brief Get the start node.
     * 
     * @return N the start node.
     */
    N get_start() override {
        if(!start_flag){
            throw std::runtime_error("Start not set!");
        }
        return this->start;
    }

    /**
     * @brief Runs this DFA with the given collection.
     * 
     * @tparam Collection a collection type (must have `begin` and `end` methods that return 
     * iterators.)
     * @param c the collection with the `begin` and `end` methods
     * @return true if transitions result in an accept state. 
     * @return false if the transitions result in a reject state.
     */
    template <class Collection>
    bool run(Collection c){
        return this->run(c.begin(), c.end());
    }

    /**
     * @brief Runs the DFA with the given begin and end iterator. 
     * 
     * @tparam it the type of the iterator.
     * @param begin the begin iterator (ie. the start of the collection)
     * @param end the end iterator (ie. the end of the collection)
     * @return true if the DFA ends on an accept state.
     * @return false if the DFA ends on a reject state.
     */
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
        return this->is_accept(st);
    }

    /**
     * @brief Returns the intersection between this DFA and the other DFA. 
     * 
     * @param dfa the other DFA.
     * @return DFA<N,V> the intersection.
     */
    DFA<std::pair<N,N>,V> intersection(DFA<N,V>& dfa){
        return this->intersection(*this, dfa);
    }

    /**
     * @brief Computes the intersection between the two given DFAs.
     * 
     * @param dfa1 the first DFA
     * @param dfa2 the second DFA.
     * @return DFA<N, V> the intersection between the two DFAs.
     */
    static DFA<std::pair<N,N>, V> intersection(DFA<N,V>& dfa1, DFA<N,V>& dfa2){
        /**
         * The plan:
         * - run DFS on the first DFA; 
         * - if the transition exists, then add the pair of states to the 
         * stack
         */

        DFA<std::pair<N,N>,V> new_dfa;
        new_dfa.add_start({dfa1.get_start(),dfa2.get_start()});
        std::vector<std::pair<N, N> > stk{{dfa1.get_start(), dfa2.get_start()}};
        std::unordered_set<std::pair<N,N> > seen{};
        while(stk.size()){
            std::pair<N,N> next = stk.back(); stk.pop_back();
            if(seen.count(next)) continue;
            if(dfa1.is_accept(next.first) && dfa2.is_accept(next.second))
                new_dfa.add_final_state(next);
            seen.insert(next); // add the state to the seen states.
            for(auto pts : dfa1.transitions(next.first)){
                V transition = pts.first;
                if(dfa2.has_transition(next.second, transition)){
                    std::pair<N,N> next_state = {dfa1.next_state(next.first, transition), dfa2.next_state(next.second, transition)};
                    new_dfa.add_transition(next, transition, next_state);
                    if(dfa1.is_accept(next_state.first) && dfa2.is_accept(next_state.second)) new_dfa.add_final_state(next_state);
                    stk.push_back(next_state);
                }
            }
        }
        return new_dfa;
    }

    /**
     * @brief Returns all of the accept states. 
     * 
     * @return std::unordered_set<N> the accept states
     */
    std::unordered_set<N> accept_states(){
        std::unordered_set<N> accepts;
        for(N state : this->states()){
            if(this->is_accept(state)){
                accepts.insert(state);
            }
        }
        return accepts;
    }

    /**
     * @brief Run a simple DFS algorithm on this DFA to find some of the
     * accept paths (does not find cycle paths). Assume that the DFA is a DAG.
     * 
     * @return std::unordered_set<std::vector<V> > 
     */
    std::unordered_set<std::vector<V> > accept_paths(){
        std::unordered_set<std::vector<V> > paths;
        std::unordered_map<N, N> parent;
        for(N state : this->states()){
            for(auto pts : this->transitions(state)){
                parent[pts.second] = state;
            }
        }
        std::unordered_set<N> accepts = this->accept_states();
        for(N ac : accepts){
            N cur = ac;
            std::list<V> path;
            while(cur != this->get_start()){
                // std::cout << "Looking for " << parent[cur] << " -> " << cur;
                path.push_front(this->get_transition(parent[cur], cur));
                cur = parent[cur];
            }
            paths.insert(std::vector<V>(path.begin(), path.end()));
        }
        return paths;
    }

    /**
     * @brief Compresses this DFA into a new DFA with each node name replaced with 
     * a number (of type: long long).
     * 
     * @return DFA<ll, V> the compressed DFA.
     */
    DFA<ll, V> compress_dfa(){
        return this->compress_dfa(*this);
    }

    /**
     * @brief Given some DFA using some naming convention (of type N), convert the DFA 
     * to a DFA<long long, T> by encoding states/nodes as numbers.
     * 
     * @tparam N the type of the node names
     * @tparam T the type of the edge names
     * @param dfa the DFA we want to convert
     * @return DFA<long long, T> the encoded DFA.
     */
    template <typename NP, typename TP>
    static DFA<ll, TP> compress_dfa(DFA<NP,TP>& dfa) {
        DFA<ll, TP> comp_dfa = DFA<ll,TP>();
        ll vertex_id = 0;
        std::unordered_map<NP, ll> m = std::unordered_map<NP, ll>();
        for(NP vertex : dfa.states()){
            m[vertex] = vertex_id++;
        }
        for(NP vertex : dfa.states()){
            for(auto edge : dfa.transitions(vertex)){
                comp_dfa.add_transition(m[vertex], edge.first, m[edge.second]);
            }
        }
        for(NP vertex : dfa.states()){
            if(dfa.is_accept(vertex)) comp_dfa.add_final_state(m[vertex]);
        }
        comp_dfa.add_start(m[dfa.get_start()]);
        return comp_dfa;
    }

    /**
     * @brief Get the alphabet set.
     * 
     * @return std::unordered_set<char> 
     */
    std::unordered_set<char> get_alphabet(){
        return this->alphabet;
    }

    friend std::ostream& operator<<(std::ostream& os, const DFA<N, V>& dt);
    friend std::istream& operator>>(std::istream& os, const DFA<N, V>& dt);

    friend std::ostream& serialize(std::ostream& os, const DFA<N, V>& dt);
    friend std::istream& deserialize(std::istream& os, const DFA<N, V>& dt);
};

// Standard output for compressed DFA:
std::ostream& operator<<(std::ostream& out, const DFA<long long, char>& dfa) {
    for(auto df : dfa.name_map){
        out << "(" << df.first << ", " << (df.second.is_accept() ? "ACCEPT" : "REJECT") << ") ";
    }
    out << "\n";
    for(auto v : dfa.edge_map){
        out << (v.first == dfa.start ? "*** " : "") << v.first << ": ";
        for(auto e : v.second){
            out << "(" << e.first << ", " << e.second << ") ";
        }
        out << "\n";
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const DFA<std::string, char>& dfa) {
    for(auto df : dfa.name_map){
        out << "(" << df.first << ", " << df.second.is_accept() << ") ";
    }
    out << "\n";
    for(auto v : dfa.edge_map){
        out << (v.first == dfa.start ? "*** " : "") << v.first << ": ";
        for(auto e : v.second){
            out << "(" << e.first << ", " << e.second << ") ";
        }
        out << "\n";
    }
    return out;
}
