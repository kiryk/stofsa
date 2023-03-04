# stofsa
The tool translates input set of whitespace-separated words into a minimal
finite state automaton that recognizes these and only these words.

It is rather fast. Building an FSA for all Polish words in all of their
lexical forms (~3800454 entries) takes just a few seconds running on a single
x86 core at 1.6GHz.

The implementation is based on the following paper:
> Jan Daciuk, Stoyan Mihov, Bruce W. Watson, Richard E. Watson;
> Incremental Construction of Minimal Acyclic Finite-State Automata.
> _Computational Linguistics_ 2000; 26 (1): 3–16.

# Usage
Compiling the tool:
```
make
```

Producing an FSA:
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
