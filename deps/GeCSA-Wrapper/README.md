# GeCSA-Wrapper

## Introdution

This project is a morder C++ wrapper of GeCSA,
the main goal is to *AVOID* lots of namespace pollution in the original project and provide an easy-to-use interface.

## Main parts

There are four main interfaces provided here:

- `BitMap` A bitwise container supporting random access.
- `GeCSA` A wrapper of GeCSA (General Compressed Suffix Array). 
- `IntArray` A packed integer container supporting
random access.
- `SparseBitMap` A read-only bitwise container with rank/select support, designed for a very sparse bitmap.

## How to build

GeCSA-Wrapper use CMake and Ninja as its build system,
use commands below in your `CMakeLists.txt` to add
it to your project:
```CMake
add_subdirectory(GeCSA-Wrapper)
target_link_libraries(YOUR-PROJECTNAME PRIVATE gecsa::gecsa)
```

## Roadmap

Unfortunately, the original project, GeCSA is not a good project, it has lots of outdated and confusing programming practices and some terriable bugs.

- [ ] Fix bug in finding left and right bounds in $\gamma$ encoding and RL-$\delta$ encoding.

- [ ] Fix memory leak in index construction.

- [ ] Fix redundant table building process in GeCSA build.


