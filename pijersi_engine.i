%module PijersiCore

%{
    #include <board.h>
    #include <piece.h>
    using namespace PijersiEngine;
%}

%include "carrays.i"
%array_class(int, intArray);

%include "std_string.i"

%include <board.h>
%include <piece.h>