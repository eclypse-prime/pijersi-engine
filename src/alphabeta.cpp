#include <vector>
#include <cstdint>
#include <cfloat>
#include <iostream>

#include <omp.h>

#include <alphabeta.hpp>
#include <logic.hpp>
#include <rng.hpp>

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

    int16_t _evaluatePiece(uint8_t piece, int i)
    {
        switch (i)
        {
        case 0:
            switch (piece)
            {
            case 17:
                return 3003;
            case 21:
                return 3003;
            case 25:
                return 3003;
            case 81:
                return 3003;
            case 85:
                return 3003;
            case 89:
                return 3003;
            case 145:
                return 3003;
            case 149:
                return 3003;
            case 153:
                return 3003;
            case 209:
                return 3003;
            case 213:
                return 3003;
            case 217:
                return 3003;
            case 221:
                return 19;
            case 1:
                return 1500;
            case 5:
                return 1500;
            case 9:
                return 1500;
            case 13:
                return 8;
            case 51:
                return -21;
            case 55:
                return -21;
            case 59:
                return -21;
            case 115:
                return -21;
            case 119:
                return -21;
            case 123:
                return -21;
            case 179:
                return -21;
            case 183:
                return -21;
            case 187:
                return -21;
            case 243:
                return -21;
            case 247:
                return -21;
            case 251:
                return -21;
            case 255:
                return -19;
            case 3:
                return -9;
            case 7:
                return -9;
            case 11:
                return -9;
            case 15:
                return -8;
            default:
                return 0;
            }
        case 1:
            switch (piece)
            {
            case 17:
                return 31;
            case 21:
                return 31;
            case 25:
                return 31;
            case 81:
                return 31;
            case 85:
                return 31;
            case 89:
                return 31;
            case 145:
                return 31;
            case 149:
                return 31;
            case 153:
                return 31;
            case 209:
                return 31;
            case 213:
                return 31;
            case 217:
                return 31;
            case 221:
                return 19;
            case 1:
                return 14;
            case 5:
                return 14;
            case 9:
                return 14;
            case 13:
                return 8;
            case 51:
                return -23;
            case 55:
                return -23;
            case 59:
                return -23;
            case 115:
                return -23;
            case 119:
                return -23;
            case 123:
                return -23;
            case 179:
                return -23;
            case 183:
                return -23;
            case 187:
                return -23;
            case 243:
                return -23;
            case 247:
                return -23;
            case 251:
                return -23;
            case 255:
                return -19;
            case 3:
                return -10;
            case 7:
                return -10;
            case 11:
                return -10;
            case 15:
                return -8;
            default:
                return 0;
            }
        case 2:
            switch (piece)
            {
            case 17:
                return 29;
            case 21:
                return 29;
            case 25:
                return 29;
            case 81:
                return 29;
            case 85:
                return 29;
            case 89:
                return 29;
            case 145:
                return 29;
            case 149:
                return 29;
            case 153:
                return 29;
            case 209:
                return 29;
            case 213:
                return 29;
            case 217:
                return 29;
            case 221:
                return 19;
            case 1:
                return 13;
            case 5:
                return 13;
            case 9:
                return 13;
            case 13:
                return 8;
            case 51:
                return -25;
            case 55:
                return -25;
            case 59:
                return -25;
            case 115:
                return -25;
            case 119:
                return -25;
            case 123:
                return -25;
            case 179:
                return -25;
            case 183:
                return -25;
            case 187:
                return -25;
            case 243:
                return -25;
            case 247:
                return -25;
            case 251:
                return -25;
            case 255:
                return -19;
            case 3:
                return -11;
            case 7:
                return -11;
            case 11:
                return -11;
            case 15:
                return -8;
            default:
                return 0;
            }
        case 3:
            switch (piece)
            {
            case 17:
                return 27;
            case 21:
                return 27;
            case 25:
                return 27;
            case 81:
                return 27;
            case 85:
                return 27;
            case 89:
                return 27;
            case 145:
                return 27;
            case 149:
                return 27;
            case 153:
                return 27;
            case 209:
                return 27;
            case 213:
                return 27;
            case 217:
                return 27;
            case 221:
                return 19;
            case 1:
                return 12;
            case 5:
                return 12;
            case 9:
                return 12;
            case 13:
                return 8;
            case 51:
                return -27;
            case 55:
                return -27;
            case 59:
                return -27;
            case 115:
                return -27;
            case 119:
                return -27;
            case 123:
                return -27;
            case 179:
                return -27;
            case 183:
                return -27;
            case 187:
                return -27;
            case 243:
                return -27;
            case 247:
                return -27;
            case 251:
                return -27;
            case 255:
                return -19;
            case 3:
                return -12;
            case 7:
                return -12;
            case 11:
                return -12;
            case 15:
                return -8;
            default:
                return 0;
            }
        case 4:
            switch (piece)
            {
            case 17:
                return 25;
            case 21:
                return 25;
            case 25:
                return 25;
            case 81:
                return 25;
            case 85:
                return 25;
            case 89:
                return 25;
            case 145:
                return 25;
            case 149:
                return 25;
            case 153:
                return 25;
            case 209:
                return 25;
            case 213:
                return 25;
            case 217:
                return 25;
            case 221:
                return 19;
            case 1:
                return 11;
            case 5:
                return 11;
            case 9:
                return 11;
            case 13:
                return 8;
            case 51:
                return -29;
            case 55:
                return -29;
            case 59:
                return -29;
            case 115:
                return -29;
            case 119:
                return -29;
            case 123:
                return -29;
            case 179:
                return -29;
            case 183:
                return -29;
            case 187:
                return -29;
            case 243:
                return -29;
            case 247:
                return -29;
            case 251:
                return -29;
            case 255:
                return -19;
            case 3:
                return -13;
            case 7:
                return -13;
            case 11:
                return -13;
            case 15:
                return -8;
            default:
                return 0;
            }
        case 5:
            switch (piece)
            {
            case 17:
                return 23;
            case 21:
                return 23;
            case 25:
                return 23;
            case 81:
                return 23;
            case 85:
                return 23;
            case 89:
                return 23;
            case 145:
                return 23;
            case 149:
                return 23;
            case 153:
                return 23;
            case 209:
                return 23;
            case 213:
                return 23;
            case 217:
                return 23;
            case 221:
                return 19;
            case 1:
                return 10;
            case 5:
                return 10;
            case 9:
                return 10;
            case 13:
                return 8;
            case 51:
                return -31;
            case 55:
                return -31;
            case 59:
                return -31;
            case 115:
                return -31;
            case 119:
                return -31;
            case 123:
                return -31;
            case 179:
                return -31;
            case 183:
                return -31;
            case 187:
                return -31;
            case 243:
                return -31;
            case 247:
                return -31;
            case 251:
                return -31;
            case 255:
                return -19;
            case 3:
                return -14;
            case 7:
                return -14;
            case 11:
                return -14;
            case 15:
                return -8;
            default:
                return 0;
            }
        case 6:
            switch (piece)
            {
            case 17:
                return 21;
            case 21:
                return 21;
            case 25:
                return 21;
            case 81:
                return 21;
            case 85:
                return 21;
            case 89:
                return 21;
            case 145:
                return 21;
            case 149:
                return 21;
            case 153:
                return 21;
            case 209:
                return 21;
            case 213:
                return 21;
            case 217:
                return 21;
            case 221:
                return 19;
            case 1:
                return 9;
            case 5:
                return 9;
            case 9:
                return 9;
            case 13:
                return 8;
            case 51:
                return -3003;
            case 55:
                return -3003;
            case 59:
                return -3003;
            case 115:
                return -3003;
            case 119:
                return -3003;
            case 123:
                return -3003;
            case 179:
                return -3003;
            case 183:
                return -3003;
            case 187:
                return -3003;
            case 243:
                return -3003;
            case 247:
                return -3003;
            case 251:
                return -3003;
            case 255:
                return -19;
            case 3:
                return -1500;
            case 7:
                return -1500;
            case 11:
                return -1500;
            case 15:
                return -8;
            default:
                return 0;
            }
        default:
            return 0;
        }
    }

    // // Evaluate piece according to its position, colour and type
    // int16_t _evaluatePieceLegacy(uint8_t piece, int i)
    // {

    //     int16_t score;
    //     // If the piece isn't Wise
    //     if ((piece & 12) != 12)
    //     {
    //         score = 15 - 12 * (piece & 2) - i;

    //         // If the piece is in a winning position
    //         if ((i == 0 && (piece & 2) == 0 ) || (i == 6 && (piece & 2) == 2))
    //         {
    //             score *= 100;
    //         }
    //     }
    //     else
    //     {
    //         score = ((piece & 2) - 1) * -8;
    //     }

    //     // If the piece is a stack
    //     if (piece >= 16)
    //     {
    //         score = score * 2 - 3 * ((piece & 2) - 1);
    //     }
    //     return score;
    // }

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
                if (currentPlayer == 0)
                {
                    int16_t maximum = INT16_MIN;
                    for (int k = 0; k < moves.size() / 6; k++)
                    {
                        maximum = max(maximum, _evaluateMoveTerminal(moves.data() + 6 * k, newCells, cellsBuffer));
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
                        minimum = min(minimum, _evaluateMoveTerminal(moves.data() + 6 * k, newCells, cellsBuffer));
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
    int16_t _evaluateMoveTerminal(int move[6], uint8_t cells[45], uint8_t newCells[45])
    {
        _setState(newCells, cells);
        _playManual(move, newCells);

        return _evaluatePosition(newCells);
    }
}