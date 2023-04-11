#pragma once

#include "DFA.hpp"
#include <vector>

template <typename T>
class nfa_val{
public:
    enum __nfa_sp__ {NONE, EP, ALL};
    __nfa_sp__ nfa_flag;
    T val;

    nfa_val(__nfa_sp__ flag){
        this->nfa_flag = flag;
    }

    nfa_val(T val){
        this->nfa_flag = NONE;
        this->val = val;
    }

    const T& operator=(const T& val) {
        this->val = val;
        this->nfa_flag = NONE;
        return val;
    }

    operator T() const {
        return val;
    }

    bool operator==(const nfa_val<T>& other) const {
        return false;
    }
};

namespace std {
    template <typename V> struct hash<nfa_val<V> > {
        size_t operator()(const nfa_val<V>& x) const
        {
            return (unsigned long) &x;
        }
    };

    template <typename V> struct hash<unordered_set<V> > {
        size_t operator()(const unordered_set<V>& x) const
        {
            size_t tot = 0;
            for(V elm : x){
                tot ^= std::hash<V>{}(elm);
            }
            return tot;
        }
    };

    template <typename N, typename V> struct hash<std::pair<N, V> >
    {
        size_t operator()(const std::pair<N, V>& x) const
        {
            return hash<N>{}(x.first) * hash<V>{}(x.second);
        }
    };

    // template <typename N, typename V> struct hash<std::pair<nfa_val<N>, V> >
    // {
    //     size_t operator()(const std::pair<nfa_val<N>, V>& x) const
    //     {
    //         return (int) (char) (x.first);
    //     }
    // };

    // template <typename N, typename V> struct hash<std::pair<N, std::unordered_set<V> > >
    // {
    //     size_t operator()(const std::pair<N, std::unordered_set<V> >& x) const
    //     {
    //         return hash<N>{}(x.first) * hash<std::unordered_set<V> >{}(x.second);
    //     }
    // };
}

template <typename N, typename T>
class NFA : public DFA<N, nfa_val<T> >{
protected:
    /**
     * @brief Replaces all kleen-stars in the nfa with the appropriate T vals in the
     * iterator range provided
     * 
     * @tparam it the type of the iterator provided (must be a forward-iterator)
     * @param nfa the NFA.
     * @param dict_begin the iterator to the beginning of the dictionary.
     * @param dict_end the iterator to the end of the dictionary.
     * @return NFA<N,T> the NFA after we remove all of the kleen-stars.
     */
    template <class it>
    static NFA<N,T> remove_star(NFA<N,T>& nfa, it dict_begin, it dict_end) {
        NFA<N,T> new_nfa = NFA<N,T>();
        new_nfa.add_start(nfa.get_start());
        for(auto vertex : nfa.vertices()){
            for(auto edge : nfa.edges(vertex)){
                if(edge.first.nfa_flag == nfa_val<T>::ALL){
                    for(it begin = dict_begin; begin != dict_end; ++begin){
                        new_nfa.add_transition(vertex, *begin, edge.second);
                    }
                }else{
                    new_nfa.add_transition(vertex, edge.first, edge.second);
                }
                
            }
        }
        for(auto vertex : nfa.vertices()){
            if(nfa.is_final(vertex)) new_nfa.add_final_state(vertex);
        }
        return new_nfa;
    }

