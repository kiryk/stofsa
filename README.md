# stofsa
The tool translates input set of whitespace-separated words into a minimal
finite state automaton that recognizes these and only these words.

It is rather fast. Building an FSA for all Polish words in all of their
lexical forms (~3800454 entries) takes about 3s running on single x86 core
at 1.6GHz.

The input words must be sorted in lexical order. UTF-8 is fully supported
but files that contain non-ASCII charactes must be sorted by unicode
values of whole runes (not just bytes!).

The implementation is based on the following paper:
> Jan Daciuk, Stoyan Mihov, Bruce W. Watson, Richard E. Watson;
> Incremental Construction of Minimal Acyclic Finite-State Automata.
> _Computational Linguistics_ 2000; 26 (1): 3–16.

# Usage
Compiling the tool:
```
make
```

Producing a FSA:
```
./stofsa < wordset.txt > fsa.txt
```

The outpot consists of two types of entries:

```
src-state dst-state rune
```
It means that there's an edge from `src` to `dst` if the input is `rune`.


```
accepting-state
```
Means that `accepting-state` is an accepting state.
