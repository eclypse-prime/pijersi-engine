@echo off
if not exist "build" mkdir build
cd src
g++ -c -O3 -I../include board.cpp piece.cpp pijersi_engine_wrap.cxx
g++ -shared -I../include board.o piece.o pijersi_engine_wrap.o -o ../build/PijersiEngine.dll