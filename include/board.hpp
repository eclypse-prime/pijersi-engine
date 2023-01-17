#ifndef BOARD_HPP
#define BOARD_HPP
#include <piece.hpp>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

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
        uint32_t ponderAlphaBeta(int recursionDepth, bool random);
        uint32_t playAlphaBeta(int recursionDepth = 3, bool random = true);
        uint32_t playMCTS(int seconds = 10, int simulationsPerRollout = 3);
        uint32_t ponderRandom();
        uint32_t playRandom();
        uint32_t ponderMCTS(int seconds, int simulationsPerRollout);
        bool isMoveLegal(uint32_t move);
        int16_t evaluate();
        void setState(uint8_t newState[45]);
        uint8_t *getState();
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