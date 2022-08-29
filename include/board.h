#ifndef BOARD_H
#define BOARD_H
#include <piece.h>
#include <string>
#include <vector>
#include <array>
#include <cstdint>

using namespace std;

namespace PijersiEngine
{
    class Board
    {
    public:
        Board();
        Board(Board &board);
        Board(array<float, 32> newWeights);
        Board(Board &board, array<float, 32> newWeights);
        // ~Board();

        void playManual(vector<int> move);
        vector<int> playAuto(int recursionDepth);
        float evaluate();
        void setState(uint8_t newState[45]);
        void init();

        uint8_t at(int i, int j);
        void print();
        string toString();

        bool checkWin();
        PieceColour currentPlayer = White;

        array<float, 32> weights;

    private:
        uint8_t cells[45];

        void addPiece(uint8_t piece, int i, int j);
        void move(int iStart, int jStart, int iEnd, int jEnd);
        void stack(int iStart, int jStart, int iEnd, int jEnd);
        void unstack(int iStart, int jStart, int iEnd, int jEnd);
        void playManual(int move[6]);
        void play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd);

        vector<int> neighbours(int index);
        vector<int> neighbours2(int index);

        float evaluateMove(int move[6], int recursionDepth, float alpha, float beta);
        float evaluatePiece(uint8_t piece, int i);

        bool isMoveValid(uint8_t movingPiece, int indexEnd);
        bool isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd);
        bool isStackValid(uint8_t movingPiece, int indexEnd);
        bool isUnstackValid(uint8_t movingPiece, int indexEnd);

        vector<int> availableMoves(int i, int j);
    };
}
#endif