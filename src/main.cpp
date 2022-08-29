#include <iostream>
#include <chrono>
#include <string>

#include <board.h>
#include <piece.h>

using namespace std;
using namespace PijersiEngine;


void chronoMove(Board* board, int recursionDepth, bool tick)
{
    auto start = chrono::steady_clock::now();
    auto move = board->playAuto(recursionDepth);
    auto end = chrono::steady_clock::now();
    auto text = (tick) ? "Black :" : "White :";
    cout << text << endl;
    cout << "Elapsed time in milliseconds: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
    cout << "move: ";
    for (int elem : move)
    {
        cout << elem << " ";
    }
    cout << endl;
    board->print();
    cout << "score: " << board->evaluate() << endl << endl;
}

int main(int argc, char **argv)
{
    int recursionDepth = 3;
    int score = 0;
    bool tick = false;

    Board *board = new Board();
    board->init();
    board->print();

    cout << "score: " << board->evaluate() << endl << endl;
    
    // board->playManual(vector<int>{6,0,5,0,3,1});
    // board->print();

    // chronoMove(board, 3, false);
    // chronoMove(board, 3, true);
    // chronoMove(board, 3, false);

    // auto start = chrono::steady_clock::now();
    while (!board->checkWin())
    {
        // board->playAuto(recursionDepth);
        chronoMove(board, recursionDepth, tick);
        tick = !tick;
        // recursionDepth = (recursionDepth==3)?2:3;
    }
    // auto end = chrono::steady_clock::now();
    // cout << "Elapsed time in milliseconds: " << chrono::duration_cast<chrono::milliseconds>(end - start).count() << " ms" << endl;
    return 0;
}