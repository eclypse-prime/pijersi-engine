#ifndef LOGIC_HPP
#define LOGIC_HPP

#include <array>
#include <cstdint>
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

#define HALF_PIECE_WIDTH 4
#define INDEX_WIDTH 8

#define BASE_MASK 0b0001U
#define COLOUR_MASK 0b0010U
#define TYPE_MASK 0b1100U
#define TOP_MASK 0b1111U

#define COLOUR_WHITE 0b0000U
#define COLOUR_BLACK 0b0010U

#define TYPE_SCISSORS 0b0000U
#define TYPE_PAPER 0b0100U
#define TYPE_ROCK 0b1000U
#define TYPE_WISE 0b1100U


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
    
    std::string moveToString(uint32_t move, const uint8_t cells[45]);
    uint32_t stringToMove(std::string moveString, const uint8_t cells[45]);
    
    std::string cellsToString(const uint8_t cells[45]);
    void stringToCells(std::string cellsString, uint8_t targetCells[45]);
    
    uint64_t perft(int recursionDepth, const uint8_t cells[45], uint8_t currentPlayer);
    std::vector<std::string> perftSplit(int recursionDepth, const uint8_t cells[45], uint8_t currentPlayer);
    
    void setState(uint8_t target[45], const uint8_t origin[45]);
    
    void play(uint32_t indexStart, uint32_t indexMid, uint32_t indexEnd, uint8_t cells[45]);
    void playManual(uint32_t move, uint8_t *cells);
    uint32_t searchRandom(const uint8_t cells[45], uint8_t currentPlayer);
    uint32_t playRandom(uint8_t cells[45], uint8_t currentPlayer);
    
    bool isPositionWin(const uint8_t cells[45]);
    bool isMoveWin(uint32_t move, const uint8_t cells[45]);
    uint8_t getWinningPlayer(const uint8_t cells[45]);
    
    std::array<uint32_t, MAX_PLAYER_MOVES> availablePlayerMoves(const uint8_t player, const uint8_t cells[45]);
    
    constexpr bool canTake(uint8_t source, uint8_t target);
    
    inline bool isMoveValid(uint8_t movingPiece, uint32_t indexEnd, const uint8_t cells[45]);
    inline bool isMove2Valid(uint8_t movingPiece, uint32_t indexStart, uint32_t indexEnd, const uint8_t cells[45]);
    inline bool isStackValid(uint8_t movingPiece, uint32_t indexEnd, const uint8_t cells[45]);
    inline bool isUnstackValid(uint8_t movingPiece, uint32_t indexEnd, const uint8_t cells[45]);

    void countMoves(uint8_t currentPlayer, const uint8_t cells[45], size_t countWhite[45], size_t countBlack[45]);
}

#endif