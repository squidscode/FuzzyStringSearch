#include "data_structures/levenshtein_nfa.hpp"
#include "data_structures/FA/NFA.hpp"
#include "data_structures/FA/DFA.hpp"
#include "data_structures/suffix_tree.hpp"
#include "util/trim.cpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <chrono>

// Macros:
#define dprintf(...)    if(e.debug) printf(__VA_ARGS__)
#define cprintf(...)    if(e.cli)   printf(__VA_ARGS__)
#define ifd             if(e.debug)
#define ifn(...)        if(e.skip_newline && __VA_ARGS__)
#define get_time(typ)   std::chrono::duration_cast<std::chrono::typ>(std::chrono::system_clock::now().time_since_epoch())
#define df_tmp(typ)     std::chrono::typ tmp
#define time(typ, op)   (tmp = get_time(typ), op, get_time(typ) - tmp)
#define CM              ,
#define FORCE(typ, v)   (*((typ*) &(v)))
#define MAX_WORD        25
#define MAX_STRING      256
#define clear()         printf("\033[H\033[J")
#define gotoxy(x,y)     printf("\033[%d;%dH", (y), (x))

namespace std{
  template <typename T> struct hash<std::vector<T> >
  {
    size_t operator()(const std::vector<T>& x) const
    {
      size_t tot = 0;
      for(T a : x){
        tot ^= a;
      }
      return tot;
    }
  };
}

// The environment template
typedef struct env_t {
  char* file_path;
  bool debug{false};
  bool save_trie{false};
  bool cli{false};
  bool skip_newline{true};
  int  chunk_size{10};
  bool lc_mode{true};
} env;

void computation(env& e, suffix_tree& st, DFA<ll, char>& compressed_dict, std::unordered_set<char>& alphabet, char* word, int& error){
  dprintf("READ: %s, %d\n", word, error);
  if(strlen(word) == 0) {printf("\n"); return;};

  auto lnfa = levenshtein_nfa(word, error);
  for(auto acc : lnfa.accept_states()){ // if we are able to get to the end of the search query, then we always accept.
    lnfa.add_transition(acc, nfa_val<char>::STAR, acc);
  }
  auto lnfa_dfa = lnfa.convert_to_dfa(alphabet).compress_dfa();
  DFA<ll , char> intersection;
  // time(milliseconds, DFA<ll CM char> intersection = compressed_dict.intersection(lnfa).compress_dfa())
  df_tmp(milliseconds);
  auto execution_time = time(milliseconds, intersection = compressed_dict.intersection(lnfa_dfa).compress_dfa());
  dprintf("Levenschtein DFA size: %lu states\n", lnfa.states().size());
  dprintf("Intersection [dict ^ lnfa] DFA size: %lu states\n", intersection.states().size());
  dprintf("Intersection [dict ^ lnfa] DFA execution time: %llu ms\n", FORCE(unsigned long long, execution_time));
  ifd {
    DFA<ll, char> intersection2;
    execution_time = time(milliseconds, intersection2 = lnfa_dfa.intersection(compressed_dict).compress_dfa());
    dprintf("Intersection [lnfa ^ dict] DFA size: %lu states\n", intersection2.states().size());
    dprintf("Intersection [lnfa ^ dict] DFA execution time: %llu ms\n", FORCE(unsigned long long, execution_time));
  }
  // std::cout << intersection;
  bool first = true;
  for(std::vector<char> result : intersection.accept_paths()){
    first = false;
    std::string needle;
    std::string print_str;
    for(char c : result){
      needle += c;
      ifn(c == '\n' || c == '\r') {print_str += '\\'; continue;} // if the character is a newline mark it as a line skip
      if(isalnum(c) || isblank(c) || ispunct(c)) print_str += c;
      else print_str += '?';
    }
    int padding = e.chunk_size + 5;
    char pstr[5];
    char buf[30] = "%-";
    sprintf(pstr, "%d", padding);
    strcat(buf, pstr);
    strcat(buf, "s");
    printf(buf, print_str.c_str());

    ifn(true){
      if(e.lc_mode){
        std::unordered_set<std::pair<ll,ll> > ind = st.get_lc(needle);
        for(auto i : ind){
          sprintf(buf, " [line %lli, col %lli]", i.first, i.second);
          printf("%s", buf);
        }
      }else{
        std::unordered_set<ll> ind = st.get_indices(needle);
        for(auto i : ind){
          sprintf(buf, " [%lli]", i);
          printf("%s", buf);
        }
      }
      printf("\n");
    }
  }
}

