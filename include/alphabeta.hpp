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
    extern int64_t predictedScore;

    uint32_t ponderAlphaBeta(int recursionDepth, bool random, uint8_t cells[45], uint8_t currentPlayer, uint32_t principalVariation, time_point<steady_clock> finishTime = time_point<steady_clock>::max(), int64_t *lastScores = nullptr);
    int64_t evaluatePiece(uint8_t piece, size_t i);
    int64_t evaluatePosition(uint8_t cells[45]);
    int64_t evaluatePosition(uint8_t cells[45], int64_t pieceScores[45]);
    int64_t updatePositionEval(int64_t previousScore, uint8_t previousPieceScores, uint8_t previousCells[45], uint8_t cells[45]);
    inline int64_t evaluateMoveTerminal(uint32_t move, uint8_t cells[45], uint8_t currentPlayer, int64_t previousScore, int64_t previousPieceScores[45]);
    int64_t evaluateMove(uint32_t move, int recursionDepth, int64_t alpha, int64_t beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove);
    int64_t evaluateMoveParallel(uint32_t move, int recursionDepth, int64_t alpha, int64_t beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove);

    // Deprecated
    int64_t updatePieceEval(int64_t previousPieceScore, uint8_t piece, int i);

    // NN-powered eval
    // namespace EvalNN
    // {
    //     uint32_t ponderAlphaBetaNN(int recursionDepth, bool random, uint8_t cells[45], uint8_t currentPlayer, uint32_t principalVariation, time_point<steady_clock> finishTime = time_point<steady_clock>::max());
    //     float evaluateMoveNN(uint32_t move, int recursionDepth, float alpha, float beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove);
    //     float evaluateMoveParallelNN(uint32_t move, int recursionDepth, float alpha, float beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove);
    //     inline float evaluatePositionNN(uint8_t cells[45], uint8_t currentPlayer);
    //     extern NN::Network network;
    // }
}

#endif