#include "../src/data_structures/FA/NFA.hpp"
#include <string>
#include <iostream>
#include <vector>
#include <functional>

int test_number = 1;
int failed_tests = 0;
std::vector<int> failed_test_numbers = {};
template <class T, class Callable>
void run_test(Callable fn, T expected_output){
    std::cout << "Test #" << test_number++ << ": ";
    T result = fn();
    if(result == expected_output){ 
        std::cout << "[PASS]";
    }else{
        std::cout << "[FAIL]";
        failed_tests += 1;
        failed_test_numbers.push_back(test_number - 1);
    }
    std::cout << "\n";
}

void print_test_results(){
    std::cout << "\n\nTest Results:\n";
    std::cout << "# of tests run: " << test_number - 1 << "\n";
    std::cout << "# of failed tests: " << failed_tests << "\n";
}

NFA<std::string, char> construct_subseq(std::string s){
    NFA<std::string, char> nfa = NFA<std::string, char>();
    nfa.add_start("");
    std::string acc = "";
    for(char c : s){
        nfa.add_transition(acc, nfa_val<char>::EP, acc + c);
        nfa.add_transition(acc, c, acc + c);
        acc += c;
    }
    nfa.add_final_state(acc);
    return nfa;
}

void run_test_suite(){
    std::vector<char> alphabet = {};
    for(char v = 'a'; v <= 'z'; v++){
        alphabet.push_back(v);
    }

    /**
     * match_ELLO matches 
     * NOTE: We match lowercase `hello`, NOT uppercase.
     */
    NFA<std::string, char> match_ELLO = NFA<std::string, char>();
    match_ELLO.add_start("");
    match_ELLO.add_transition("", nfa_val<char>::ALL, ".");
    match_ELLO.add_transition(".", 'e', ".e");
    match_ELLO.add_transition(".e", 'l', ".el");
    match_ELLO.add_transition(".el", 'l', ".ell");
    match_ELLO.add_transition(".ell", 'o', ".ello");
    match_ELLO.add_transition("", nfa_val<char>::EP, "_");
    match_ELLO.add_transition("_", 'e', "_e");
    match_ELLO.add_transition("_e", 'l', "_el");
    match_ELLO.add_transition("_el", 'l', "_ell");
    match_ELLO.add_transition("_ell", 'o', "_ello");
    match_ELLO.add_final_state(".ello");
    match_ELLO.add_final_state("_ello");
    DFA<std::unordered_set<std::string>, char> med = match_ELLO.convert_to_dfa(alphabet); 
    run_test([&med](){return med.run(std::vector<char>{});}, false);
    run_test([&med](){return med.run(std::vector<char>{'f', 'e'});}, false);
    run_test([&med](){return med.run(std::vector<char>{'f', '\0'});}, false);
    run_test([&med](){return med.run(std::vector<char>{'f', 'e', 'k', 'l', 'o'});}, false);
    run_test([&med](){return med.run(std::vector<char>{'f', 'e', 'l', 'l', 'o'});}, true);
    run_test([&med](){return med.run(std::vector<char>{'e', 'l', 'l', 'o'});}, true);
    run_test([&med](){return med.run(std::vector<char>{'e', 'e', 'l', 'l', 'o'});}, true);
    std::cout << "\n";

    /*
     * Using an NFA to determine if a word is a subsequence of a string.
     */
    DFA<ll, char> unicorn = construct_subseq("unicorn").convert_to_dfa(alphabet).compress_dfa();
    std::cout << "Testing Subsequences:\nWord: 'unicorn'\n";
    run_test([&unicorn](){return unicorn.run(std::vector<char>{});}, true);
    run_test([&unicorn](){return unicorn.run(std::vector<char>{'u'});}, true);
    run_test([&unicorn](){return unicorn.run(std::vector<char>{'u', 'n', 'i'});}, true);
    run_test([&unicorn](){return unicorn.run(std::vector<char>{'u', 'n', 'i', 'c', 'o', 'r', 'n'});}, true);
    run_test([&unicorn](){return unicorn.run(std::vector<char>{'u', 'n', 'c', 'i', 'o', 'r', 'n'});}, false); // swap!
    run_test([&unicorn](){return unicorn.run(std::vector<char>{'u', 'i', 'r', 'n'});}, true);
    run_test([&unicorn](){return unicorn.run(std::vector<char>{'i', 'r', 'n'});}, true);
    run_test([&unicorn](){return unicorn.run(std::vector<char>{'c', 'o', 'n'});}, true);
    run_test([&unicorn](){return unicorn.run(std::vector<char>{'u', 'u', 'n'});}, false);
    std::cout << "\n";

    /**
     * Testing the functionality of multiple edge dispatch
     */
    NFA<std::string, char> h_ello_elp;
    std::cout << "Testing edge dispatch (ie. 2 edges with the same name)\n";
    h_ello_elp.add_start("");
    h_ello_elp.add_transition("", 'h', "h");
    h_ello_elp.add_transition("h", 'e', "hello");
    h_ello_elp.add_transition("hello", 'y', "hello-YES");
    h_ello_elp.add_transition("h", 'e', "help");
    h_ello_elp.add_transition("help", 'o', "help-OK");
    h_ello_elp.add_final_state("hello-YES");
    h_ello_elp.add_final_state("help-OK");
    DFA<ll, char> comp = h_ello_elp.convert_to_dfa(alphabet).compress_dfa();
    run_test([&comp](){return comp.run(std::vector<char>{});}, false);
    run_test([&comp](){return comp.run(std::vector<char>{'h', 'e'});}, false);
    run_test([&comp](){return comp.run(std::vector<char>{'h', 'e', 'y'});}, true);
    run_test([&comp](){return comp.run(std::vector<char>{'h', 'e', 'o'});}, true);
    print_test_results();
}

