# pijersi-engine

This project is a C++ implementation of a Pijersi game engine aimed to be used in C#/Unity.

To learn more about Pijersi: [GitHub](https://github.com/LucasBorboleta/pijersi).
The Unity project can be found here: [GitHub](https://github.com/arthur-liu-lsh/pijersi-unity)

The project contains: 
* The actual C++ engine
* SWIG stuff to wrap the engine for C# 

The engine is now powered by Gravity, a neural network evaluation function.

## Requirements

* SWIG 4.0.2+
* MinGW g++ 12+

This project has been built on Windows, but could be replicated on Linux by adjusting the build instructions in Makefile.

## Instructions

After installing the requirements, simply run the makefile ```mingw32-make```. This will generate the C++/C# sources, compile them and link them into a DLL library. C# files will also be generated to make the use of the compiled library easier.
The DLL and the C# files will be found in ```/wrap_csharp```. Simply copy and paste them in the C#/Unity project and they are ready to use.

## Useful data

### Perft results

Ran on 16 threads Ryzen 7 4800H. Time averaged over 5 runs (except for perft 6).

| Depth | Leaf nodes         | Time (ms) |
|-------|--------------------|-----------|
| 0     | 1                  | 0         |
| 1     | 186                | 0.0036    |
| 2     | 34,054             | 1.460     |
| 3     | 6,410,472          | 3.847     |
| 4     | 1,181,445,032      | 411.371   |
| 5     | 220,561,140,835    | 73896.22  |
| 6     | 40,310,812,241,663 | 13541600  |