    /**
     * @brief Removes all of the epsilons in the NFA by converting the nodes to
     * sets of nodes.
     * 
     * @param nfa the nfa we want to remove epsilons from.
     * @return NFA<std::unordered_set<N>,T> the nfa after the remove epsilon transformation.
     */
    static NFA<std::unordered_set<N>,T> remove_epsilon(NFA<N,T>& nfa){
        /**
         * The algorithm for removing epsilons from the NFA is as follows:
         * 1. Run DFS through all of the vertices and add the edges 
         * 2. 1-to-1 matching except, when the vertex that we are visiting has an epsilon edge
         * 3. If an epsilon edge exists, then we DFS through all epsilon edges and add that to the 
         * unordered set of "reachable states".
         */

        // Create an directed map of all vertices connected by an epsilon
        std::unordered_multimap<N,N> epsilon_edges = {};
        for(auto edge : nfa.edge_map){
            for(auto ed2 : edge.second){
                assert(ed2.first.nfa_flag != nfa_val<T>::ALL); // check that all stars have been taken out.
                if(ed2.first.nfa_flag == nfa_val<T>::EP){
                    epsilon_edges.insert({edge.first, ed2.second});
                }
            }
        }

        // Create a map of the vertex to the set of all of the vertices we
        // can get to by DFS through epsilons.
        std::unordered_map<N,std::unordered_set<N>> vertex_map = {};
        for(auto vertex : nfa.vertices()){
            std::vector<N> verts;
            verts.push_back(vertex);
            std::unordered_set<N> seen = {};
            while(verts.size() > 0){
                N next = verts[verts.size() - 1];
                verts.pop_back();
                if(!seen.count(next)){
                    seen.insert(next);
                    auto range = epsilon_edges.equal_range(next);
                    for(auto p = range.first; p != range.second; ++p){
                        if(!seen.count(p->second)) verts.push_back(p->second);
                    }
                }
            }
            vertex_map[vertex] = seen;
        }

        NFA<std::unordered_set<N>, T> ret_nfa;
        std::vector<std::unordered_set<N> > stk;
        stk.push_back(vertex_map[nfa.get_start()]);
        std::unordered_set<std::unordered_set<N>> seen = {};
        ret_nfa.add_start(vertex_map[nfa.get_start()]);
        while(stk.size() > 0){
            std::unordered_set<N> next = stk[stk.size() - 1];
            stk.pop_back();
            if(!seen.count(next)){
                seen.insert(next);
                std::unordered_set<T> valid_edges;
                for(N node : next){
                    for(auto edge : nfa.edges(node)){
                        valid_edges.insert(edge.first);
                    }
                }
                for(T edge_val : valid_edges){
                    std::unordered_set<N> con = {};
                    for(N node : next){
                        for(auto edge : nfa.edges(node)){
                            if(edge.first.nfa_flag != nfa_val<T>::NONE) continue;
                            if((T) edge.first == edge_val){
                                con.insert(vertex_map[edge.second].begin(), vertex_map[edge.second].end());
                            }
                        }
                    }
                    ret_nfa.add_transition(next, edge_val, con);
                    for(N node : con){
                        if(nfa.is_final(node)){
                            ret_nfa.add_final_state(con);
                            break;
                        }
                    }
                    stk.push_back(con);
                }
            }
        }
        for(N node : vertex_map[nfa.get_start()]){
            if(nfa.is_final(node)){
                ret_nfa.add_final_state(vertex_map[nfa.get_start()]);
                break;
            }
        }
        
        return ret_nfa;
    }

public:
    /**
     * @brief Replaces all nfa_vals with their respective T values.
     * REQUIRES that the NFA does not contain an EPSILON or STAR.
     * 
     * @param nfa the NFA (without an epsilon or star).
     * @return DFA<N,T> the DFA after replacing all nfa_vals.
     */
    static DFA<N,T> replace_nfa_vals(NFA<N,T>& nfa){
        DFA<N,T> new_dfa = DFA<N,T>();
        new_dfa.add_start(nfa.start);
        for(auto vertex : nfa.vertices())
            for(auto edge : nfa.edges(vertex))
                new_dfa.add_transition(vertex, (T) edge.first, edge.second);
        for(auto vertex : nfa.vertices())
            if(nfa.is_final(vertex))
                new_dfa.add_final_state(vertex);
        return new_dfa;
    }

    template <class col>
    DFA<std::unordered_set<N>, T> convert_to_dfa(col dict){
        return this->convert_to_dfa(*this, dict.begin(), dict.end());
    }

    template <class it>
    DFA<std::unordered_set<N>, T> convert_to_dfa(it dict_begin, it dict_end){
        return this->convert_to_dfa(*this, dict_begin, dict_end);
    }

    template <class it>
    static DFA<std::unordered_set<N>, T> convert_to_dfa(NFA<N,T>& nfa, it dict_begin, it dict_end){
        NFA<N,T> nfa_1 = remove_star(nfa, dict_begin, dict_end);
        NFA<std::unordered_set<N>,T> nfa_2 = remove_epsilon(nfa_1);
        DFA<std::unordered_set<N>,T> dfa = NFA<std::unordered_set<N>,T>::replace_nfa_vals(nfa_2);
        return dfa;
    }
};