#include <algorithm>
#include <cstdint>
#include <cfloat>
#include <iostream>
#include <numeric>
#include <vector>
#include <chrono>

#include <omp.h>

#include <alphabeta.hpp>
#include <logic.hpp>
#include <lookup.hpp>
#include <options.hpp>
#include <rng.hpp>
#include <utils.hpp>

using std::array;
using std::cout;
using std::endl;
using std::max;
using std::vector;
using std::chrono::steady_clock;
using std::chrono::time_point;

namespace PijersiEngine::AlphaBeta
{
    float predictedScore = 0;

    /* Calculates a move using alphabeta minimax algorithm of chosen depth.
    If a finish time is provided, it will search until that time point is reached.
    In that case, the function will return a null move. */
    uint32_t ponderAlphaBeta(int recursionDepth, bool random, const uint8_t cells[45], uint8_t currentPlayer, uint32_t principalVariation, time_point<steady_clock> finishTime, float* lastScores, size_t side)
    {

        // Get a vector of all the available moves for the current player
        array<uint32_t, MAX_PLAYER_MOVES> moves = Logic::availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves[MAX_PLAYER_MOVES - 1];

        // Return a null move if time is elapsed
        if (steady_clock::now() > finishTime)
        {
            return NULL_MOVE;
        }

        if (nMoves > 0)
        {
            if (recursionDepth > 0)
            {

                // // The Principal Variation is the first move to be searched
                // if (principalVariation != NULL_MOVE)
                // {
                //     Utils::sortPrincipalVariation(moves, principalVariation);
                // }
                size_t *indices = new size_t[nMoves];
                for (size_t k = 0; k < nMoves; k++){
                    indices[k] = k;
                }
                if (lastScores != nullptr && recursionDepth > 1)
                {
                    Utils::doubleSort(lastScores, indices, nMoves);
                }

                size_t index = 0;

                // Initializing scores array
                float *scores = new float[nMoves];
                for (size_t k = 0; k < nMoves; k++)
                {
                    scores[k] = -FLT_MAX;
                }

                // Cutoffs will happen on winning moves
                float alpha = -BASE_BETA;
                float beta = BASE_BETA;

                // This will stop iteration if there is a cutoff
                bool cut = false;

                /* Search the first move first (Principal Variation), basic YBW parallel search, disabled, perf too low */
                /*scores[indices[0]] = -evaluateMoveParallel(moves[indices[0]], recursionDepth - 1, -beta, -alpha, cells, 1 - currentPlayer, finishTime, false);
                if (scores[indices[0]] > alpha)
                {
                    alpha = scores[indices[0]];
                }
                if (alpha > beta)
                {
                    cut = true;
                }*/
                if (recursionDepth > 1)
                {
                    // Evaluate possible moves
                    #pragma omp parallel for schedule(dynamic) shared (alpha) num_threads(Options::threads)
                    for (size_t k = 0; k < nMoves; k++)
                    {
                        if (cut)
                        {
                            continue;
                        }

                        // Search with a null window
                        float eval = -evaluateMove(moves[indices[k]], recursionDepth - 1, -alpha - 1, -alpha, cells, 1 - currentPlayer, finishTime, true, side);

                        // If fail high, do the search with the full window
                        if (alpha < eval && eval < beta)
                        {
                            eval = -evaluateMove(moves[indices[k]], recursionDepth - 1, -beta, -alpha, cells, 1 - currentPlayer, finishTime, true, side);
                        }

                        // Update alpha
                        #pragma omp atomic compare
                        if (eval > alpha)
                        {
                            alpha = eval;
                        }

                        // Cutoff
                        if (alpha > beta)
                        {
                            cut = true;
                        }

                        scores[indices[k]] = eval;
                    }
                }
                else
                {
                    float previousPieceScores[45] = {0};
                    float previousScore = evaluatePosition(cells, previousPieceScores, side);
                    for (size_t k = 0; k < nMoves; k++)
                    {
                        scores[k] = -evaluateMoveTerminal(moves[k], cells, 1 - currentPlayer, previousScore, previousPieceScores, side);
                        alpha = max(alpha, scores[k]);
                        if (alpha > beta)
                        {
                            break;
                        }
                    }
                }

                // Return a null move if time is elapsed
                if (steady_clock::now() > finishTime)
                {
                    delete [] indices;
                    delete [] scores;
                    return NULL_MOVE;
                }

                // Find best move
                float maximum = -FLT_MAX;
                for (size_t k = 0; k < nMoves; k++)
                {
                    // Add randomness to separate equal moves if parameter active
                    float salt = random ? RNG::distribution(RNG::gen) : 0.f;
                    float saltedScore = salt + (float)scores[k];
                    if (saltedScore > maximum)
                    {
                        maximum = saltedScore;
                        index = k;
                    }
                }
                if (lastScores != nullptr)
                {
                    for (size_t k = 0; k < nMoves; k++)
                    {
                        lastScores[k] = scores[k];
                    }
                }

                predictedScore = scores[index];

                delete [] indices;
                delete [] scores;

                return moves[index];
            }
        }
        return NULL_MOVE;
    }

