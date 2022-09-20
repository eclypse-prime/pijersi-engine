#ifndef BOARD_H
#define BOARD_H
#include <piece.h>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

namespace PijersiEngine
{
    void _playManual(int move[6], uint8_t *cells);
    vector<int> _availableMoves(int i, int j, uint8_t cells[45]);
    void _move(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45]);
    void _stack(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45]);
    void _unstack(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45]);
    bool _isMoveValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    bool _isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd, uint8_t cells[45]);
    bool _isStackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    bool _isUnstackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    float _evaluate(uint8_t cells[45]);
    float _evaluateMove(int move[6], int recursionDepth, float alpha, float beta, uint8_t cells[45], int currentPlayer);
    void _play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd, uint8_t cells[45]);
    vector<int> _neighbours(int index);
    vector<int> _neighbours2(int index);

    class Board
    {
    public:
        Board();
        Board(Board &board);
        // ~Board();

        void playManual(vector<int> move);
        vector<int> playAuto(int recursionDepth, bool random = true);
        float evaluate();
        void setState(uint8_t newState[45]);
        void init();

        uint8_t at(int i, int j);
        void print();
        string toString();

        bool checkWin();
        uint8_t currentPlayer = 0;

    private:
        uint8_t cells[45];
        float forecast;
        void addPiece(uint8_t piece, int i, int j);

    };

}
#endif