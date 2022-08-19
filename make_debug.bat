@echo off
cd src
g++ -ggdb -fopenmp -I../include *.cpp -o ../build/debug.exe