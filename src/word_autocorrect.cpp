#include "data_structures/levenshtein_nfa.hpp"
#include "data_structures/FA/NFA.hpp"
#include "data_structures/FA/DFA.hpp"
#include "data_structures/trie.hpp"
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
#define get_time(typ)   std::chrono::duration_cast<std::chrono::typ>(std::chrono::system_clock::now().time_since_epoch())
#define df_tmp(typ)     std::chrono::typ tmp
#define time(typ, op)   (tmp = get_time(typ), op, get_time(typ) - tmp)
#define CM              ,
#define FORCE(typ, v)   (*((typ*) &(v)))
#define MAX_WORD        25
#define MAX_STRING      256

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
} env;


void computation(env& e, DFA<ll, char>& compressed_dict, std::unordered_set<char>& alphabet, char* word, int& error){
  dprintf("READ: %s, %d\n", word, error);
  if(strlen(word) == 0) {printf("()\n"); return;};

  DFA<ll, char> lnfa = levenshtein_nfa(word, error).convert_to_dfa(alphabet).compress_dfa();
  DFA<ll , char> intersection;
  // time(milliseconds, DFA<ll CM char> intersection = compressed_dict.intersection(lnfa).compress_dfa())
  df_tmp(milliseconds);
  auto execution_time = time(milliseconds, intersection = compressed_dict.intersection(lnfa).compress_dfa());
  dprintf("Levenschtein DFA size: %lu states\n", lnfa.states().size());
  dprintf("Intersection [dict ^ lnfa] DFA size: %lu states\n", intersection.states().size());
  dprintf("Intersection [dict ^ lnfa] DFA execution time: %llu ms\n", FORCE(unsigned long long, execution_time));
  ifd {
    DFA<ll, char> intersection2;
    execution_time = time(milliseconds, intersection2 = lnfa.intersection(compressed_dict).compress_dfa());
    dprintf("Intersection [lnfa ^ dict] DFA size: %lu states\n", intersection2.states().size());
    dprintf("Intersection [lnfa ^ dict] DFA execution time: %llu ms\n", FORCE(unsigned long long, execution_time));
  }
  // std::cout << intersection;
  bool first = true;
  printf("(");
  for(std::vector<char> result : intersection.accept_paths()){
    if(!first) printf(", ");
    first = false;
    for(char c : result){
      printf("%c", c);
    }
  }
  printf(")\n");
}

// the search loop
void begin_search_loop(env e){
  // Construct a dict trie:
  trie dict;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;
  int LOADING_INTERVAL = 10000;
  int dict_size = 0;


  FILE* fs = fopen(e.file_path, "r");
  if(fs == NULL){
    fprintf(stderr, "ERROR: File error! Check if the file exists and if reads are allowed.\n");
    exit(1);
  }
  printf("Loading ");
  while((read = getline(&line, &len, fs)) != -1){
    std::string s = line;
    dprintf("line read from file: %s\n", s.substr(0, s.size() - 1).c_str()); fflush(stdout);
    if(strcmp(line, "\n") == 0) continue;
    dict.insert((trim(s), s));
    if(!e.debug && ++dict_size % LOADING_INTERVAL == 0) (dict_size %= LOADING_INTERVAL, printf("."), fflush(stdout));
  }
  printf(" Done!\n"); 
  cprintf("> "); fflush(stdout);
  fclose(fs);

  if(e.save_trie){
    std::ofstream of; of.open("trie.txt");
    of << dict; // add the entire dict.
    of.close();
  }

  // Alphabet construction:
  std::unordered_set<char> alphabet = dict.get_alphabet();

  DFA<ll, char> compressed_dict = dict.compress_dfa();
  while(getline(&line, &len, stdin) != -1){ // while lines can be read:
    char word[MAX_WORD] = ""; int error = 0;
    
    if(line[0] == '\'' && e.cli){
      int i = 1;
      while(i < strlen(line) && line[i] != '\'') ++i; // find the last i
      if(i != strlen(line)){
        sscanf(line+i+1, " %d", &error);
        line[i] = '\0';
        computation(e, compressed_dict, alphabet, line+1, error);
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
          computation(e, compressed_dict, alphabet, tok, error);
          tok = strtok(NULL, " ");
        }
      }else{
        printf("Error: A STRING message must end with a \"!\n");
      }
    }else{
      sscanf(line, "%25s %d", word, &error);
      computation(e, compressed_dict, alphabet, word, error);
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

// the main driver
int main(int argc, char* argv[]){
  env main_env{};
  std::unordered_map<std::string, std::function<void(env&,int&,char**)> > commands;
  commands["-d"] = debug_switch;
  commands["--debug"] = debug_switch;
  commands["--cli"] = command_line_interface;
  commands["-s"] = save_trie;
  commands["--save"] = save_trie;
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