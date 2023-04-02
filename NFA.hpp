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

    template <typename V> struct hash<std::pair<nfa_val<char>, V> >
    {
        size_t operator()(const std::pair<nfa_val<char>, V>& x) const
        {
            return (int) (char) (x.first);
        }
    };
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
         * 
         */

    }

    /**
     * @brief Replaces all nfa_vals with their respective T values.
     * REQUIRES that the NFA does not contain an EPSILON or STAR.
     * 
     * @param nfa the NFA (without an epsilon or star).
     * @return DFA<N,T> the DFA after replacing all nfa_vals.
     */
    static DFA<N,T> replace_nfa_vals(NFA<N,T>& nfa){
        DFA<N,T> new_dfa = DFA<N,T>();
        new_dfa.start = nfa.start;
        new_dfa.start_flag = nfa.start_flag;
        new_dfa.name_map = nfa.name_map;
        for(auto vertex : nfa.vertices())
            for(auto edge : nfa.edges(new_dfa))
                new_dfa.edge_map[vertex][edge.first] = edge.second;
        return new_dfa;
    }

public:
    template <class it>
    DFA<N, T> convert_to_dfa(it dict_begin, it dict_end){
        return this->convert_to_dfa(*this, dict_begin, dict_end);
    }

    template <class it>
    static DFA<std::unordered_set<N>, T> convert_to_dfa(NFA<N,T>& nfa, it dict_begin, it dict_end){
        NFA<std::unordered_set<N>,T> new_nfa = NFA<std::unordered_set<N>,T>();
        new_nfa = remove_epsilon(remove_star(nfa, dict_begin, dict_end));
        return replace_nfa_vals(nfa);
    }
};