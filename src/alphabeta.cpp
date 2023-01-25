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
#include <rng.hpp>

using std::chrono::steady_clock;
using std::chrono::time_point;
using std::cout;
using std::max;
using std::vector;

namespace PijersiEngine::AlphaBeta
{

    // Calculates a move using alphabeta minimax algorithm of chosen depth.
    uint32_t ponderAlphaBeta(int recursionDepth, bool random, uint8_t cells[45], uint8_t currentPlayer)
    {

        // Get a vector of all the available moves for the current player
        vector<uint32_t> moves = Logic::availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves.size();

        if (nMoves > 0)
        {
            if (recursionDepth > 0)
            {

                size_t index = 0;
                int16_t *scores = new int16_t[nMoves];

                for (size_t k = 0; k < nMoves; k++)
                {
                    scores[k] = INT16_MIN;
                }

                // Cutoffs will happen on winning moves
                int16_t alpha = -1500;
                int16_t beta = 1500;

                bool cut = false;

                // Evaluate possible moves
                #pragma omp parallel for schedule(dynamic)
                for (size_t k = 0; k < nMoves; k++)
                {
                    if (cut)
                    {
                        continue;
                    }
                    scores[k] = -evaluateMove(moves[k], recursionDepth - 1, -beta, -alpha, cells, 1 - currentPlayer);
                    if (scores[k] > beta)
                    {
                        cut = true;
                    }
                }

                // Find best move
                float maximum = -FLT_MAX;
                for (size_t k = 0; k < nMoves; k++)
                {
                    // Add randomness to separate equal moves if parameter active
                    float salt = random ? RNG::distribution(RNG::gen) : 0.f;
                    float saltedScore = salt + (float) scores[k];
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
        return 0x00FFFFFF;
    }


    // Evaluate piece according to its position, colour and type
    int16_t evaluatePiece(uint8_t piece, uint32_t i)
    {

        int16_t score;
        // If the piece isn't Wise
        if ((piece & 12) != 12)
        {
            score = 15 - 12 * (piece & 2) - i;

            // If the piece is in a winning position
            if ((i == 0 && (piece & 2) == 0 ) || (i == 6 && (piece & 2) == 2))
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
        return score;
    }

    // Evaluates the board
    int16_t evaluatePosition(uint8_t cells[45])
    {
        int16_t score = 0;
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != 0)
            {
                score += evaluatePiece(cells[k], Logic::indexToLine(k));
            }
        }
        return score;
    }

    int16_t evaluatePosition(uint8_t cells[45], int16_t pieceScores[45])
    {
        int16_t totalScore = 0;
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != 0)
            {
                int score = evaluatePiece(cells[k], Logic::indexToLine(k));
                pieceScores[k] = score;
                totalScore += score;
            }
        }
        return totalScore;
    }

    int16_t updatePieceEval(int16_t previousPieceScore, uint8_t piece, int i)
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

    int16_t updatePositionEval(int16_t previousScore, int16_t previousPieceScores[45], uint8_t previousCells[45], uint8_t cells[45])
    {
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != previousCells[k])
            {
                previousScore += updatePieceEval(previousPieceScores[k], cells[k], Logic::indexToLine(k));
            }
        }
        return previousScore;
    }

    // Evaluates a move by calculating the possible subsequent moves recursively
    int16_t evaluateMove(uint32_t move, int recursionDepth, int16_t alpha, int16_t beta, uint8_t cells[45], uint8_t currentPlayer)
    {
        // Create a new board on which the move will be played
        uint8_t newCells[45];
        Logic::setState(newCells, cells);
        Logic::playManual(move, newCells);


        // Stop the recursion if a winning position is achieved
        if (Logic::isWin(newCells) || recursionDepth == 0)
        {
            return (currentPlayer == 0) ? evaluatePosition(newCells) : -evaluatePosition(newCells);
        }

        vector<uint32_t> moves = Logic::availablePlayerMoves(currentPlayer, newCells);
        size_t nMoves = moves.size();
        
        int16_t score = INT16_MIN;

        // Evaluate available moves and find the best one
        if (moves.size() > 0)
        {
            if (recursionDepth > 1)
            {
                for (size_t k = 0; k < nMoves; k++)
                {
                    score = max(score, (int16_t)-evaluateMove(moves[k], recursionDepth - 1, -beta, -alpha, newCells, 1 - currentPlayer));
                    alpha = max(alpha, score);
                    if (alpha > beta)
                    {
                        break;
                    }
                }
            }
            else
            {
                uint8_t cellsBuffer[45];
                int16_t previousPieceScores[45] = {0};
                int16_t previousScore = evaluatePosition(newCells, previousPieceScores);
                for (size_t k = 0; k < nMoves; k++)
                {
                    int16_t evaluation = evaluateMoveTerminal(moves[k], newCells, cellsBuffer, previousScore, previousPieceScores);
                    if (currentPlayer == 1)
                    {
                        evaluation = -evaluation;
                    }
                    score = max(score, evaluation);
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

    // Evaluation function for terminal nodes (depth 0)
    int16_t evaluateMoveTerminal(uint32_t move, uint8_t cells[45], uint8_t newCells[45], int16_t previousScore, int16_t previousPieceScores[45])
    {
        Logic::setState(newCells, cells);
        Logic::playManual(move, newCells);

        return updatePositionEval(previousScore, previousPieceScores, cells, newCells);
    }
}