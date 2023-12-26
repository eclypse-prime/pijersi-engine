#ifndef BOARD_HPP
#define BOARD_HPP
#include <cstdint>
#include <string>
#include <vector>

#include <logic.hpp>
#include <piece.hpp>

#define MAX_DEPTH 10

namespace PijersiEngine
{
    class Board
    {
    public:
        Board();
        Board(Board &board);
        // ~Board();

        void playManual(std::vector<uint32_t> move);
        void playManual(uint32_t move);
        void playManual(std::string move);

        // Depth limited search

        uint32_t searchDepth(int recursionDepth, bool random, uint32_t prinvipalVariation = NULL_MOVE, uint64_t searchTimeMilliseconds = UINT64_MAX, bool iterative = true);
        uint32_t playDepth(int recursionDepth, bool random = true, uint32_t prinvipalVariation = NULL_MOVE, uint64_t searchTimeMilliseconds = UINT64_MAX, bool iterative = true);

        // Time limited search
        
        uint32_t searchTime(bool random, uint64_t searchTimeMilliseconds = UINT64_MAX);
        uint32_t playTime(bool random = true, uint64_t searchTimeMilliseconds = UINT64_MAX);
        
        // Random search

        uint32_t searchRandom();
        uint32_t playRandom();

        // uint32_t playMCTS(int seconds = 10, int simulationsPerRollout = 3);
        // uint32_t ponderMCTS(int seconds, int simulationsPerRollout);
        
        std::string advice(int recursionDepth, bool random);
        bool isMoveLegal(uint32_t move);
        int64_t evaluate();

        void setState(uint8_t newState[45]);
        uint8_t *getState();
        
        void setStringState(std::string stateString);
        std::string getStringState();

        void init();

        uint8_t at(int i, int j);
        void print();
        std::string toString();

        bool checkWin();
        bool checkDraw();
        // TODO
        bool checkStalemate();
        uint8_t getWinner();
        int64_t getPredictedScore();
        uint8_t currentPlayer = 0;

    private:
        uint32_t searchBook();

        uint32_t countPieces();
        void endTurn();

        uint8_t cells[45];

        uint32_t lastPieceCount = 0;
        uint32_t halfMoveCounter = 0;
        uint32_t moveCounter = 1;

        void addPiece(uint8_t piece, int i, int j);
    };

}
#endif