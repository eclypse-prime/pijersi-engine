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
        Piece *at(int i, int j);
        void addPiece(Piece *piece);
        void init();

    private:
        Piece *cells[45];

        bool checkWin();
        int *availableMoves(Piece piece);
    };
}
#endif