// the search loop
void begin_search_loop(env e){
  // Construct a dict trie:
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  FILE* fs = fopen(e.file_path, "r");
  if(fs == NULL){
    fprintf(stderr, "ERROR: File error! Check if the file exists and if reads are allowed.\n");
    exit(1);
  }
  printf("Loading ... "); fflush(stdout);
  suffix_tree doc(e.file_path, e.chunk_size);
  printf(" Done!\n"); 
  cprintf("> "); fflush(stdout);
  fclose(fs);

  if(e.save_trie){
    std::ofstream of; of.open("suffix_trie.txt");
    of << doc; // add the entire dict.
    of.close();
  }

  // Alphabet construction:
  std::unordered_set<char> alphabet = doc.get_alphabet();

  DFA<ll, char> compressed_dict = doc.compress_dfa();
  while(getline(&line, &len, stdin) != -1){ // while lines can be read:
    char word[MAX_WORD] = ""; int error = 0;
    
    if(line[0] == '\'' && e.cli){
      int i = 1;
      while(i < strlen(line) && line[i] != '\'') ++i; // find the last i
      if(i != strlen(line)){
        sscanf(line+i+1, " %d", &error);
        line[i] = '\0';
        computation(e, doc, compressed_dict, alphabet, line+1, error);
      }else{
        printf("Error: A WORD message must end with a '!\n");
      }
    }else if(line[0] == '"' && e.cli){
      int i = 1;
      while(i < strlen(line) && line[i] != '"') ++i; // find the last i
      if(i != strlen(line)){
        sscanf(line+i+1, " %d", &error);
        line[i] = '\0';
        char buf[MAX_STRING];
        strcpy(buf, line+1);
        char* tok = strtok(buf, " ");
        while(tok){
          computation(e, doc, compressed_dict, alphabet, tok, error);
          tok = strtok(NULL, " ");
        }
      }else{
        printf("Error: A STRING message must end with a \"!\n");
      }
    }else{
      sscanf(line, "%25s %d", word, &error);
      computation(e, doc, compressed_dict, alphabet, word, error);
    }

    // for next line:
    cprintf("> "); fflush(stdout);
  }
}

void debug_switch(env& _env_, int& flag_pos, char* argv[]){
  _env_.debug = true;
}

void save_trie(env& _env_, int& flag_pos, char* argv[]){
  _env_.save_trie = true;
}

void command_line_interface(env& _env_, int& flag_pos, char* argv[]){
  _env_.cli = true;
}

void change_chunk_size(env& _env_, int& flag_pos, char* argv[]){
  _env_.chunk_size = atoi(argv[++flag_pos]);
}

int main(int argc, char* argv[]){
  env main_env{};
  std::unordered_map<std::string, std::function<void(env&,int&,char**)> > commands;
  commands["-d"] = debug_switch;
  commands["--debug"] = debug_switch;
  commands["--cli"] = command_line_interface;
  commands["-s"] = save_trie;
  commands["--save"] = save_trie;
  commands["--chunk"] = change_chunk_size;
  int st = 1;
  int pos = 0;
  while(st < argc){
    if(commands.count(argv[st])){ // if the flag is found
      commands[argv[st]](main_env,st,argv);
    }else{
      switch(pos){
        case 0:
          main_env.file_path = argv[st]; // the filepath
          break;
        default:
          fprintf(stderr, "ERROR: Default case for position argument called: %d\n", pos);
          exit(1);
      }
      ++pos;
    }
    ++st;
  }
  begin_search_loop(main_env);
}