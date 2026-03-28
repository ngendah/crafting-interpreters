# Pratt Parser

A "Simple As Possible" Pratt parser implemented in C++.
Its grammar alphabet is `[a-z0-9=+-*/^]`.

The goal is to build intuition for how Pratt parsers work, so this implementation is intentionally minimal. For example, operators can be used as both infix and prefix, leading to interesting results. This intuition is helpful for understanding how the `ByteCodeVM` compiler works.

- Configure:

- Configure:
```
cmake -S . -B build
```

- Build:
```
cmake --build build
```

- Run:
The main program source is in the `bin` directory, and the built binary is in `build/bin`.
To run:
```
cd build/bin
./parser
```


References:

[Robert Nystrom, Pratt Parsers Made Easy](https://journal.stuffwithstuff.com/2011/03/19/pratt-parsers-expression-parsing-made-easy/)

[Martin Janiczek, Demystifying Pratt Parsers](https://martin.janiczek.cz/2023/07/03/demystifying-pratt-parsers.html)
