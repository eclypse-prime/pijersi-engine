#include <logic.hpp>
#include <rng.hpp>

#include <iostream>
#include <string>
#include <utility>

using namespace std;

namespace PijersiEngine::Logic
{
    // A vector associating a cell index to the indices of its 1-range neighbours
    vector<int> neighbours[45] = {
        vector<int>({1, 6, 7}),
        vector<int>({0, 2, 7, 8}),
        vector<int>({1, 3, 8, 9}),
        vector<int>({2, 4, 9, 10}),
        vector<int>({3, 5, 10, 11}),
        vector<int>({4, 11, 12}),
        vector<int>({0, 7, 13}),
        vector<int>({0, 1, 6, 8, 13, 14}),
        vector<int>({1, 2, 7, 9, 14, 15}),
        vector<int>({2, 3, 8, 10, 15, 16}),
        vector<int>({3, 4, 9, 11, 16, 17}),
        vector<int>({4, 5, 10, 12, 17, 18}),
        vector<int>({5, 11, 18}),
        vector<int>({6, 7, 14, 19, 20}),
        vector<int>({7, 8, 13, 15, 20, 21}),
        vector<int>({8, 9, 14, 16, 21, 22}),
        vector<int>({9, 10, 15, 17, 22, 23}),
        vector<int>({10, 11, 16, 18, 23, 24}),
        vector<int>({11, 12, 17, 24, 25}),
        vector<int>({13, 20, 26}),
        vector<int>({13, 14, 19, 21, 26, 27}),
        vector<int>({14, 15, 20, 22, 27, 28}),
        vector<int>({15, 16, 21, 23, 28, 29}),
        vector<int>({16, 17, 22, 24, 29, 30}),
        vector<int>({17, 18, 23, 25, 30, 31}),
        vector<int>({18, 24, 31}),
        vector<int>({19, 20, 27, 32, 33}),
        vector<int>({20, 21, 26, 28, 33, 34}),
        vector<int>({21, 22, 27, 29, 34, 35}),
        vector<int>({22, 23, 28, 30, 35, 36}),
        vector<int>({23, 24, 29, 31, 36, 37}),
        vector<int>({24, 25, 30, 37, 38}),
        vector<int>({26, 33, 39}),
        vector<int>({26, 27, 32, 34, 39, 40}),
        vector<int>({27, 28, 33, 35, 40, 41}),
        vector<int>({28, 29, 34, 36, 41, 42}),
        vector<int>({29, 30, 35, 37, 42, 43}),
        vector<int>({30, 31, 36, 38, 43, 44}),
        vector<int>({31, 37, 44}),
        vector<int>({32, 33, 40}),
        vector<int>({33, 34, 39, 41}),
        vector<int>({34, 35, 40, 42}),
        vector<int>({35, 36, 41, 43}),
        vector<int>({36, 37, 42, 44}),
        vector<int>({37, 38, 43})};

