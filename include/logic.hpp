#ifndef LOGIC_H
#define LOGIC_H
#include <vector>
#include <cstdint>

using namespace std;

namespace PijersiEngine
{
    int coordsToIndex(int i, int j);
    void indexToCoords(int index, int *i, int *j);
    int indexToLine(int index);
    void _setState(uint8_t cells[45], const uint8_t newState[45]);
    void _play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd, uint8_t cells[45]);
    void _playManual(int move[6], uint8_t *cells);
    bool _checkWin(const uint8_t cells[45]);
    vector<int> _availablePieceMoves(int i, int j, uint8_t cells[45]);
    vector<int> _availablePlayerMoves(uint8_t player, uint8_t cells[45]);
    bool _canTake(uint8_t source, uint8_t target);
    void _move(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45]);
    void _stack(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45]);
    void _unstack(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45]);
    bool _isMoveValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    bool _isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd, uint8_t cells[45]);
    bool _isStackValid(uint8_t movingPiece, int indexEnd, const uint8_t cells[45]);
    bool _isUnstackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    vector<int> _neighbours(int index);
    vector<int> _neighbours2(int index);
}

#endif