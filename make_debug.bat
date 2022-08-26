@echo off
cd src
g++ -ggdb -O3 -flto -fopenmp -I../include *.cpp -o ../build/debug.exe