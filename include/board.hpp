#ifndef BOARD_HPP
#define BOARD_HPP
#include <cstdint>
#include <string>
#include <vector>

#include <piece.hpp>

using std::string;
using std::vector;

namespace PijersiEngine
{
    class Board
    {
    public:
        Board();
        Board(Board &board);
        // ~Board();

        void playManual(vector<uint32_t> move);
        void playManual(uint32_t move);
        void playManual(string move);

        // Depth limited search

        uint32_t searchDepth(int recursionDepth, bool random, uint64_t searchTimeMilliseconds = UINT64_MAX);
        uint32_t playDepth(int recursionDepth = 3, bool random = true, uint64_t searchTimeMilliseconds = UINT64_MAX);

        // Time limited search
        
        uint32_t searchTime(bool random, uint64_t searchTimeMilliseconds = UINT64_MAX);
        uint32_t playTime(bool random = true, uint64_t searchTimeMilliseconds = UINT64_MAX);
        
        // Random search

        uint32_t searchRandom();
        uint32_t playRandom();

        // uint32_t playMCTS(int seconds = 10, int simulationsPerRollout = 3);
        // uint32_t ponderMCTS(int seconds, int simulationsPerRollout);
        
        string advice(int recursionDepth, bool random);
        bool isMoveLegal(uint32_t move);
        int16_t evaluate();

        void setState(uint8_t newState[45]);
        uint8_t *getState();
        
        void setStringState(string stateString);
        string getStringState();

        void init();

        uint8_t at(int i, int j);
        void print();
        string toString();

        bool checkWin();
        int16_t getForecast();
        uint8_t currentPlayer = 0;

    private:
        uint8_t cells[45];
        int16_t forecast = 0;

        void addPiece(uint8_t piece, int i, int j);

    };

}
#endif