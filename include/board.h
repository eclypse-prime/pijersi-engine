#ifndef BOARD_H
#define BOARD_H
#include <piece.h>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

namespace PijersiEngine
{
    class Board
    {
    public:
        Board();
        Board(Board &board);
        // ~Board();

        void playManual(vector<int> move);
        vector<int> playAuto(int recursionDepth);
        int evaluate();
        void setState(uint8_t newState[45]);
        void init();

        uint8_t at(int i, int j);
        void print();
        string toString();

        bool checkWin();

    private:
        uint8_t cells[45];
        PieceColour currentPlayer = White;

        void addPiece(uint8_t piece, int i, int j);
        void move(int iStart, int jStart, int iEnd, int jEnd);
        void stack(int iStart, int jStart, int iEnd, int jEnd);
        void unstack(int iStart, int jStart, int iEnd, int jEnd);
        void playManual(int move[6]);
        void play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd);

        vector<int> neighbours(int index);
        vector<int> neighbours2(int index);

        int evaluateMove(int move[6], int recursionDepth, int alpha, int beta);

        bool isMoveValid(uint8_t movingPiece, int indexEnd);
        bool isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd);
        bool isStackValid(uint8_t movingPiece, int indexEnd);
        bool isUnstackValid(uint8_t movingPiece, int indexEnd);

        vector<int> availableMoves(int i, int j);
    };
}
#endif