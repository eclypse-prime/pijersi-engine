%module PijersiCore

%{
    #include <board.hpp>
    #include <piece.hpp>
    #include <logic.hpp>
    using namespace PijersiEngine;
%}

%include "carrays.i"
%include "std_vector.i"
%include "stdint.i"
%array_class(int, intArray);
%array_class(uint8_t, byteArray);
namespace std {
   %template(IntVector) vector<int>;
}
%include "std_string.i"

%include <board.hpp>
%include <piece.hpp>
%include <logic.hpp>