int main(){
//    visual_tests(); // Uncomment for visual confirmation of a few NFAs.
    // NFA tests attempting to use * and epsilon.
    run_test_suite();
}


/*
std::ostream& operator<<(std::ostream& out, const DFA<std::string, nfa_val<char> >& dfa) {
    for(auto df : dfa.name_map){
        out << "(" << df.first << ", " << (df.second.is_accept() ? "ACCEPT" : "REJECT") << ") ";
    }
    out << "\n";
    for(auto v : dfa.name_map){
        out << (v.first == dfa.start ? "*** " : "") << v.first << ": ";
        if(!dfa.edge_map.count(v.first)){
            out << "\n";
            continue;
        }
        for(auto e : dfa.edge_map.at(v.first)){
            char v = (e.first.nfa_flag == nfa_val<char>::NONE ? (char) e.first :
                      e.first.nfa_flag == nfa_val<char>::EP ? '^' : '*');
            out << "(" << v << ", " << e.second << ") ";
        }
        out << "\n";
    }
    return out;
}
std::ostream& operator<<(std::ostream& out, const std::unordered_set<std::string>& set){
    out << "{";
    for(std::string s : set){
        out << s.c_str() << ", ";
    }
    out << "}";
    return out;
}
std::ostream& operator<<(std::ostream& out, const DFA<std::unordered_set<std::string>, nfa_val<char> >& dfa) {
    for(auto df : dfa.name_map){
        out << "(" << df.first << ", " << (df.second.is_accept() ? "ACCEPT" : "REJECT") << ") ";
    }
    out << "\n";
    for(auto v : dfa.name_map){
        out << (v.first == dfa.start ? "*** " : "") << v.first << ": ";
        if(!dfa.edge_map.count(v.first)){
            out << "\n";
            continue;
        }
        for(auto e : dfa.edge_map.at(v.first)){
            char v = (e.first.nfa_flag == nfa_val<char>::NONE ? (char) e.first :
                      e.first.nfa_flag == nfa_val<char>::EP ? '^' : '*');
            out << "(" << v << ", "<< e.second << ") ";
        }
        out << "\n";
    }
    return out;
}
std::ostream& operator<<(std::ostream& out, const DFA<std::string, nfa_val<int> >& dfa) {
    for(auto df : dfa.name_map){
        out << "(" << df.first << ", " << (df.second.is_accept() ? "ACCEPT" : "REJECT") << ") ";
    }
    out << "\n";
    for(auto v : dfa.name_map){
        out << (v.first == dfa.start ? "*** " : "") << v.first << ": ";
        if(!dfa.edge_map.count(v.first)){
            out << "\n";
            continue;
        }
        for(auto e : dfa.edge_map.at(v.first)){
            int v = (e.first.nfa_flag == nfa_val<int>::NONE ? (int) e.first :
                      e.first.nfa_flag == nfa_val<int>::EP ? -1 : -2);
            out << "(" << v << ", " << e.second << ") ";
        }
        out << "\n";
    }
    return out;
}
std::ostream& operator<<(std::ostream& out, const DFA<std::unordered_set<std::string>, nfa_val<int> >& dfa) {
    for(auto df : dfa.name_map){
        out << "(" << df.first << ", " << (df.second.is_accept() ? "ACCEPT" : "REJECT") << ") ";
    }
    out << "\n";
    for(auto v : dfa.name_map){
        out << (v.first == dfa.start ? "*** " : "")<< v.first << ": ";
        if(!dfa.edge_map.count(v.first)){
            out << "\n";
            continue;
        }
        for(auto e : dfa.edge_map.at(v.first)){
            int v = (e.first.nfa_flag == nfa_val<int>::NONE ? (int) e.first :
                      e.first.nfa_flag == nfa_val<int>::EP ? -1 : -2);
            out << "(" << v << ", "<< e.second << ") ";
        }
        out << "\n";
    }
    return out;
}

// Checking that all of the nfas are correctly converted.
void visual_tests(){
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

    std::unordered_set<std::string> ps{"hi", "there"};
    std::cout << ps << "\n";

    std::cout << "\n" << "Verices: " << nfa.vertices() << "\n";

    // The following would error:
    // std::cout << "\n\n" << nfa.remove_epsilon(nfa) << "\n";
    

    // Lets create an dfa that 0 or 1, but nothing else:
    NFA<std::string, int> nfa2;
    nfa2.add_start("start");
    nfa2.add_transition("start", nfa_val<int>::EP, "st1");
    nfa2.add_transition("start", nfa_val<int>::EP, "st2");
    nfa2.add_transition("st1", 0, "0");
    nfa2.add_transition("st2", 1, "1");
    nfa2.add_transition("0", 0, "reject");
    nfa2.add_transition("0", 1, "reject");
    nfa2.add_transition("1", nfa_val<int>::ALL, "reject");
    nfa2.add_transition("reject", 0, "reject");
    nfa2.add_transition("reject", 1, "reject");
    nfa2.add_final_state("0"); nfa2.add_final_state("1");
    std::cout << "\nWITH EPSILON:\n" << nfa2 << "\n";
    std::vector<int> bin = {0, 1};
    DFA<std::unordered_set<std::string>, int> dfa1 = nfa2.convert_to_dfa(bin.begin(), bin.end());
    std::vector<int> v1 = {0};
    std::cout << (dfa1.run(v1.begin(), v1.end()) ? "True" : "False") << " -- 0\n";
    v1 = {1};
    std::cout << (dfa1.run(v1.begin(), v1.end()) ? "True" : "False") << " -- 1\n";
    v1 = {0,1,0,1};
    std::cout << (dfa1.run(v1.begin(), v1.end()) ? "True" : "False") << " -- 0 1 0 1\n";
    v1 = {1,1,0,1};
    std::cout << (dfa1.run(v1.begin(), v1.end()) ? "True" : "False") << " -- 1 1 0 1\n";
}

*/