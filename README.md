# pijersi-engine

This project is a C++ implementation of a Pijersi game engine aimed to be used in C#/Unity.

To learn more about Pijersi: [GitHub](https://github.com/LucasBorboleta/pijersi).
The Unity project can be found here: [GitHub](https://github.com/arthur-liu-lsh/pijersi-unity)

The project contains: 
* The actual C++ engine
* SWIG stuff to wrap the engine for C# 

## Requirements

* SWIG 4.0.2+
* MinGW g++ 8.1.0+

This project has been built on Windows, but could be replicated on Linux by adjusting the build instructions in Makefile.

## Instructions

After installing the requirements, simply run the makefile ```mingw32-make```. This will generate the C++/C# sources, compile them and link them into a DLL library. C# files will also be generated to make the use of the compiled library easier.
The DLL and the C# files will be found in ```/wrap_csharp```. Simply copy and paste them in the C#/Unity project and they are ready to use.

## Useful data

### Perft results

Averaged over 5 runs (except for depth 5).

| Depth | Leaf nodes   | Time (ms) |
|-------|--------------|-----------|
| 0     | 1            | 0.26      |
| 1     | 186          | 0.24      |
| 2     | 34054        | 2.33      |
| 3     | 6410472      | 25.49     |
| 4     | 1181614492   | 4974.4    |
| 5     | 220645877908 | 815135    |