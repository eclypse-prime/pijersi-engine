#ifndef BOARD_H
#define BOARD_H
#include <piece.h>

namespace PijersiEngine
{
    class Board {
        public:
            void playManual(int move[6]);
            int* playAuto();
        
        private:


            bool checkWin();
            int* availableMoves(Piece piece);
    };
}
#endif