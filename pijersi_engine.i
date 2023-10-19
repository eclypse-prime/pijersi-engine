%module PijersiCore

%{
    #include <board.hpp>
    #include <logic.hpp>
    #include <piece.hpp>
    #include <weight_optim.hpp>
    using namespace PijersiEngine;
%}

%include "carrays.i"
%include "std_vector.i"
%include "stdint.i"
%array_class(int, intArray);
%array_class(uint8_t, byteArray);
%array_class(float, floatArray);
namespace std {
   %template(IntVector) vector<int>;
}
%include "std_string.i"

%include <board.hpp>
%include <logic.hpp>
%include <piece.hpp>
%include <weight_optim.hpp>