    // A vector associating a cell index to the indices of its 2-range neighbours
    vector<int> neighbours2[45] = {
        vector<int>({2, 14}),
        vector<int>({3, 13, 15}),
        vector<int>({0, 4, 14, 16}),
        vector<int>({1, 5, 15, 17}),
        vector<int>({2, 16, 18}),
        vector<int>({3, 17}),
        vector<int>({8, 20}),
        vector<int>({9, 19, 21}),
        vector<int>({6, 10, 20, 22}),
        vector<int>({7, 11, 21, 23}),
        vector<int>({8, 12, 22, 24}),
        vector<int>({9, 23, 25}),
        vector<int>({10, 24}),
        vector<int>({1, 15, 27}),
        vector<int>({0, 2, 16, 26, 28}),
        vector<int>({1, 3, 13, 17, 27, 29}),
        vector<int>({2, 4, 14, 18, 28, 30}),
        vector<int>({3, 5, 15, 29, 31}),
        vector<int>({4, 16, 30}),
        vector<int>({7, 21, 33}),
        vector<int>({6, 8, 22, 32, 34}),
        vector<int>({7, 9, 19, 23, 33, 35}),
        vector<int>({8, 10, 20, 24, 34, 36}),
        vector<int>({9, 11, 21, 25, 35, 37}),
        vector<int>({10, 12, 22, 36, 38}),
        vector<int>({11, 23, 37}),
        vector<int>({14, 28, 40}),
        vector<int>({13, 15, 29, 39, 41}),
        vector<int>({14, 16, 26, 30, 40, 42}),
        vector<int>({15, 17, 27, 31, 41, 43}),
        vector<int>({16, 18, 28, 42, 44}),
        vector<int>({17, 29, 43}),
        vector<int>({20, 34}),
        vector<int>({19, 21, 35}),
        vector<int>({20, 22, 32, 36}),
        vector<int>({21, 23, 33, 37}),
        vector<int>({22, 24, 34, 38}),
        vector<int>({23, 25, 35}),
        vector<int>({24, 36}),
        vector<int>({27, 41}),
        vector<int>({26, 28, 42}),
        vector<int>({27, 29, 39, 43}),
        vector<int>({28, 30, 40, 44}),
        vector<int>({29, 31, 41}),
        vector<int>({30, 42})};

    string rowLetters = "gfedcba";

    // Converts a (i, j) coordinate set to an index
    int coordsToIndex(int i, int j)
    {
        int index;
        if (i % 2 == 0)
        {
            index = 13 * i / 2 + j;
        }
        else
        {
            index = 6 + 13 * (i - 1) / 2 + j;
        }
        return index;
    }

    // Converts an index to a (i, j) coordinate set
    Coords indexToCoords(int index)
    {
        int i = 2 * (index / 13);
        int j = index % 13;
        if (j > 5)
        {
            j -= 6;
            i += 1;
        }
        return Coords(i, j);
    }

    // Converts an index to a line number
    int indexToLine(int index)
    {
        int i = 2 * (index / 13);
        if ((index % 13) > 5)
        {
            i += 1;
        }
        return i;
    }

    string indexToString(int index)
    {
        Coords coords = indexToCoords(index);
        string cellString = rowLetters[coords.first] + to_string(coords.second + 1);
        return cellString;
    }

    string moveToString(int move[3], uint8_t cells[45])
    {
        int indexStart = move[0];
        int indexMid = move[1];
        int indexEnd = move[2];

        if (indexStart < 0)
        {
            return string("");
        }

        string moveString = indexToString(indexStart);

        if (indexMid >= 0)
        {
            if (indexMid == indexStart)
            {
                moveString += "-" + indexToString(indexEnd);
            }
            else if (cells[indexStart] >= 16)
            {
                moveString += "=" + indexToString(indexMid) + "-" + indexToString(indexEnd);
            }
            else
            {
                moveString += "-" + indexToString(indexMid) + "=" + indexToString(indexEnd);
            }
        }
        else
        {
            if (cells[indexStart] >= 16)
            {
                moveString += "=" + indexToString(indexEnd);
            }
            else
            {
                moveString += "-" + indexToString(indexEnd);
            }
        }
        // if (moveString == "f1-f1")
        // {
        //     cout << indexStart << " " << indexMid << " " << indexEnd << endl;
        // }
        return moveString;
    }

    // Subroutine of the perft debug function that is ran by the main perft() function
    uint64_t _perftIter(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer)
    {
        if (isWin(cells))
        {
            return 0ULL;
        }
        // Get a vector of all the available moves for the current player
        vector<int> moves = availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves.size() / 3;
        if (recursionDepth == 1)
        {
            return nMoves;
        }

        uint64_t sum = 0ULL;

        uint8_t newCells[45];
        for (size_t k = 0; k < nMoves; k++)
        {
            setState(newCells, cells);
            playManual(moves.data() + 3 * k, newCells);
            sum += _perftIter(recursionDepth - 1, newCells, 1 - currentPlayer);
        }
        return sum;
    }

