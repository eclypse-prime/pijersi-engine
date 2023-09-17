#ifndef ALPHABETA_HPP
#define ALPHABETA_HPP
#include <chrono>
#include <cstdint>
#include <vector>

#include <nn.hpp>

using std::chrono::steady_clock;
using std::chrono::time_point;

namespace PijersiEngine::AlphaBeta
{
    uint32_t ponderAlphaBeta(int recursionDepth, bool random, uint8_t cells[45], uint8_t currentPlayer, uint32_t principalVariation, time_point<steady_clock> finishTime = time_point<steady_clock>::max());
    int16_t evaluatePiece(uint8_t piece, uint32_t i);
    int16_t evaluatePosition(uint8_t cells[45]);
    int16_t evaluatePosition(uint8_t cells[45], int16_t pieceScores[45]);
    int16_t updatePieceEval(int16_t previousPieceScore, uint8_t piece, int i);
    int16_t updatePositionEval(int16_t previousScore, uint8_t previousPieceScores, uint8_t previousCells[45], uint8_t cells[45]);
    inline int16_t evaluateMoveTerminal(uint32_t move, uint8_t cells[45], uint8_t currentPlayer, int16_t previousScore, int16_t previousPieceScores[45]);
    int16_t evaluateMove(uint32_t move, int recursionDepth, int16_t alpha, int16_t beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove);
    int16_t evaluateMoveParallel(uint32_t move, int recursionDepth, int16_t alpha, int16_t beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove);

    // NN-powered eval
    uint32_t ponderAlphaBetaNN(int recursionDepth, bool random, uint8_t cells[45], uint8_t currentPlayer, uint32_t principalVariation, time_point<steady_clock> finishTime = time_point<steady_clock>::max());
    float evaluateMoveNN(uint32_t move, int recursionDepth, float alpha, float beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove);
    float evaluateMoveParallelNN(uint32_t move, int recursionDepth, float alpha, float beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove);
    inline float evaluatePositionNN(uint8_t cells[45], uint8_t currentPlayer);

    extern NN::Network network;
}

#endif