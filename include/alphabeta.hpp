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
    extern float predictedScore;

    uint32_t ponderAlphaBeta(int recursionDepth, bool random, uint8_t cells[45], uint8_t currentPlayer, uint32_t principalVariation, size_t side, time_point<steady_clock> finishTime = time_point<steady_clock>::max(), float *lastScores = nullptr);
    float evaluatePiece(uint8_t piece, size_t i, size_t side);
    float evaluatePosition(uint8_t cells[45], size_t side);
    float evaluatePosition(uint8_t cells[45], float pieceScores[45], size_t side);
    inline float evaluateMoveTerminal(uint32_t move, uint8_t cells[45], uint8_t currentPlayer, float previousScore, float previousPieceScores[45], size_t side);
    float evaluateMove(uint32_t move, int recursionDepth, float alpha, float beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove, size_t side);
    float evaluateMoveParallel(uint32_t move, int recursionDepth, float alpha, float beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove, size_t side);
    float updatePieceEval(float previousPieceScore, uint8_t piece, size_t i, size_t size);

    // Deprecated
    float updatePositionEval(float previousScore, uint8_t previousPieceScores, uint8_t previousCells[45], uint8_t cells[45], size_t side);

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