    // Evaluate piece according to its position, colour and type, uses lookup table for speed
    [[nodiscard]]
    inline float evaluatePiece(uint8_t piece, size_t index, size_t side)
    {
        return Lookup::pieceScores[1575 * side + Lookup::pieceToIndex[piece] * 45 + index];
    }

    // Evaluates the board
    [[nodiscard]]
    float evaluatePosition(const uint8_t cells[45], size_t side)
    {
        float score = 0;
        for (size_t k = 0; k < 45; k++)
        {
            score += evaluatePiece(cells[k], k, side);
        }
        return score;
    }

    [[nodiscard]]
    float evaluatePosition(const uint8_t cells[45], float pieceScores[45], size_t side)
    {
        float totalScore = 0;
        for (size_t k = 0; k < 45; k++)
        {
            int score = evaluatePiece(cells[k], k, side);
            pieceScores[k] = score;
            totalScore += score;
        }
        return totalScore;
    }

    // Evaluation function for terminal nodes (depth 0)
    // TODO: possible optims
    [[nodiscard]]
    inline float evaluateMoveTerminal(uint32_t move, const uint8_t cells[45], uint8_t currentPlayer, float previousScore, float previousPieceScores[45], size_t side)
    {
        size_t indexStart = move & 0x000000FF;
        size_t indexMid = (move >> 8) & 0x000000FF;
        size_t indexEnd = (move >> 16) & 0x000000FF;

        if ((cells[indexStart] & 12) != 12)
        {
            if ((currentPlayer == 1 && (indexEnd <= 5)) || (currentPlayer == 0 && (indexEnd >= 39)))
            {
                return -MAX_SCORE;
            }
        }

        if (indexMid > 44)
        {
            // Starting cell
            previousScore -= previousPieceScores[indexStart];

            // Ending cell
            previousScore -= previousPieceScores[indexEnd];
            previousScore += evaluatePiece(cells[indexStart], indexEnd, side);
        }
        else
        {
            uint8_t startPiece = cells[indexStart];
            uint8_t midPiece = cells[indexMid];
            uint8_t endPiece = cells[indexEnd];
            // The piece at the mid coordinates is an ally : stack and move
            if (midPiece != 0 && (midPiece & 2) == (startPiece & 2) && (indexMid != indexStart))
            {
                endPiece = (startPiece & 15) + (midPiece << 4);
                startPiece = (startPiece >> 4);
                midPiece = 0;

                // Starting cell
                previousScore -= previousPieceScores[indexStart];
                previousScore += evaluatePiece(startPiece, indexStart, side);

                // Middle cell
                previousScore -= previousPieceScores[indexMid];
                previousScore += evaluatePiece(midPiece, indexMid, side);

                // Ending cell
                if (indexStart != indexEnd)
                {
                    previousScore -= previousPieceScores[indexEnd];
                }
                previousScore += evaluatePiece(endPiece, indexEnd, side);
            }
            // The piece at the end coordinates is an ally : move and stack
            else if (endPiece != 0 && (endPiece & 2) == (startPiece & 2))
            {
                midPiece = startPiece;
                startPiece = 0;
                endPiece = (midPiece & 15) + (endPiece << 4);
                if (indexStart == indexEnd)
                {
                    endPiece = (midPiece & 15);
                }
                midPiece = (midPiece >> 4);

                // Starting cell
                if (indexStart != indexMid)
                {
                    previousScore -= previousPieceScores[indexStart];
                }

                // Middle cell
                previousScore -= previousPieceScores[indexMid];
                previousScore += evaluatePiece(midPiece, indexMid, side);

                // Ending cell
                if (indexStart != indexEnd)
                {
                    previousScore -= previousPieceScores[indexEnd];
                }
                previousScore += evaluatePiece(endPiece, indexEnd, side);
            }
            // The end coordinates contain an enemy or no piece : move and unstack
            else
            {
                midPiece = startPiece;
                startPiece = 0;
                endPiece = (midPiece & 15);
                midPiece = (midPiece >> 4);
                // Starting cell
                if (indexStart != indexMid)
                {
                    previousScore -= previousPieceScores[indexStart];
                }

                // Middle cell
                previousScore -= previousPieceScores[indexMid];
                previousScore += evaluatePiece(midPiece, indexMid, side);

                // Ending cell
                previousScore -= previousPieceScores[indexEnd];
                previousScore += evaluatePiece(endPiece, indexEnd, side);
            }
        }

        return (currentPlayer == 0) ? previousScore : -previousScore;
    }

