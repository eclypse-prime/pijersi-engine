%module PijersiEngine

%{
    #include <board.h>
    #include <piece.h>
    using namespace PijersiEngine;
%}

%include "carrays.i"
%array_class(int, intArray);

%include <board.h>
%include <piece.h>