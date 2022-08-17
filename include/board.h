#ifndef BOARD_H
#define BOARD_H
#include <piece.h>
#include <string>
#include <vector>

using namespace std;

namespace PijersiEngine
{
    class Board
    {
    public:
        Board();
        void playManual(int move[6]);
        int *playAuto();
        int coordsToIndex(int i, int j);
        Piece *at(int i, int j);
        int evaluate();
        void setState(int colours[45], int top[45], int bottom[45]);
        void init();
        void print();
        string toString();

    private:
        Piece *cells[45];
        PieceColour currentPlayer = White;

        void addPiece(Piece *piece, int i, int j);
        void move(int iStart, int jStart, int iEnd, int jEnd);
        void stack(int iStart, int jStart, int iEnd, int jEnd);
        void unstack(int iStart, int jStart, int iEnd, int jEnd);
        void play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd);

        bool checkWin();
        vector<int> neighbours(int i, int j);
        vector<int> neighbours(int index);
    };
    char pieceToChar(Piece *piece);
}
#endif