    // Perft debug function to measure the number of leaf nodes (possible moves) at a given depth
    uint64_t perft(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer)
    {
        if (recursionDepth == 0)
        {
            return 1ULL;
        }
        else if (isWin(cells))
        {
            return 0ULL;
        }
        else if (recursionDepth == 1)
        {
            return _perftIter(1, cells, currentPlayer);
        }
        else
        {
            // Get a vector of all the available moves for the current player
            vector<int> moves = availablePlayerMoves(currentPlayer, cells);
            size_t nMoves = moves.size() / 3;

            uint64_t sum = 0ULL;
            #pragma omp parallel for schedule(dynamic) reduction(+ : sum)
            for (size_t k = 0; k < nMoves; k++)
            {
                uint8_t newCells[45];
                setState(newCells, cells);
                playManual(moves.data() + 3 * k, newCells);
                sum += _perftIter(recursionDepth - 1, newCells, 1 - currentPlayer);
            }
            return sum;
        }
    }

    vector<string> perftSplit(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer)
    {
        vector<string> results;
        if (recursionDepth == 0 || isWin(cells))
        {
            return results;
        }

        results.reserve(256);

        vector<int> moves = availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves.size() / 3;

        for (size_t k = 0; k < nMoves; k++)
        {
            results.push_back(moveToString(moves.data() + 3 * k, cells));
        }
        if (recursionDepth == 1)
        {
            return results;
        }
        else
        {
            // Get a vector of all the available moves for the current player

            #pragma omp parallel for schedule(dynamic)
            for (size_t k = 0; k < nMoves; k++)
            {
                uint8_t newCells[45];
                setState(newCells, cells);
                playManual(moves.data() + 3 * k, newCells);
                results[k] += ": " + to_string(_perftIter(recursionDepth - 1, newCells, 1 - currentPlayer));
            }
        }
        return results;
    }

    void setState(uint8_t target[45], const uint8_t origin[45])
    {
        for (int k = 0; k < 45; k++)
        {
            target[k] = origin[k];
        }
    }

    // Plays the selected move
    void play(int indexStart, int indexMid, int indexEnd, uint8_t cells[45])
    {
        if (indexStart < 0)
        {
            return;
        }
        uint8_t movingPiece = cells[indexStart];
        if (movingPiece != 0)
        {
            // If there is no intermediate move
            if (indexMid < 0)
            {
                // Simple move
                move(indexStart, indexEnd, cells);
            }
            // There is an intermediate move
            else
            {
                uint8_t midPiece = cells[indexMid];
                uint8_t endPiece = cells[indexEnd];
                // The piece at the mid coordinates is an ally : stack and move
                if (midPiece != 0 && (midPiece & 2) == (movingPiece & 2) && (indexMid != indexStart))
                {
                    stack(indexStart, indexMid, cells);
                    move(indexMid, indexEnd, cells);
                }
                // The piece at the end coordinates is an ally : move and stack
                else if (endPiece != 0 && (endPiece & 2) == (movingPiece & 2))
                {
                    move(indexStart, indexMid, cells);
                    stack(indexMid, indexEnd, cells);
                }
                // The end coordinates contain an enemy or no piece : move and unstack
                else
                {
                    move(indexStart, indexMid, cells);
                    unstack(indexMid, indexEnd, cells);
                }
            }
        }
    }

    void playManual(int move[3], uint8_t cells[45])
    {
        play(move[0], move[1], move[2], cells);
    }

    // Generates a random move
    vector<int> ponderRandom(uint8_t cells[45], uint8_t currentPlayer)
    {
        // Get a vector of all the available moves for the current player
        vector<int> moves = availablePlayerMoves(currentPlayer, cells);

        if (moves.size() > 0)
        {
            uniform_int_distribution<int> intDistribution(0, moves.size() / 6 - 1);

            int index = intDistribution(RNG::gen);

            vector<int>::const_iterator first = moves.begin() + 6 * index;
            vector<int>::const_iterator last = moves.begin() + 6 * (index + 1);
            vector<int> move(first, last);
            return move;
        }
        return vector<int>({-1, -1, -1, -1, -1, -1});
    }

