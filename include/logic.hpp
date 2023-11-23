#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <array>
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

#define NULL_MOVE 0x00FFFFFFU
#define MAX_PLAYER_MOVES 512

namespace PijersiEngine::Logic
{
    using Coords = std::pair<uint32_t,uint32_t>;

    // TODO: convert and move to lookup
    uint32_t coordsToIndex(uint32_t i, uint32_t j);
    Coords indexToCoords(uint32_t index);
    extern uint32_t indexToLine[45];

    extern std::unordered_map<uint8_t, char> pieceToChar;
    extern std::unordered_map<char, uint8_t> charToPiece;

    std::string indexToString(uint32_t index);
    uint32_t stringToIndex(std::string cellString);
    
    std::string moveToString(uint32_t move, uint8_t cells[45]);
    uint32_t stringToMove(std::string moveString, uint8_t cells[45]);
    
    std::string cellsToString(uint8_t cells[45]);
    void stringToCells(std::string cellsString, uint8_t targetCells[45]);
    
    uint64_t perft(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer);
    std::vector<std::string> perftSplit(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer);
    
    void setState(uint8_t target[45], const uint8_t origin[45]);
    
    void play(uint32_t indexStart, uint32_t indexMid, uint32_t indexEnd, uint8_t cells[45]);
    void playManual(uint32_t move, uint8_t *cells);
    uint32_t searchRandom(uint8_t cells[45], uint8_t currentPlayer);
    uint32_t playRandom(uint8_t cells[45], uint8_t currentPlayer);
    
    bool isWin(const uint8_t cells[45]);
    uint8_t getWinningPlayer(const uint8_t cells[45]);
    
    std::array<uint32_t, MAX_PLAYER_MOVES> availablePlayerMoves(uint8_t player, uint8_t cells[45]);
    
    bool canTake(uint8_t source, uint8_t target);
    
    void move(uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45]);
    void stack(uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45]);
    void unstack(uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45]);
    
    bool isMoveValid(uint8_t movingPiece, uint32_t indexEnd, uint8_t cells[45]);
    bool isMove2Valid(uint8_t movingPiece, uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45]);
    bool isStackValid(uint8_t movingPiece, uint32_t indexEnd, const uint8_t cells[45]);
    bool isUnstackValid(uint8_t movingPiece, uint32_t indexEnd, uint8_t cells[45]);
}

#endif