    // Evaluates a move by calculating the possible subsequent moves recursively
    float evaluateMove(uint32_t move, int recursionDepth, float alpha, float beta, const uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove, size_t side)
    {
        // Stop the recursion if a winning position is achieved
        size_t indexStart = move & 0x000000FF;
        size_t indexEnd = (move >> 16) & 0x000000FF;
        if ((cells[indexStart] & 12) != 12)
        {
            if ((currentPlayer == 1 && (indexEnd <= 5)) || (currentPlayer == 0 && (indexEnd >= 39)))
            {
                return -MAX_SCORE;
            }
        }

        // Create a new board on which the move will be played
        uint8_t newCells[45];
        Logic::setState(newCells, cells);
        Logic::playManual(move, newCells);

        if (recursionDepth <= 0)
        {
            return (currentPlayer == 0) ? evaluatePosition(newCells, side) : -evaluatePosition(newCells, side);
        }

        array<uint32_t, MAX_PLAYER_MOVES> moves = Logic::availablePlayerMoves(currentPlayer, newCells);
        size_t nMoves = moves[MAX_PLAYER_MOVES - 1];

        float score = -FLT_MAX;

        // Return a minimal score if time is elapsed
        if (steady_clock::now() > finishTime)
        {
            return score;
        }

        // Evaluate available moves and find the best one
        if (nMoves > 0)
        {
            if (recursionDepth > 1)
            {
                for (size_t k = 0; k < nMoves; k++)
                {
                    float eval = -FLT_MAX;
                    if (k==0)
                    {
                        eval = -evaluateMove(moves[k], recursionDepth - 1, -beta, -alpha, newCells, 1 - currentPlayer, finishTime, allowNullMove, side);
                    }
                    else
                    {
                        // Search with a null window
                        eval = -evaluateMove(moves[k], recursionDepth - 1, -alpha - 1, -alpha, newCells, 1 - currentPlayer, finishTime, allowNullMove, side);

                        // If fail high, do the search with the full window
                        if (alpha < eval && eval < beta)
                        {
                            eval = -evaluateMove(moves[k], recursionDepth - 1, -beta, -alpha, newCells, 1 - currentPlayer, finishTime, allowNullMove, side);
                        }
                    }
                    score = max(score, eval);
                    alpha = max(alpha, score);
                    if (alpha > beta)
                    {
                        break;
                    }
                }
            }
            else
            {
                float previousPieceScores[45] = {0};
                float previousScore = evaluatePosition(newCells, previousPieceScores, side);
                for (size_t k = 0; k < nMoves; k++)
                {
                    score = max(score, -evaluateMoveTerminal(moves[k], newCells, 1 - currentPlayer, previousScore, previousPieceScores, side));
                    alpha = max(alpha, score);
                    if (alpha > beta)
                    {
                        break;
                    }
                }
            }
        }

        return score;
    }
    // Unused NN methods
    /*
    namespace EvalNN
    {
        // The network for NN eval
        NN::Network network;

        // Calculates a move using alphabeta minimax algorithm of chosen depth.
        // If a finish time is provided, it will search until that time point is reached.
        // In that case, the function will return a null move.
        uint32_t ponderAlphaBetaNN(int recursionDepth, bool random, uint8_t cells[45], uint8_t currentPlayer, uint32_t principalVariation, time_point<steady_clock> finishTime)
        {

            // Get a vector of all the available moves for the current player
            vector<uint32_t> moves = Logic::availablePlayerMoves(currentPlayer, cells);
            size_t nMoves = moves.size();

            // Return a null move if time is elapsed
            if (steady_clock::now() > finishTime)
            {
                return NULL_MOVE;
            }

            if (nMoves > 0)
            {
                if (recursionDepth > 0)
                {

                    // The Principal Variation is the first move to be searched
                    if (principalVariation != NULL_MOVE)
                    {
                        Utils::sortPrincipalVariation(moves, principalVariation);
                    }

                    size_t index = 0;

                    // Initializing scores array
                    float *scores = new float[nMoves];
                    for (size_t k = 0; k < nMoves; k++)
                    {
                        scores[k] = -FLT_MAX;
                    }

                    // Cutoffs will happen on winning moves
                    float alpha = -0.9;
                    float beta = 0.9;

                    // This will stop iteration if there is a cutoff
                    bool cut = false;

                    // Search the first move first (Principal Variation)
                    scores[0] = -evaluateMoveParallelNN(moves[0], recursionDepth - 1, -beta, -alpha, cells, 1 - currentPlayer, finishTime, false);
                    if (scores[0] > alpha)
                    {
                        alpha = scores[0];
                    }
                    if (alpha > beta)
                    {
                        cut = true;
                    }
                    // Evaluate possible moves
                    #pragma omp parallel for schedule(dynamic) shared (alpha)
                    for (size_t k = 1; k < nMoves; k++)
                    {
                        if (cut)
                        {
                            continue;
                        }

                        // Search with a null window
                        scores[k] = -evaluateMoveNN(moves[k], recursionDepth - 1, -alpha - 1, -alpha, cells, 1 - currentPlayer, finishTime, true);

                        // If fail high, do the search with the full window
                        if (alpha < scores[k] && scores[k] < beta)
                        {
                            scores[k] = -evaluateMoveNN(moves[k], recursionDepth - 1, -beta, -alpha, cells, 1 - currentPlayer, finishTime, true);
                        }

                        // Update alpha
                        #pragma omp atomic compare
                        if (scores[k] > alpha)
                        {
                            alpha = scores[k];
                        }

                        // Cutoff
                        if (alpha > beta)
                        {
                            cut = true;
                        }
                    }

                    // Return a null move if time is elapsed
                    if (steady_clock::now() > finishTime)
                    {
                        return NULL_MOVE;
                    }

                    // Find best move
                    float maximum = -FLT_MAX;
                    for (size_t k = 0; k < nMoves; k++)
                    {
                        // Add randomness to separate equal moves if parameter active
                        float salt = random ? RNG::distribution(RNG::gen) : 0.f;
                        float saltedScore = salt + scores[k];
                        if (saltedScore > maximum)
                        {
                            maximum = saltedScore;
                            index = k;
                        }
                    }

                    delete scores;

                    return moves[index];
                }
            }
            return NULL_MOVE;
        }

        // Evaluates a move by calculating the possible subsequent moves recursively
        float evaluateMoveNN(uint32_t move, int recursionDepth, float alpha, float beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove)
        {
            // Create a new board on which the move will be played
            uint8_t newCells[45];
            Logic::setState(newCells, cells);
            Logic::playManual(move, newCells);

            // Stop the recursion if a winning position is achieved
            if (Logic::isWin(newCells))
            {
                return -2;
            }
            if (recursionDepth <= 0)
            {
                return evaluatePositionNN(newCells, currentPlayer);
            }

            vector<uint32_t> moves = Logic::availablePlayerMoves(currentPlayer, newCells);
            size_t nMoves = moves.size();

            float score = -FLT_MAX;

            // Return a minimal score if time is elapsed
            if (steady_clock::now() > finishTime)
            {
                return score;
            }

            // Evaluate available moves and find the best one
            if (moves.size() > 0)
            {
                for (size_t k = 0; k < nMoves; k++)
                {
                    score = max(score, -evaluateMoveNN(moves[k], recursionDepth - 1, -beta, -alpha, newCells, 1 - currentPlayer, finishTime, allowNullMove));
                    alpha = max(alpha, score);
                    if (alpha > beta)
                    {
                        break;
                    }
                }
            }

            return score;
        }

        // Evaluates a move by calculating the possible subsequent moves recursively
        float evaluateMoveParallelNN(uint32_t move, int recursionDepth, float alpha, float beta, uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove)
        {
            // Create a new board on which the move will be played
            uint8_t newCells[45];
            Logic::setState(newCells, cells);
            Logic::playManual(move, newCells);

            // Stop the recursion if a winning position is achieved
            if (Logic::isWin(newCells))
            {
                return -2;
            }
            if (recursionDepth <= 0)
            {
                return evaluatePositionNN(newCells, currentPlayer);
            }

            vector<uint32_t> moves = Logic::availablePlayerMoves(currentPlayer, newCells);
            size_t nMoves = moves.size();

            float score = -FLT_MAX;

            // Return a minimal score if time is elapsed
            if (steady_clock::now() > finishTime)
            {
                return score;
            }

            // Evaluate available moves and find the best one
            if (moves.size() > 0)
            {
                bool cut = false;
                #pragma omp parallel for schedule(dynamic) shared(alpha)
                for (size_t k = 0; k < nMoves; k++)
                {
                    if (cut)
                    {
                        continue;
                    }
                    float eval = -evaluateMoveNN(moves[k], recursionDepth - 1, -beta, -alpha, newCells, 1 - currentPlayer, finishTime, allowNullMove);
                    #pragma omp atomic compare
                    if (eval > score)
                    {
                        score = eval;
                    }
                    #pragma omp atomic compare
                    if (score > alpha)
                    {
                        alpha = score;
                    }
                    if (alpha > beta)
                    {
                        cut = true;
                    }
                }
            }

            return score;
        }

        inline float evaluatePositionNN(uint8_t cells[45], uint8_t currentPlayer)
        {
            return network.forward(cells, currentPlayer);
        }
    }*/

}