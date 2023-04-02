#include "FA.hpp"
#include "NFA.hpp"
#include <string>
#include <iostream>
#include <vector>

std::ostream& operator<<(std::ostream& out, const DFA<std::string, nfa_val<char> >& dfa) {
    for(auto df : dfa.name_map){
        out << "(" << df.first << ", " << df.second.is_accept() << ") ";
    }
    out << "\n";
    for(auto v : dfa.edge_map){
        out << (v.first == dfa.start ? "*** " : "") << v.first << ": ";
        for(auto e : v.second){
            char v = (e.first.nfa_flag == nfa_val<char>::NONE ? (char) e.first :
                      e.first.nfa_flag == nfa_val<char>::EP ? '^' : '*');
            out << "(" << v << ", " << e.second << ") ";
        }
        out << "\n";
    }
    return out;
}

std::ostream& operator<<(std::ostream& out, const std::unordered_set<std::string>& names) {
    for(auto name : names){
        out << name << " ";
    }
    return out;
}

int main(){
    NFA<std::string, char> nfa = NFA<std::string, char>();
    nfa.add_transition("start", 'm', "m");
    nfa.add_transition("start", nfa_val<char>::EP, "monkey");
    nfa.add_transition("start", nfa_val<char>::ALL, "*");
    nfa.add_transition("m", 'o', "mo");
    nfa.add_transition("m", 'o', "money");
    nfa.add_transition("mo", 'n', "monkey");
    nfa.add_transition("mo", 'u', "mou");
    nfa.add_transition("mou", 's', "mous");
    nfa.add_transition("mous", 'e', "mouse");
    nfa.add_transition("mou", 't', "mout");
    nfa.add_transition("mout", 'h', "mouth");

    nfa.add_start("start");

    nfa.add_final_state("mouse");
    nfa.add_final_state("mouth");

    std::string cur = nfa.get_start();
    while(!nfa.is_final(cur)){
        printf("Current Node: %s\n", cur.c_str());
        std::string next = "";
        for(auto edge : nfa.edges(cur)){
            char v = (edge.first.nfa_flag == nfa_val<char>::NONE ? (char) edge.first :
                      edge.first.nfa_flag == nfa_val<char>::EP ? 'E' : 'A');
            printf("Edge: %s, %c -> %s\n", cur.c_str(), v, edge.second.c_str());
            next = edge.second;
        }
        if(next == ""){
            break;
        }
        cur = next;
        // THIS NO LONGER WORKS!!
        // cur = nfa.next_state(cur, next_transition); 
    }
    if(nfa.is_final(cur)){
        printf("ACCEPT\n");
    }else{
        printf("REJECT\n");
    }
    nfa.add_transition("start",'a',"a");
    nfa.add_transition("a",'p',"ap");
    nfa.add_transition("ap", 'p', "app");
    nfa.add_transition("app",'l',"apple");

    std::vector<char> seq1 = {'m', 'o', 'u', 's', 'e'};
    std::vector<char> seq2 = {'m', 'o', 'u', 't', 'h'};
    std::vector<char> seq3 = {'m', 'q', 'u', 't', 'h'};
    std::vector<char> seq4 = {'m', 'o', 'u', 't', 'h', 'y'};

    // std::vector< std::vector<char> > seqs = {seq1, seq2, seq3, seq4};
    // std::cout << "\n";
    // for(auto seq : seqs){
    //     for(auto c : seq){
    //         std::cout << c;
    //     }
    //     std::cout << " " << nfa.run(seq.begin(), seq.end()) << "\n";

    // }

    std::cout << "\n" << nfa;
    std::cout << "\n" << "Verices: " << nfa.vertices() << "\n";

    
}