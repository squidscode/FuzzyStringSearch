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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>


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
  bool cli{true};             // Assume that we want to use the CLI display by default. 
  bool skip_newline{true};
  int  chunk_size{15};
  bool lc_mode{true};
} env;

std::string dir_path;
std::string sfx_path;
DFA<ll, char> compressed_dict;

void computation(env& e, suffix_tree& st, DFA<ll, char>& compressed_dict, std::unordered_set<char>& alphabet, char* word, int& error){
  dprintf("READ: %s, %d\n", word, error);
  if(strlen(word) == 0) {printf("\n"); return;};
  if(strlen(word) > e.chunk_size) {printf("'%s' is longer than the chunk size [%i]\n", word, e.chunk_size); return;}

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

bool file_exists(char* fp){
  FILE* fs = fopen(fp, "r");
  struct stat s;
  stat(fp, &s);
  if(fs == NULL || !S_ISREG(s.st_mode)){
    return false;
  }
  return true;
}

void save_file(env& e){
  if(!file_exists(e.file_path)){
    fprintf(stderr, "ERROR: File error! Check if the file exists and if reads are allowed.\n");
    return;
  }

  // Create the '.cache' folder if not already there
  struct stat st{0};
  if(stat(dir_path.c_str(), &st) == -1) {
    mkdir(dir_path.c_str(), 0700);
  }

  // Save all information to that file.
  std::ofstream of; of.open(sfx_path.c_str(), std::ofstream::binary);
  serialize(of, compressed_dict); // add the entire dict.
  of.close();
}

void initialize_paths(env& e, char* fp){
  // Path string constructions:
  sfx_path = fp;
  sfx_path.insert(sfx_path.rfind('/'), std::string("/.cache"));
  std::string suf = ".sfx" + std::to_string(e.chunk_size);
  sfx_path = sfx_path.replace(sfx_path.begin() + sfx_path.rfind('.'), sfx_path.end(), suf);
  std::string tmp = sfx_path;
  dir_path = tmp.replace(tmp.begin() + tmp.rfind('/'), tmp.end(), "");
}

void wait_for_file_load(env& e){
  printf("Use `load FILE_PATH` to load a file\n");
  printf("Use `save FILE_PATH` to save a file to the local '.cache' directory.\n");
  char *buf; size_t buf_size = 64;
  buf = new char[64];
  while(!strcmp(e.file_path, "")){
    char *fp; char *input;
    printf("> "); fflush(stdout);
    getline(&buf, &buf_size, stdin);
    if(sscanf(buf, "load %s", fp) == 1){
      if(fp[0] == '\''){
        fp = &fp[1];
        int i = 0;
        while(fp[i] != '\0'){
          if(fp[i++] == '\'') {fp[i - 1] = '\0'; break;}
        }
      }
      if(!file_exists(fp)){
        fprintf(stderr, "ERROR: File error! Check if the file exists and if reads are allowed.\n");
        continue;
      }
      e.file_path = fp;
    }else if(sscanf(buf, "save %s", fp) == 1){
      if(fp[0] == '\''){
        fp = &fp[1];
        int i = 0;
        while(fp[i] != '\0'){
          if(fp[i++] == '\'') {fp[i - 1] = '\0'; break;}
        }
      }
      if(!file_exists(fp)){
        fprintf(stderr, "ERROR: File error! Check if the file exists and if reads are allowed.\n");
        continue;
      }
      e.file_path = fp;
      suffix_tree doc;
      initialize_paths(e, fp);
      doc.load_file(e.file_path, e.chunk_size);
      compressed_dict = doc.compress_dfa();
      save_file(e);
      strcpy(e.file_path, "");
    }
  }
}

// the search loop
void begin_search_loop(env& e){
  // Construct a dict trie:
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  std::unordered_set<char> alphabet;
  suffix_tree doc;

  if(!strcmp(e.file_path, "")){
    wait_for_file_load(e);
  }

  FILE* fs = fopen(e.file_path, "r");
  if(fs == NULL){
    fprintf(stderr, "ERROR: File error! Check if the file exists and if reads are allowed.\n");
    exit(1);
  }

  initialize_paths(e, e.file_path);

  if(fopen(sfx_path.c_str(), "r") == NULL){
    printf("[No cache found] Loading ..."); fflush(stdout);
    doc.load_file(e.file_path, e.chunk_size);
    alphabet = doc.get_alphabet();
    compressed_dict = doc.compress_dfa();
    if(e.save_trie){  // If we want to save the trie.
      // Create the '.cache' folder if not already there
      struct stat st{0};
      if(stat(dir_path.c_str(), &st) == -1) {
        mkdir(dir_path.c_str(), 0700);
      }

      // Save all information to that file.
      std::ofstream of; of.open(sfx_path.c_str(), std::ofstream::binary);
      serialize(of, compressed_dict); // add the entire dict.
      of.close();
    }
  }else{
    printf("[Cache found] Loading ..."); fflush(stdout);
    doc.load_file(e.file_path, e.chunk_size, false);
    alphabet = doc.get_alphabet();
    std::ifstream ifs(sfx_path, std::ifstream::binary);
    deserialize(ifs, compressed_dict);
    ifs.close();
  }
  printf(" Done!\n"); 
  cprintf("> "); fflush(stdout);
  fclose(fs);

  // Alphabet construction:

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
  main_env.file_path = new char[1];
  strcpy(main_env.file_path, "");
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