    // Plays a random move
    vector<int> playRandom(uint8_t cells[45], uint8_t currentPlayer)
    {
        vector<int> move = ponderRandom(cells, currentPlayer);
        // Apply move
        playManual(move.data(), cells);

        return move;
    }

    // Returns true if the board is in a winning position
    bool isWin(const uint8_t cells[45])
    {
        for (int k = 0; k < 6; k++)
        {
            if (cells[k] != 0)
            {
                // If piece is White and not Wise
                if ((cells[k] & 2) == 0 && (cells[k] & 12) != 12)
                {
                    return true;
                }
            }
        }
        for (int k = 39; k < 45; k++)
        {
            if (cells[k] != 0)
            {
                // If piece is Black and not Wise
                if ((cells[k] & 2) == 2 && (cells[k] & 12) != 12)
                {
                    return true;
                }
            }
        }
        return false;
    }

    // Returns the list of possible moves for a specific piece
    vector<int> availablePieceMoves(int indexStart, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];

        vector<int> moves = vector<int>();
        moves.reserve(128);

        // If the piece is not a stack
        if (movingPiece < 16)
        {
            // 1-range first action
            for (int indexMid : neighbours[indexStart])
            {
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (int indexEnd : neighbours2[indexMid])
                    {
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells) || ((indexStart == (indexMid + indexEnd) / 2) && isMoveValid(movingPiece, indexEnd, cells)))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // stack, 0/1-range move
                    for (int indexEnd : neighbours[indexMid])
                    {
                        if (isMoveValid(movingPiece, indexEnd, cells) || (indexStart == indexEnd))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // stack only
                    moves.insert(moves.end(), {indexStart, indexStart, indexMid});
                }
                // 1-range move
                if (isMoveValid(movingPiece, indexMid, cells))
                {
                    moves.insert(moves.end(), {indexStart, -1, indexMid});
                }
            }
        }
        else
        {
            // 2 range first action
            for (int indexMid : neighbours2[indexStart])
            {
                if (isMove2Valid(movingPiece, indexStart, indexMid, cells))
                {
                    // 2-range move, stack or unstack
                    for (int indexEnd : neighbours[indexMid])
                    {
                        // 2-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }

                        // 2-range move, stack
                        if (isStackValid(movingPiece, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // 2-range move
                    moves.insert(moves.end(), {indexStart, -1, indexMid});
                }
            }
            // 1-range first action
            for (int indexMid : neighbours[indexStart])
            {

                // 1-range move, [stack or unstack] optional
                if (isMoveValid(movingPiece, indexMid, cells))
                {

                    // 1-range move, stack or unstack
                    for (int indexEnd : neighbours[indexMid])
                    {
                        // 1-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }

                        // 1-range move, stack
                        if (isStackValid(movingPiece, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }
                    // 1-range move, unstack on starting position
                    moves.insert(moves.end(), {indexStart, indexMid, indexStart});

                    // 1-range move
                    moves.insert(moves.end(), {indexStart, -1, indexMid});
                }
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (int indexEnd : neighbours2[indexMid])
                    {
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // stack, 1-range move
                    for (int indexEnd : neighbours[indexMid])
                    {
                        if (isMoveValid(movingPiece, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // stack only
                    moves.insert(moves.end(), {indexStart, indexStart, indexMid});
                }

                // unstack
                if (isUnstackValid(movingPiece, indexMid, cells))
                {
                    // unstack only
                    moves.insert(moves.end(), {indexStart, indexStart, indexMid});
                }
            }
        }

        return moves;
    }

    // Returns the list of possible moves for a player
    vector<int> availablePlayerMoves(uint8_t player, uint8_t cells[45])
    {
        vector<int> moves = vector<int>();
        // Reserve space in vector for optimization purposes
        moves.reserve(1024);
        // Calculate possible moves
        for (int index = 0; index < 45; index++)
        {
            if (cells[index] != 0)
            {
                // Choose pieces of the current player's colour
                if ((cells[index] & 2) == (player << 1))
                {
                    vector<int> pieceMoves = availablePieceMoves(index, cells);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                }
            }
        }
        return moves;
    }

    // Returns whether a source piece can capture a target piece
    bool canTake(uint8_t source, uint8_t target)
    {
        uint8_t sourceType = source & 12;
        uint8_t targetType = target & 12;
        // Scissors > Paper, Paper > Rock, Rock > Scissors
        if ((sourceType == 0 && targetType == 4) || (sourceType == 4 && targetType == 8) || (sourceType == 8 && targetType == 0))
        {
            return true;
        }
        return false;
    }

    // Applies a move between chosen coordinates
    void move(int indexStart, int indexEnd, uint8_t cells[45])
    {
        // Do nothing if start and end coordinate are identical
        if (indexStart != indexEnd)
        {
            // Move the piece to the target cell
            cells[indexEnd] = cells[indexStart];

            // Set the starting cell as empty
            cells[indexStart] = 0;
        }
    }

    // Applies a stack between chosen coordinates
    void stack(int indexStart, int indexEnd, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];
        uint8_t endPiece = cells[indexEnd];

        // If the moving piece is already on top of a stack, leave the bottom piece in the starting cell
        cells[indexStart] = (movingPiece >> 4);

        // Move the top piece to the target cell and set its new bottom piece
        cells[indexEnd] = (movingPiece & 15) + (endPiece << 4);
    }

    // Applies an unstack between chosen coordinates
    void unstack(int indexStart, int indexEnd, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];

        // Leave the bottom piece in the starting cell
        cells[indexStart] = (movingPiece >> 4);
        // Remove the bottom piece from the moving piece
        // Move the top piece to the target cell
        // Will overwrite the eaten piece if there is one
        cells[indexEnd] = (movingPiece & 15);
    }

    // Returns whether a certain 1-range move is possible
    bool isMoveValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45])
    {
        if (cells[indexEnd] != 0)
        {
            // If the end piece and the moving piece are the same colour
            if ((cells[indexEnd] & 2) == (movingPiece & 2))
            {
                return false;
            }
            if (!canTake(movingPiece, cells[indexEnd]))
            {
                return false;
            }
        }
        return true;
    }

    // Returns whether a certain 2-range move is possible
    bool isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd, uint8_t cells[45])
    {
        // If there is a piece blocking the move (cell between the start and end positions)
        if (cells[(indexEnd + indexStart) / 2] != 0)
        {
            return false;
        }
        if (cells[indexEnd] != 0)
        {
            // If the end piece and the moving piece are the same colour
            if ((cells[indexEnd] & 2) == (movingPiece & 2))
            {
                return false;
            }
            if (!canTake(movingPiece, cells[indexEnd]))
            {
                return false;
            }
        }
        return true;
    }

    // Returns whether a certain stack action is possible
    bool isStackValid(uint8_t movingPiece, int indexEnd, const uint8_t cells[45])
    {
        // If the end cell is not empty
        // If the target piece and the moving piece are the same colour
        // If the end piece is not a stack
        if ((cells[indexEnd] != 0) && ((cells[indexEnd] & 2) == (movingPiece & 2)) && (cells[indexEnd] < 16))
        {
            // If the upper piece is Wise and the target piece is not Wise
            if ((movingPiece & 12) == 12 && (cells[indexEnd] & 12) != 12)
            {
                return false;
            }
            return true;
        }
        return false;
    }

    // Returns whether a certain unstack action is possible
    bool isUnstackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45])
    {
        if (cells[indexEnd] != 0)
        {
            // If the cells are the same colour
            if ((cells[indexEnd] & 2) == (movingPiece & 2))
            {
                return false;
            }
            if (!canTake(movingPiece, cells[indexEnd]))
            {
                return false;
            }
        }
        return true;
    }

}