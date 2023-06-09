# Fuzzy String Search
This project is a general purpose C++ library for fuzzy-string-searching. Currently it supports dictionary searching and document searching. This project only supports *levenschtein distance* searching (ie. given a string or word, find all results that are **n** errors away from the search), other distance metrics are yet to be implemented. 

To demonstrate this functionality, I have included two executables that work on dictionary files and document files (ie. '.txt' files). 

### Word Search Command Line Interface

To build the word_search binary, call `make`. The help information for the generated binary can be called with the `-h` or `--help` flag:
```
$ bin/word_search -h
usage: word_search [-d | --debug] [-s | --save] [-h | --help] FILE_NAME

Builds a trie out of the given dictionary file [FILE_NAME] (the file MUST be
newline separated). Then, search through the dictionary by specifying a string
and a levenschtein error.

  d : print debug information [for developer use only]
  s : forces a file read and saves the trie in a `.cache` directory
  h : print this help message

There are three ways to search in the provided dictionary via the command line
interface:

  > WORD                      : searches for the word in the dictionary with 0 errors
  > WORD N                    : searches for the word in the dictionary with N errors
  > "WORD_1 WORD_2 ..." N     : searches for each of the words with N errors
  > 'WORD' N                  : searches for the word (without escaping spaces)
                                with N errors
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

If we save the file before loading it, the program detects that a cache has already been created, and it automatically deserializes and loads the cached trie. This is considerably faster than reconstructing the trie from a dictionary.
```
$ time echo -n "" | bin/word_search data/dict_files/words.txt -s
Loading ..................................... Done!
> 
real    0m10.558s
user    0m10.041s
sys     0m0.489s

$ time echo -n "" | bin/word_search data/dict_files/words.txt
Loading  Done!
> 
real    0m1.739s
user    0m1.581s
sys     0m0.136s
```

### Document Search Command Line Interface

The document search CLI works similarly to the word search CLI, except that it searchs within a document, rather than a dictionary. The line number and column of each of the matches is printed alongside the match. 
```
$ bin/document_search -h
usage: document_search [-d | --debug] [-s | --save] [-c | --chunk N]
                       [-h | --help] [FILE_NAME]

Builds a suffix tree out of the given document (if provided). If no file
name is provided, then file mode is activated and the user can load and
save files via the `load` and `save` commands. Then, it allows the user to
search for strings in the document with the given levenschtein error.

  d : print debug information [for developer use only]
  s : forces a file read and saves the trie in a `.cache` directory
  c : the size of the chunks used in the suffix tree (a larger chunk
      size results in more preprocessing time and memory consumption)
  h : print this help message

There are three ways to search in the provided file via the command line
interface:

  > WORD                      : searches for the word in the document with 0 errors
  > WORD N                    : searches for the word in the document with N errors
  > "WORD_1 WORD_2 ..." N     : searches for each of the words with N errors
  > 'WORD' N                  : searches for the word (without escaping 
                                spaces) with N errors
```

A query using the document_search binary will print out the line number and the column that the match is associated with. 
```
$ bin/document_search data/docs/frankenstein.txt
[No cache found] Loading ... Done!
> assert
asserted omnipo      [line 1138, col 44]
assertion again      [line 6068, col 27]
assertion; when      [line 6051, col 1]
```