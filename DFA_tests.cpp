#include "FA.hpp"
#include "DFA.hpp"
#include <string>
#include <iostream>
#include <vector>

std::ostream& operator<<(std::ostream& out, const DFA<long long, char>& dfa) {
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

int main(){
    DFA<std::string, char> dfa = DFA<std::string, char>();
    dfa.add_transition("start", 'm', "m");
    dfa.add_transition("m", 'o', "mo");
    dfa.add_transition("mo", 'n', "monkey");
    dfa.add_transition("mo", 'u', "mou");
    dfa.add_transition("mou", 's', "mous");
    dfa.add_transition("mous", 'e', "mouse");
    dfa.add_transition("mou", 't', "mout");
    dfa.add_transition("mout", 'h', "mouth");

    dfa.add_start("start");

    dfa.add_final_state("mouse");
    dfa.add_final_state("mouth");

    std::string cur = dfa.get_start();
    while(!dfa.is_final(cur)){
        printf("Current Node: %s\n", cur.c_str());
        char next_transition = '\0';

        for(auto edge : dfa.edges(cur)){
            printf("Edge: %s, %c -> %s\n", cur.c_str(), edge.first, edge.second.c_str());
            next_transition = edge.first;
        }
        if(next_transition == '\0'){
            break;
        }
        cur = dfa.next_state(cur, next_transition);
    }
    if(dfa.is_final(cur)){
        printf("ACCEPT\n");
    }else{
        printf("REJECT\n");
    }
    dfa.add_transition("start",'a',"a");
    dfa.add_transition("a",'p',"ap");
    dfa.add_transition("ap", 'p', "app");
    dfa.add_transition("app",'l',"apple");

    std::vector<char> seq1 = {'m', 'o', 'u', 's', 'e'};
    std::vector<char> seq2 = {'m', 'o', 'u', 't', 'h'};
    std::vector<char> seq3 = {'m', 'q', 'u', 't', 'h'};
    std::vector<char> seq4 = {'m', 'o', 'u', 't', 'h', 'y'};

    std::vector< std::vector<char> > seqs = {seq1, seq2, seq3, seq4};
    std::cout << "\n";
    for(auto seq : seqs){
        for(auto c : seq){
            std::cout << c;
        }
        std::cout << " " << dfa.run(seq.begin(), seq.end()) << "\n";

    }

    std::cout << "\n" << dfa;
    std::cout << "\n" << DFA<std::string,char>::compress_dfa(dfa);

    std::cout << "\n";
    DFA<long long, char> compressed = dfa.compress_dfa();
    for(auto seq : seqs){
        for(auto c : seq){
            std::cout << c;
        }
        std::cout << " " << compressed.run(seq.begin(), seq.end()) << "\n";

    }
}