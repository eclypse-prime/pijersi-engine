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
        Board(Board &board);
        ~Board();

        void playManual(vector<int> move);
        vector<int> playAuto(int recursionDepth);
        int evaluate();
        void setState(int colours[45], int top[45], int bottom[45]);
        void init();

        Piece *at(int i, int j);
        void print();
        string toString();

        bool checkWin();

    private:
        Piece *cells[45];
        PieceColour currentPlayer = White;

        void addPiece(Piece *piece, int i, int j);
        void move(int iStart, int jStart, int iEnd, int jEnd);
        void stack(int iStart, int jStart, int iEnd, int jEnd);
        void unstack(int iStart, int jStart, int iEnd, int jEnd);
        void playManual(int move[6]);
        void play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd);

        vector<int> neighbours(int index);
        vector<int> neighbours2(int index);

        int evaluateMove(int move[6], int recursionDepth);

        bool isMoveValid(int indexStart, int indexEnd);
        bool isMove2Valid(int indexStart, int indexEnd);
        bool isStackValid(int indexStart, int indexEnd);
        bool isUnstackValid(int indexStart, int indexEnd);

        vector<int> availableMoves(int i, int j);
    };
    char pieceToChar(Piece *piece);
}
#endif