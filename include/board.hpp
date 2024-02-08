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

        void playManual(uint64_t move);
        void playManual(std::string move);

        // Depth limited search

        uint64_t searchDepth(int recursionDepth, bool random, uint64_t prinvipalVariation = NULL_MOVE, uint64_t searchTimeMilliseconds = UINT64_MAX, bool iterative = true);
        uint64_t playDepth(int recursionDepth, bool random = true, uint64_t prinvipalVariation = NULL_MOVE, uint64_t searchTimeMilliseconds = UINT64_MAX, bool iterative = true);

        // Time limited search
        
        uint64_t searchTime(bool random, uint64_t searchTimeMilliseconds = UINT64_MAX);
        uint64_t playTime(bool random = true, uint64_t searchTimeMilliseconds = UINT64_MAX);
        
        // Random search

        uint64_t searchRandom();
        uint64_t playRandom();

        // uint64_t playMCTS(int seconds = 10, int simulationsPerRollout = 3);
        // uint64_t ponderMCTS(int seconds, int simulationsPerRollout);
        
        std::string advice(int recursionDepth, bool random);
        bool isMoveLegal(uint64_t move);
        int64_t evaluate();
        
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
        
        uint8_t cells[45];

    private:
        uint64_t searchBook();

        uint64_t countPieces();
        void endTurn();

        uint64_t lastPieceCount = 0;
        uint64_t halfMoveCounter = 0;
        uint64_t moveCounter = 1;

        void addPiece(uint8_t piece, int i, int j);
    };

}
#endif