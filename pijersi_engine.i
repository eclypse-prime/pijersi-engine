%module PijersiCore

%{
    #include <board.h>
    #include <piece.h>
    using namespace PijersiEngine;
%}

%include "carrays.i"
%include "std_vector.i"
%array_class(int, intArray);
namespace std {
   %template(IntVector) vector<int>;
}
%include "std_string.i"

%include <board.h>
%include <piece.h>