# pijersi-engine

This project is a C++ implementation of a Pijersi game engine aimed to be used in C#/Unity.

To learn more about Pijersi:

The project contains: 
* The actual C++ engine
* SWIG stuff to wrap the engine for C# 

## Requirements

* SWIG 4.0.2+
* g++ 8.1.0+

This project has been built on Windows, but could be replicated on Linux by adjusting the build instructions in the batch files.

## Instructions

After installing the requirements, simply run ```make_all.bat```. This will generate the C++/C# sources, compile them and link them into a DLL library. C# files will also be generated to make the use of the compiled library easier.
The DLL will be found in ```/build```, and the C# files will be found in ```/wrap```. Simply copy and paste them in the C#/Unity project and they are ready to use.