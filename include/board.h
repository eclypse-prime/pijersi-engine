#ifndef BOARD_H
#define BOARD_H
#include <piece.h>

namespace PijersiEngine
{
    class Board
    {
    public:
        void playManual(int move[6]);
        int *playAuto();
        int coordsToIndex(int i, int j);
        void move(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd);
        Piece *at(int i, int j);
        void addPiece(Piece *piece, int i, int j);
        void init();
        void print();

    private:
        Piece *cells[45];

        bool checkWin();
        int *availableMoves(Piece piece);
    };
    char pieceToChar(Piece *piece);
}
#endif