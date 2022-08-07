#ifndef BOARD_H
#define BOARD_H
#include <piece.h>

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
        void addPiece(Piece *piece, int i, int j);
        void init();
        void print();

    private:
        Piece *cells[45];

        void move(int iStart, int jStart, int iEnd, int jEnd);
        void stack(int iStart, int jStart, int iEnd, int jEnd);
        void unstack(int iStart, int jStart, int iEnd, int jEnd);
        void play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd);

        bool checkWin();
        int *availableMoves(Piece piece);
    };
    char pieceToChar(Piece *piece);
}
#endif