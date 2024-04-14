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
    int64_t predictedScore = 0;

    /* Calculates a move using alphabeta minimax algorithm of chosen depth.
    If a finish time is provided, it will search until that time point is reached.
    In that case, the function will return a null move. */
    uint64_t ponderAlphaBeta(int recursionDepth, bool random, const uint8_t cells[45], uint8_t currentPlayer, uint64_t principalVariation, time_point<steady_clock> finishTime, int64_t* lastScores)
    {

        // Get an array of all the available moves for the current player, the last element of the array is the number of available moves
        array<uint64_t, MAX_PLAYER_MOVES> moves = Logic::availablePlayerMoves(currentPlayer, cells);
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

                // Sort the order of moves to evaluate according to the last search at depth - 1
                size_t *indices = new size_t[nMoves];
                for (size_t k = 0; k < nMoves; k++){
                    indices[k] = k;
                }
                if (lastScores != nullptr && recursionDepth > 1)
                {
                    Utils::doubleSort(lastScores, indices, nMoves);
                }

                // Index of the best move
                size_t index = 0;

                // Initializing scores array
                int64_t *scores = new int64_t[nMoves];
                for (size_t k = 0; k < nMoves; k++)
                {
                    scores[k] = INT64_MIN;
                }

                // Cutoffs will happen on winning moves
                int64_t alpha = -BASE_BETA;
                int64_t beta = BASE_BETA;

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
                // On depth > 1, run the classic recursive search, with the lowest depth being parallelized
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
                        int64_t eval = -evaluateMove(moves[indices[k]], recursionDepth - 1, -alpha - 1, -alpha, cells, 1 - currentPlayer, finishTime, true);

                        // If fail high, do the search with the full window
                        if (alpha < eval && eval < beta)
                        {
                            eval = -evaluateMove(moves[indices[k]], recursionDepth - 1, -beta, -alpha, cells, 1 - currentPlayer, finishTime, true);
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
                // On depth 0, run the lightweight eval, only calculating score differences on cells that changed (incremental eval)
                else
                {
                    int64_t previousPieceScores[45] = {0};
                    int64_t previousScore = evaluatePosition(cells, previousPieceScores);
                    for (size_t k = 0; k < nMoves; k++)
                    {
                        scores[k] = -evaluateMoveTerminal(moves[k], cells, 1 - currentPlayer, previousScore, previousPieceScores);
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
                    // TODO: change this to get the first best score (since it is already sorted)
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
    inline int64_t evaluatePiece(uint8_t piece, size_t index)
    {
        return Lookup::pieceScores[Lookup::pieceToIndex[piece] * 45 + index];
    }

    // Evaluates the board
    [[nodiscard]]
    int64_t evaluatePosition(const uint8_t cells[45])
    {
        int64_t score = 0;
        for (size_t k = 0; k < 45; k++)
        {
            score += evaluatePiece(cells[k], k);
        }
        return score;
    }

    // Evaluates the board, saves the individual cell scores
    [[nodiscard]]
    int64_t evaluatePosition(const uint8_t cells[45], int64_t pieceScores[45])
    {
        int64_t totalScore = 0;
        for (size_t k = 0; k < 45; k++)
        {
            int score = evaluatePiece(cells[k], k);
            pieceScores[k] = score;
            totalScore += score;
        }
        return totalScore;
    }

    // Update a piece's score according to its last measured score, returns the difference between its current and last score
    [[nodiscard]]
    int64_t updatePieceEval(int64_t previousPieceScore, uint8_t piece, size_t i)
    {
        if (piece == 0)
        {
            return -previousPieceScore;
        }
        else
        {
            return evaluatePiece(piece, i) - previousPieceScore;
        }
    }

    // Evaluation function for terminal nodes (depth 0), only calculates cells that changed (incremental eval)
    // TODO: possible optims
    [[nodiscard]]
    inline int64_t evaluateMoveTerminal(uint64_t move, const uint8_t cells[45], uint8_t currentPlayer, int64_t previousScore, int64_t previousPieceScores[45])
    {
        size_t indexStart = move & INDEX_MASK;
        size_t indexMid = (move >> INDEX_WIDTH) & INDEX_MASK;
        size_t indexEnd = (move >> (2*INDEX_WIDTH)) & INDEX_MASK;

        if ((cells[indexStart] & TYPE_MASK) != TYPE_WISE)
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
            previousScore += evaluatePiece(cells[indexStart], indexEnd);
        }
        else
        {
            uint8_t startPiece = cells[indexStart];
            uint8_t midPiece = cells[indexMid];
            uint8_t endPiece = cells[indexEnd];
            // The piece at the mid coordinates is an ally : stack and move
            if (midPiece != 0 && (midPiece & COLOUR_MASK) == (startPiece & COLOUR_MASK) && (indexMid != indexStart))
            {
                endPiece = (startPiece & TOP_MASK) + (midPiece << HALF_PIECE_WIDTH);
                startPiece = (startPiece >> HALF_PIECE_WIDTH);
                midPiece = 0;

                // Starting cell
                previousScore -= previousPieceScores[indexStart];
                previousScore += evaluatePiece(startPiece, indexStart);

                // Middle cell
                previousScore -= previousPieceScores[indexMid];
                previousScore += evaluatePiece(midPiece, indexMid);

                // Ending cell
                if (indexStart != indexEnd)
                {
                    previousScore -= previousPieceScores[indexEnd];
                }
                previousScore += evaluatePiece(endPiece, indexEnd);
            }
            // The piece at the end coordinates is an ally : move and stack
            else if (endPiece != 0 && (endPiece & COLOUR_MASK) == (startPiece & COLOUR_MASK))
            {
                midPiece = startPiece;
                startPiece = 0;
                endPiece = (midPiece & TOP_MASK) + (endPiece << HALF_PIECE_WIDTH);
                if (indexStart == indexEnd)
                {
                    endPiece = (midPiece & TOP_MASK);
                }
                midPiece = (midPiece >> HALF_PIECE_WIDTH);

                // Starting cell
                if (indexStart != indexMid)
                {
                    previousScore -= previousPieceScores[indexStart];
                }

                // Middle cell
                previousScore -= previousPieceScores[indexMid];
                previousScore += evaluatePiece(midPiece, indexMid);

                // Ending cell
                if (indexStart != indexEnd)
                {
                    previousScore -= previousPieceScores[indexEnd];
                }
                previousScore += evaluatePiece(endPiece, indexEnd);
            }
            // The end coordinates contain an enemy or no piece : move and unstack
            else
            {
                midPiece = startPiece;
                startPiece = 0;
                endPiece = (midPiece & TOP_MASK);
                midPiece = (midPiece >> HALF_PIECE_WIDTH);
                // Starting cell
                if (indexStart != indexMid)
                {
                    previousScore -= previousPieceScores[indexStart];
                }

                // Middle cell
                previousScore -= previousPieceScores[indexMid];
                previousScore += evaluatePiece(midPiece, indexMid);

                // Ending cell
                previousScore -= previousPieceScores[indexEnd];
                previousScore += evaluatePiece(endPiece, indexEnd);
            }
        }

        return (currentPlayer == 0) ? previousScore : -previousScore;
    }

    // Evaluates a move by calculating the possible subsequent moves recursively
    int64_t evaluateMove(uint64_t move, int recursionDepth, int64_t alpha, int64_t beta, const uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove)
    {
        // Stop the recursion if a winning position is achieved
        size_t indexStart = move & INDEX_MASK;
        size_t indexEnd = (move >> (2 * INDEX_WIDTH)) & INDEX_MASK;
        if ((cells[indexStart] & TYPE_MASK) != TYPE_WISE)
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
            return (currentPlayer == 0) ? evaluatePosition(newCells) : -evaluatePosition(newCells);
        }

        array<uint64_t, MAX_PLAYER_MOVES> moves = Logic::availablePlayerMoves(currentPlayer, newCells);
        size_t nMoves = moves[MAX_PLAYER_MOVES - 1];

        int64_t score = INT64_MIN;

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
                    int64_t eval = INT64_MIN;
                    if (k==0)
                    {
                        eval = -evaluateMove(moves[k], recursionDepth - 1, -beta, -alpha, newCells, 1 - currentPlayer, finishTime, allowNullMove);
                    }
                    else
                    {
                        // Search with a null window
                        eval = -evaluateMove(moves[k], recursionDepth - 1, -alpha - 1, -alpha, newCells, 1 - currentPlayer, finishTime, allowNullMove);

                        // If fail high, do the search with the full window
                        if (alpha < eval && eval < beta)
                        {
                            eval = -evaluateMove(moves[k], recursionDepth - 1, -beta, -alpha, newCells, 1 - currentPlayer, finishTime, allowNullMove);
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
                int64_t previousPieceScores[45] = {0};
                int64_t previousScore = evaluatePosition(newCells, previousPieceScores);
                for (size_t k = 0; k < nMoves; k++)
                {
                    score = max(score, -evaluateMoveTerminal(moves[k], newCells, 1 - currentPlayer, previousScore, previousPieceScores));
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

    // Evaluates a move by calculating the possible subsequent moves recursively
    int64_t evaluateMoveParallel(uint64_t move, int recursionDepth, int64_t alpha, int64_t beta, const uint8_t cells[45], uint8_t currentPlayer, time_point<steady_clock> finishTime, bool allowNullMove)
    {
        // Stop the recursion if a winning position is achieved
        size_t indexStart = move & INDEX_MASK;
        size_t indexEnd = (move >> (2 * INDEX_WIDTH)) & INDEX_MASK;
        if ((cells[indexStart] & TYPE_MASK) != TYPE_WISE)
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
            return (currentPlayer == 0) ? evaluatePosition(newCells) : -evaluatePosition(newCells);
        }

        array<uint64_t, MAX_PLAYER_MOVES> moves = Logic::availablePlayerMoves(currentPlayer, newCells);
        size_t nMoves = moves[MAX_PLAYER_MOVES - 1];

        int64_t score = INT64_MIN;

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
                bool cut = false;
                #pragma omp parallel for schedule(dynamic) shared(alpha) if (recursionDepth > 1)
                for (size_t k = 0; k < nMoves; k++)
                {
                    if (cut)
                    {
                        continue;
                    }
                    int64_t eval = -evaluateMove(moves[k], recursionDepth - 1, -beta, -alpha, newCells, 1 - currentPlayer, finishTime, allowNullMove);
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
            else
            {
                int64_t previousPieceScores[45] = {0};
                int64_t previousScore = evaluatePosition(newCells, previousPieceScores);
                for (size_t k = 0; k < nMoves; k++)
                {
                    score = max(score, -evaluateMoveTerminal(moves[k], newCells, 1 - currentPlayer, previousScore, previousPieceScores));
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

    // Update the position's score according to the last measured position and score.
    // This will only evaluate the pieces that have changed.
    [[nodiscard]]
    [[deprecated("slower than current method")]]
    int64_t updatePositionEval(int64_t previousScore, int64_t previousPieceScores[45], uint8_t previousCells[45], uint8_t cells[45])
    {
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != previousCells[k])
            {
                previousScore += updatePieceEval(previousPieceScores[k], cells[k], k);
            }
        }
        return previousScore;
    }

    // Evaluate piece according to its position, colour and type, unused method
    [[deprecated("slower performance and lower engine strength than current implementation")]]
    inline int64_t evaluatePieceManual(uint8_t piece, uint64_t i)
    {
        int64_t score;

        // If the piece isn't Wise
        if ((piece & TYPE_MASK) != TYPE_WISE)
        {
            score = 15 - 12 * (piece & 2) - i;

            // If the piece is in a winning position
            if ((i == 0 && (piece & COLOUR_MASK) == COLOUR_WHITE) || (i == 6 && (piece & COLOUR_MASK) == COLOUR_BLACK))
            {
                score *= 256;
            }
        }
        else
        {
            score = ((piece & 2) - 1) * -8;
        }

        // If the piece is a stack
        if (piece >= 16)
        {
            score = score * 2 - 3 * ((piece & 2) - 1);
        }

        // If there is no piece, score is 0
        score *= (piece & 1);

        return score;
    }

}