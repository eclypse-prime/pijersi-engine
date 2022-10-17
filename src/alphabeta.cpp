#include <vector>
#include <cstdint>
#include <cfloat>
#include <iostream>

#include <omp.h>

#include <alphabeta.hpp>
#include <logic.hpp>
#include <rng.hpp>

using namespace std;

namespace PijersiEngine
{

    // Calculates a move using alphabeta minimax algorithm of chosen depth.
    vector<int> _ponderAlphaBeta(int recursionDepth, bool random, uint8_t cells[45], uint8_t currentPlayer)
    {

        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(currentPlayer, cells);

        if (moves.size() > 0)
        {
            int index = 0;
            int16_t *extremums = new int16_t[moves.size() / 6];

            int16_t alpha = INT16_MIN;
            int16_t beta = INT16_MAX;

            // Evaluate possible moves
            #pragma omp parallel for schedule(dynamic)
            for (int k = 0; k < moves.size() / 6; k++)
            {
                extremums[k] = _evaluateMove(moves.data() + 6 * k, recursionDepth, alpha, beta, cells, currentPlayer);
            }

            // Find best move
            if (currentPlayer == 0)
            {
                float maximum = -FLT_MAX;
                for (int k = 0; k < moves.size() / 6; k++)
                {
                    // Add randomness to separate equal moves if parameter active
                    float salt = random ? distribution(gen) : 0.f;
                    float extremum = salt + (float)extremums[k];
                    if (extremum > maximum)
                    {
                        maximum = extremum;
                        index = k;
                    }
                }
            }
            else
            {
                float minimum = FLT_MAX;
                for (int k = 0; k < moves.size() / 6; k++)
                {
                    // Add randomness to separate equal moves if parameter active
                    float salt = random ? distribution(gen) : 0.f;
                    float extremum = salt + (float)extremums[k];
                    if (extremum < minimum)
                    {
                        minimum = extremum;
                        index = k;
                    }
                }
            }

            delete extremums;

            vector<int>::const_iterator first = moves.begin() + 6 * index;
            vector<int>::const_iterator last = moves.begin() + 6 * (index + 1);
            vector<int> move(first, last);
            return move;
        }
        return vector<int>({-1, -1, -1, -1, -1, -1});
    }

    // int16_t _evaluatePiece(uint8_t piece, int i)
    // {
    //     switch (piece)
    //     {
    //     case 17:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 21:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 25:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 81:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 85:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 89:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 145:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 149:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 153:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 209:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 213:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 217:
    //         switch (i)
    //         {
    //         case 0:
    //             return 3003;
    //         case 1:
    //             return 31;
    //         case 2:
    //             return 29;
    //         case 3:
    //             return 27;
    //         case 4:
    //             return 25;
    //         case 5:
    //             return 23;
    //         case 6:
    //             return 21;
    //         default:
    //             return 0;
    //         }
    //     case 221:
    //         return 19;
    //     case 1:
    //         switch (i)
    //         {
    //         case 0:
    //             return 1500;
    //         case 1:
    //             return 14;
    //         case 2:
    //             return 13;
    //         case 3:
    //             return 12;
    //         case 4:
    //             return 11;
    //         case 5:
    //             return 10;
    //         case 6:
    //             return 9;
    //         default:
    //             return 0;
    //         }
    //     case 5:
    //         switch (i)
    //         {
    //         case 0:
    //             return 1500;
    //         case 1:
    //             return 14;
    //         case 2:
    //             return 13;
    //         case 3:
    //             return 12;
    //         case 4:
    //             return 11;
    //         case 5:
    //             return 10;
    //         case 6:
    //             return 9;
    //         default:
    //             return 0;
    //         }
    //     case 9:
    //         switch (i)
    //         {
    //         case 0:
    //             return 1500;
    //         case 1:
    //             return 14;
    //         case 2:
    //             return 13;
    //         case 3:
    //             return 12;
    //         case 4:
    //             return 11;
    //         case 5:
    //             return 10;
    //         case 6:
    //             return 9;
    //         default:
    //             return 0;
    //         }
    //     case 13:
    //         return 8;
    //     case 51:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 55:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 59:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 115:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 119:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 123:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 179:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 183:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 187:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 243:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 247:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 251:
    //         switch (i)
    //         {
    //         case 0:
    //             return -21;
    //         case 1:
    //             return -23;
    //         case 2:
    //             return -25;
    //         case 3:
    //             return -27;
    //         case 4:
    //             return -29;
    //         case 5:
    //             return -31;
    //         case 6:
    //             return -3003;
    //         default:
    //             return 0;
    //         }
    //     case 255:
    //         return -19;
    //     case 3:
    //         switch (i)
    //         {
    //         case 0:
    //             return -9;
    //         case 1:
    //             return -10;
    //         case 2:
    //             return -11;
    //         case 3:
    //             return -12;
    //         case 4:
    //             return -13;
    //         case 5:
    //             return -14;
    //         case 6:
    //             return -1500;
    //         default:
    //             return 0;
    //         }
    //     case 7:
    //         switch (i)
    //         {
    //         case 0:
    //             return -9;
    //         case 1:
    //             return -10;
    //         case 2:
    //             return -11;
    //         case 3:
    //             return -12;
    //         case 4:
    //             return -13;
    //         case 5:
    //             return -14;
    //         case 6:
    //             return -1500;
    //         default:
    //             return 0;
    //         }
    //     case 11:
    //         switch (i)
    //         {
    //         case 0:
    //             return -9;
    //         case 1:
    //             return -10;
    //         case 2:
    //             return -11;
    //         case 3:
    //             return -12;
    //         case 4:
    //             return -13;
    //         case 5:
    //             return -14;
    //         case 6:
    //             return -1500;
    //         default:
    //             return 0;
    //         }
    //     case 15:
    //         return -8;
    //     default:
    //         return 0;
    //     }
    // }

