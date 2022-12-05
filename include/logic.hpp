#ifndef LOGIC_HPP
#define LOGIC_HPP
#include <cstdint>
#include <vector>
#include <string>
#include <utility>

using namespace std;

namespace PijersiEngine::Logic
{
    using Coords = pair<uint32_t,uint32_t>;

    uint32_t coordsToIndex(uint32_t i, uint32_t j);
    Coords indexToCoords(uint32_t index);
    uint32_t indexToLine(uint32_t index);
    
    string indexToString(uint32_t index);
    uint32_t stringToIndex(string cellString);
    
    string moveToString(uint32_t move, uint8_t cells[45]);
    uint32_t stringToMove(string moveString, uint8_t cells[45]);
    
    uint64_t perft(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer);
    vector<string> perftSplit(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer);
    
    void setState(uint8_t target[45], const uint8_t origin[45]);
    
    void play(uint32_t indexStart, uint32_t indexMid, uint32_t indexEnd, uint8_t cells[45]);
    void playManual(uint32_t move, uint8_t *cells);
    uint32_t ponderRandom(uint8_t cells[45], uint8_t currentPlayer);
    uint32_t playRandom(uint8_t cells[45], uint8_t currentPlayer);
    
    bool isWin(const uint8_t cells[45]);
    
    vector<uint32_t> availablePieceMoves(uint32_t indexStart, uint8_t cells[45]);
    vector<uint32_t> availablePlayerMoves(uint8_t player, uint8_t cells[45]);
    
    bool canTake(uint8_t source, uint8_t target);
    
    void move(uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45]);
    void stack(uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45]);
    void unstack(uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45]);
    
    bool isMoveValid(uint8_t movingPiece, uint32_t indexEnd, uint8_t cells[45]);
    bool isMove2Valid(uint8_t movingPiece, uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45]);
    bool isStackValid(uint8_t movingPiece, uint32_t indexEnd, const uint8_t cells[45]);
    bool isUnstackValid(uint8_t movingPiece, uint32_t indexEnd, uint8_t cells[45]);

    void sortMoves(vector<uint32_t> moves);
}

#endif