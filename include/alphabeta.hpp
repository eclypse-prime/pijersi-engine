#ifndef ALPHABETA_HPP
#define ALPHABETA_HPP
#include <chrono>
#include <cstdint>
#include <vector>

// NN disabled for now
// #include <nn.hpp>

#define BASE_BETA 262144
#define MAX_SCORE 524288

using std::chrono::steady_clock;
using std::chrono::time_point;

namespace PijersiEngine::AlphaBeta
{
    extern float predictedScore;

    uint32_t ponderAlphaBeta(int recursionDepth, bool random, const uint8_t cells[45], uint8_t currentPlayer, uint32_t principalVariation, time_point<steady_clock> finishTime = time_point<steady_clock>::max(), float *lastScores = nullptr, size_t side = 0);
    inline float evaluatePiece(uint8_t piece, size_t i, size_t side);
    float evaluatePosition(const uint8_t cells[45], size_t side);
    float evaluatePosition(const uint8_t cells[45], float pieceScores[45], size_t side);
    inline float evaluateMoveTerminal(uint32_t move, const uint8_t cells[45], uint8_t currentPlayer, float previousScore, float previousPieceScores[45], size_t side);
    float evaluateMove(uint32_t move, int recursionDepth, float alpha, float beta, const uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove, size_t side);

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