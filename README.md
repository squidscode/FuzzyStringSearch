# Fuzzy String Search
## Description
This project is a general purpose C++ library for fuzzy-string-searching. Currently it supports loading a dictionary searching AND document searching. This project only supports *levenschtein distance* searching (ie. given a string or word, find all results that are **n** errors away from the search), but progress is being made to incorporate other forms of edit distance. 

To demonstrate this functionality, I have included two executables that work on dictionary files and document files. 

### Word Search Command Line Interface

To build the word_search binary, call `make`. The help information for the generated binary can be called with the `-h` or `--help` flag:
```
$ bin/word_search -h
usage: word_search [-d | --debug] [-s | --save] [-h | --help] FILE_NAME

Builds a suffix tree out of the given dictionary file (the file MUST be newline separated).

  d : print debug information [for developer use only]
  s : forces a file read and saves the trie in a `.cache` directory
  h : print this help message

There are three ways to search for a word in the provided dictionary via the command line interface:

  > WORD                      : searches for the word in the dictionary with 0 errors
  > WORD N                    : searches for the word in the dictionary with N errors
  > "WORD_1 WORD_2 ..." N     : searches for each of the words with N errors
  > 'WORD' N                  : searches for the word (without escaping spaces) with N errors
```

Here's an example of the executable working:

```
$ bin/word_search data/dict_files/words.txt 
Loading ..................................... Done!
> howdy
(howdy)
> howdy 1
(hoddy, rowdy, dowdy, hoody, gowdy, howdy)
> ' hope' 1
(hope, shope)
> "good day"
(good)
(day)
> "good day" 1
(glood, wood, goof, goad, lood, god, good, gowd, gool, mood, goon, geod, goo, bood, pood, goos, goods, rood, goog, gold, glod, goop, food, gond, goody, gook, hood)
(dal, day, may, dap, dey, lay, bay, dak, ay, das, nay, dag, hay, dau, dat, jay, daw, fay, dab, days, dah, gay, pay, dam, davy, way, dry, dy, aday, cay, dar, dae, dan, say, dazy, dao, ray, tay, dray, kay, yday, da, dad, yay)
```