@echo off
cd src
g++ -c -fpic -I../include *.cpp
g++ -shared -I../include *.o -o ../build/lib.dll