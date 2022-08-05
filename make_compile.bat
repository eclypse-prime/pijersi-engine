@echo off
cd src
g++ -c -O3 -I../include *.cpp *.cxx
g++ -shared -I../include *.o -o ../build/PijersiEngine.dll