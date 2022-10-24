#ifndef LOGIC_HPP
#define LOGIC_HPP
#include <cstdint>
#include <vector>
#include <string>
#include <utility>

using namespace std;

namespace PijersiEngine
{
    using Coords = pair<int,int>;

    int coordsToIndex(int i, int j);
    Coords indexToCoords(int index);
    int indexToLine(int index);
    string indexToString(int index);
    string moveToString(int move[3], uint8_t cells[45]);
    uint64_t perft(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer);
    vector<string> perftSplit(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer);
    void _setState(uint8_t target[45], const uint8_t origin[45]);
    void _play(int indexStart, int indexMid, int indexEnd, uint8_t cells[45]);
    void _playManual(int move[3], uint8_t *cells);
    vector<int> _ponderRandom(uint8_t cells[45], uint8_t currentPlayer);
    vector<int> _playRandom(uint8_t cells[45], uint8_t currentPlayer);
    bool _checkWin(const uint8_t cells[45]);
    vector<int> _availablePieceMoves(int indexStart, uint8_t cells[45]);
    vector<int> _availablePlayerMoves(uint8_t player, uint8_t cells[45]);
    bool _canTake(uint8_t source, uint8_t target);
    void _move(int indexStart, int indexEnd, uint8_t cells[45]);
    void _stack(int indexStart, int indexEnd, uint8_t cells[45]);
    void _unstack(int indexStart, int indexEnd, uint8_t cells[45]);
    bool _isMoveValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    bool _isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd, uint8_t cells[45]);
    bool _isStackValid(uint8_t movingPiece, int indexEnd, const uint8_t cells[45]);
    bool _isUnstackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
}

#endif