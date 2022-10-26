#ifndef LOGIC_HPP
#define LOGIC_HPP
#include <cstdint>
#include <vector>
#include <string>
#include <utility>

using namespace std;

namespace PijersiEngine::Logic
{
    using Coords = pair<int,int>;

    int coordsToIndex(int i, int j);
    Coords indexToCoords(int index);
    int indexToLine(int index);
    string indexToString(int index);
    string moveToString(int move[3], uint8_t cells[45]);
    uint64_t perft(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer);
    vector<string> perftSplit(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer);
    void setState(uint8_t target[45], const uint8_t origin[45]);
    void play(int indexStart, int indexMid, int indexEnd, uint8_t cells[45]);
    void playManual(int move[3], uint8_t *cells);
    vector<int> ponderRandom(uint8_t cells[45], uint8_t currentPlayer);
    vector<int> playRandom(uint8_t cells[45], uint8_t currentPlayer);
    bool isWin(const uint8_t cells[45]);
    vector<int> availablePieceMoves(int indexStart, uint8_t cells[45]);
    vector<int> availablePlayerMoves(uint8_t player, uint8_t cells[45]);
    bool canTake(uint8_t source, uint8_t target);
    void move(int indexStart, int indexEnd, uint8_t cells[45]);
    void stack(int indexStart, int indexEnd, uint8_t cells[45]);
    void unstack(int indexStart, int indexEnd, uint8_t cells[45]);
    bool isMoveValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    bool isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd, uint8_t cells[45]);
    bool isStackValid(uint8_t movingPiece, int indexEnd, const uint8_t cells[45]);
    bool isUnstackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
}

#endif