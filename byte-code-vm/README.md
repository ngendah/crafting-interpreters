# Crafting Interpreters (C)

This is a small hobby project where I am implementing the interpreter as described in  
**Crafting Interpreters by Robert Nystrom**, using **C23** with **CMake**.

The goal of this implementation is educational, to better understand language design,
parsing, and interpreter implementation.

- Configure:
```
cmake -S . -B build
```

- Build:
```
cmake --build build
```

- Running tests:
```
cmake --build build --target run_tests
```
```