    // Evaluate piece according to its position, colour and type
    int16_t _evaluatePiece(uint8_t piece, int i)
    {

        int16_t score;
        // If the piece isn't Wise
        if ((piece & 12) != 12)
        {
            score = 15 - 12 * (piece & 2) - i;

            // If the piece is in a winning position
            if ((i == 0 && (piece & 2) == 0 ) || (i == 6 && (piece & 2) == 2))
            {
                score *= 128;
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
    int16_t _evaluatePosition(uint8_t cells[45])
    {
        int16_t score = 0;
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != 0)
            {
                score += _evaluatePiece(cells[k], indexToLine(k));
            }
        }
        return score;
    }

    int16_t _evaluatePosition(uint8_t cells[45], int16_t pieceScores[45])
    {
        int16_t totalScore = 0;
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != 0)
            {
                int score = _evaluatePiece(cells[k], indexToLine(k));
                pieceScores[k] = score;
                totalScore += score;
            }
        }
        return totalScore;
    }

    int16_t _updatePieceEval(int16_t previousPieceScore, uint8_t piece, int i)
    {
        if (piece == 0)
        {
            return -previousPieceScore;
        }
        else
        {
            return _evaluatePiece(piece, i) - previousPieceScore;
        }
    }

    int16_t _updatePositionEval(int16_t previousScore, int16_t previousPieceScores[45], uint8_t previousCells[45], uint8_t cells[45])
    {
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != previousCells[k])
            {
                previousScore += _updatePieceEval(previousPieceScores[k], cells[k], indexToLine(k));
            }
        }
        return previousScore;
    }

    // Evaluates a move by calculating the possible subsequent moves recursively
    int16_t _evaluateMove(int move[6], int recursionDepth, int16_t alpha, int16_t beta, uint8_t cells[45], uint8_t currentPlayer)
    {
        // Create a new board on which the move will be played
        uint8_t newCells[45];
        _setState(newCells, cells);
        _playManual(move, newCells);
        // Set current player to the other colour.
        currentPlayer = 1 - currentPlayer;

        int16_t score = 0;

        // Stop the recursion if a winning position is achieved
        if (_checkWin(newCells))
        {
            return _evaluatePosition(newCells);
        }

        vector<int> moves = _availablePlayerMoves(currentPlayer, newCells);

        // Evaluate available moves and find the best one
        if (moves.size() > 0)
        {
            if (recursionDepth > 1)
            {
                if (currentPlayer == 0)
                {
                    int16_t maximum = INT16_MIN;
                    for (int k = 0; k < moves.size() / 6; k++)
                    {
                        maximum = max(maximum, _evaluateMove(moves.data() + 6 * k, recursionDepth - 1, alpha, beta, newCells, currentPlayer));
                        if (maximum > beta)
                        {
                            break;
                        }
                        alpha = max(alpha, maximum);
                    }
                    score = maximum;
                }
                else
                {
                    int16_t minimum = INT16_MAX;
                    for (int k = 0; k < moves.size() / 6; k++)
                    {
                        minimum = min(minimum, _evaluateMove(moves.data() + 6 * k, recursionDepth - 1, alpha, beta, newCells, currentPlayer));
                        if (minimum < alpha)
                        {
                            break;
                        }
                        beta = min(beta, minimum);
                    }
                    score = minimum;
                }
            }
            // Recursion not needed, only applying move and evaluating
            // This can save us a lot of memory allocations
            else
            {
                uint8_t cellsBuffer[45];
                int16_t currentPieceScores[45] = {0};

                int16_t currentScore = _evaluatePosition(newCells, currentPieceScores);

                if (currentPlayer == 0)
                {
                    int16_t maximum = INT16_MIN;
                    for (int k = 0; k < moves.size() / 6; k++)
                    {
                        maximum = max(maximum, _evaluateMoveTerminal(moves.data() + 6 * k, newCells, cellsBuffer, currentScore, currentPieceScores));
                        if (maximum > beta)
                        {
                            break;
                        }
                        alpha = max(alpha, maximum);
                    }
                    score = maximum;
                }
                else
                {
                    int16_t minimum = INT16_MAX;
                    for (int k = 0; k < moves.size() / 6; k++)
                    {
                        minimum = min(minimum, _evaluateMoveTerminal(moves.data() + 6 * k, newCells, cellsBuffer, currentScore, currentPieceScores));
                        if (minimum < alpha)
                        {
                            break;
                        }
                        beta = min(beta, minimum);
                    }
                    score = minimum;
                }
            }
        }

        return score;
    }

    // Evaluation function for terminal nodes (depth 0)
    int16_t _evaluateMoveTerminal(int move[6], uint8_t cells[45], uint8_t newCells[45], int16_t previousScore, int16_t previousPieceScores[45])
    {
        _setState(newCells, cells);
        _playManual(move, newCells);

        return _updatePositionEval(previousScore, previousPieceScores, cells, newCells);
    }
}