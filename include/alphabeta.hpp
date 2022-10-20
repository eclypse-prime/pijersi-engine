#ifndef ALPHABETA_HPP
#define ALPHABETA_HPP
#include <vector>
#include <cstdint>

using namespace std;

namespace PijersiEngine
{
    vector<int> _ponderAlphaBeta(int recursionDepth, bool random, uint8_t cells[45], uint8_t currentPlayer);
    int16_t _evaluatePiece(uint8_t piece, int i);
    int16_t _evaluatePosition(uint8_t cells[45]);
    int16_t _evaluatePosition(uint8_t cells[45], int16_t pieceScores[45]);
    int16_t _evaluateFuturePosition(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer);
    int16_t _updatePieceEval(int16_t previousPieceScore, uint8_t piece, int i);
    int16_t _updatePositionEval(int16_t previousScore, uint8_t previousPieceScores, uint8_t previousCells[45], uint8_t cells[45]);
    int16_t _evaluateMove(int move[6], int recursionDepth, int16_t alpha, int16_t beta, uint8_t cells[45], uint8_t currentPlayer);
    int16_t _evaluateMoveTerminal(int move[6], uint8_t cells[45], uint8_t newCells[45], int16_t previousScore, int16_t previousPieceScores[45]);
}

#endif