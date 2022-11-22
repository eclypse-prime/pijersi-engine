#include <logic.hpp>
#include <rng.hpp>

#include <iostream>
#include <string>
#include <utility>

using namespace std;

namespace PijersiEngine::Logic
{
    // A vector associating a cell index to the indices of its 1-range neighbours
    vector<uint32_t> neighbours[45] = {
        vector<uint32_t>({1, 6, 7}),
        vector<uint32_t>({0, 2, 7, 8}),
        vector<uint32_t>({1, 3, 8, 9}),
        vector<uint32_t>({2, 4, 9, 10}),
        vector<uint32_t>({3, 5, 10, 11}),
        vector<uint32_t>({4, 11, 12}),
        vector<uint32_t>({0, 7, 13}),
        vector<uint32_t>({0, 1, 6, 8, 13, 14}),
        vector<uint32_t>({1, 2, 7, 9, 14, 15}),
        vector<uint32_t>({2, 3, 8, 10, 15, 16}),
        vector<uint32_t>({3, 4, 9, 11, 16, 17}),
        vector<uint32_t>({4, 5, 10, 12, 17, 18}),
        vector<uint32_t>({5, 11, 18}),
        vector<uint32_t>({6, 7, 14, 19, 20}),
        vector<uint32_t>({7, 8, 13, 15, 20, 21}),
        vector<uint32_t>({8, 9, 14, 16, 21, 22}),
        vector<uint32_t>({9, 10, 15, 17, 22, 23}),
        vector<uint32_t>({10, 11, 16, 18, 23, 24}),
        vector<uint32_t>({11, 12, 17, 24, 25}),
        vector<uint32_t>({13, 20, 26}),
        vector<uint32_t>({13, 14, 19, 21, 26, 27}),
        vector<uint32_t>({14, 15, 20, 22, 27, 28}),
        vector<uint32_t>({15, 16, 21, 23, 28, 29}),
        vector<uint32_t>({16, 17, 22, 24, 29, 30}),
        vector<uint32_t>({17, 18, 23, 25, 30, 31}),
        vector<uint32_t>({18, 24, 31}),
        vector<uint32_t>({19, 20, 27, 32, 33}),
        vector<uint32_t>({20, 21, 26, 28, 33, 34}),
        vector<uint32_t>({21, 22, 27, 29, 34, 35}),
        vector<uint32_t>({22, 23, 28, 30, 35, 36}),
        vector<uint32_t>({23, 24, 29, 31, 36, 37}),
        vector<uint32_t>({24, 25, 30, 37, 38}),
        vector<uint32_t>({26, 33, 39}),
        vector<uint32_t>({26, 27, 32, 34, 39, 40}),
        vector<uint32_t>({27, 28, 33, 35, 40, 41}),
        vector<uint32_t>({28, 29, 34, 36, 41, 42}),
        vector<uint32_t>({29, 30, 35, 37, 42, 43}),
        vector<uint32_t>({30, 31, 36, 38, 43, 44}),
        vector<uint32_t>({31, 37, 44}),
        vector<uint32_t>({32, 33, 40}),
        vector<uint32_t>({33, 34, 39, 41}),
        vector<uint32_t>({34, 35, 40, 42}),
        vector<uint32_t>({35, 36, 41, 43}),
        vector<uint32_t>({36, 37, 42, 44}),
        vector<uint32_t>({37, 38, 43})};

    // A vector associating a cell index to the indices of its 2-range neighbours
    vector<uint32_t> neighbours2[45] = {
        vector<uint32_t>({2, 14}),
        vector<uint32_t>({3, 13, 15}),
        vector<uint32_t>({0, 4, 14, 16}),
        vector<uint32_t>({1, 5, 15, 17}),
        vector<uint32_t>({2, 16, 18}),
        vector<uint32_t>({3, 17}),
        vector<uint32_t>({8, 20}),
        vector<uint32_t>({9, 19, 21}),
        vector<uint32_t>({6, 10, 20, 22}),
        vector<uint32_t>({7, 11, 21, 23}),
        vector<uint32_t>({8, 12, 22, 24}),
        vector<uint32_t>({9, 23, 25}),
        vector<uint32_t>({10, 24}),
        vector<uint32_t>({1, 15, 27}),
        vector<uint32_t>({0, 2, 16, 26, 28}),
        vector<uint32_t>({1, 3, 13, 17, 27, 29}),
        vector<uint32_t>({2, 4, 14, 18, 28, 30}),
        vector<uint32_t>({3, 5, 15, 29, 31}),
        vector<uint32_t>({4, 16, 30}),
        vector<uint32_t>({7, 21, 33}),
        vector<uint32_t>({6, 8, 22, 32, 34}),
        vector<uint32_t>({7, 9, 19, 23, 33, 35}),
        vector<uint32_t>({8, 10, 20, 24, 34, 36}),
        vector<uint32_t>({9, 11, 21, 25, 35, 37}),
        vector<uint32_t>({10, 12, 22, 36, 38}),
        vector<uint32_t>({11, 23, 37}),
        vector<uint32_t>({14, 28, 40}),
        vector<uint32_t>({13, 15, 29, 39, 41}),
        vector<uint32_t>({14, 16, 26, 30, 40, 42}),
        vector<uint32_t>({15, 17, 27, 31, 41, 43}),
        vector<uint32_t>({16, 18, 28, 42, 44}),
        vector<uint32_t>({17, 29, 43}),
        vector<uint32_t>({20, 34}),
        vector<uint32_t>({19, 21, 35}),
        vector<uint32_t>({20, 22, 32, 36}),
        vector<uint32_t>({21, 23, 33, 37}),
        vector<uint32_t>({22, 24, 34, 38}),
        vector<uint32_t>({23, 25, 35}),
        vector<uint32_t>({24, 36}),
        vector<uint32_t>({27, 41}),
        vector<uint32_t>({26, 28, 42}),
        vector<uint32_t>({27, 29, 39, 43}),
        vector<uint32_t>({28, 30, 40, 44}),
        vector<uint32_t>({29, 31, 41}),
        vector<uint32_t>({30, 42})};

    string rowLetters = "gfedcba";

    string invalidCellStringException = "Invalid character in coordinates string.";
    string invalidMoveStringException = "Invalid move string.";

    // Converts a (i, j) coordinate set to an index
    uint32_t coordsToIndex(uint32_t i, uint32_t j)
    {
        uint32_t index;
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
    Coords indexToCoords(uint32_t index)
    {
        uint32_t i = 2 * (index / 13);
        uint32_t j = index % 13;
        if (j > 5)
        {
            j -= 6;
            i += 1;
        }
        return Coords(i, j);
    }

    // Converts an index to a line number
    uint32_t indexToLine(uint32_t index)
    {
        uint32_t i = 2 * (index / 13);
        if ((index % 13) > 5)
        {
            i += 1;
        }
        return i;
    }

    inline uint32_t _concatenateMove(uint32_t indexStart, uint32_t indexMid, uint32_t indexEnd)
    {
        return (indexStart) | (indexMid << 8) | (indexEnd << 16);
    }

    inline uint32_t _concatenateHalfMove(uint32_t halfMove, uint32_t indexEnd)
    {
        return halfMove | (indexEnd << 16);
    }

    // Converts a native index into a "a1" style string
    string indexToString(uint32_t index)
    {
        Coords coords = indexToCoords(index);
        string cellString = rowLetters[coords.first] + to_string(coords.second + 1);
        return cellString;
    }

    // Converts a "a1" style string coordinate into an index
    uint32_t stringToIndex(string cellString)
    {
        uint32_t i, j;
        if (cellString.size() == 2)
        {
            switch (cellString[0])
            {
            case 'a':
                i = 6;
                break;
            case 'b':
                i = 5;
                break;
            case 'c':
                i = 4;
                break;
            case 'd':
                i = 3;
                break;
            case 'e':
                i = 2;
                break;
            case 'f':
                i = 1;
                break;
            case 'g':
                i = 0;
                break;
            default:
                throw invalid_argument(invalidCellStringException);
            }
            switch (cellString[1])
            {
            case '1':
                j = 0;
                break;
            case '2':
                j = 1;
                break;
            case '3':
                j = 2;
                break;
            case '4':
                j = 3;
                break;
            case '5':
                j = 4;
                break;
            case '6':
                j = 5;
                break;
            case '7':
                j = 6;
                break;
            default:
                throw invalid_argument(invalidCellStringException);
            }
        }
        return coordsToIndex(i, j);
    }

    // Convert a native triple-index move into the string (a1-b1=c1 style) format.
    string moveToString(uint32_t move, uint8_t cells[45])
    {
        uint32_t indexStart = move & 0x000000FF;
        uint32_t indexMid = (move >> 8) & 0x000000FF;
        uint32_t indexEnd = (move >> 16) & 0x000000FF;

        if (indexStart > 44)
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
                if (cells[indexMid] != 0 && (cells[indexMid] & 2) == (cells[indexStart] & 2))
                {
                    moveString += "-" + indexToString(indexMid) + "=" + indexToString(indexEnd);
                }
                else
                {
                    moveString += "=" + indexToString(indexMid) + "-" + indexToString(indexEnd);
                }
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
        return moveString;
    }

    // Converts a string (a1-b1=c1 style) move to the native triple-index format
    uint32_t stringToMove(string moveString, uint8_t cells[45])
    {
        vector<uint32_t> move(3, -1);
        if (moveString.size() == 5)
        {
            move[0] = stringToIndex(moveString.substr(0, 2));
            move[2] = stringToIndex(moveString.substr(3, 2));
            if (moveString[2] == '-')
            {
                if (cells[move[0]] >= 16 || ((cells[move[0]] & 2) == (cells[move[1]] & 2)))
                {
                    move[1] = move[0];
                }
            }
        }
        else if (moveString.size() == 8)
        {
            move[0] = stringToIndex(moveString.substr(0, 2));
            move[1] = stringToIndex(moveString.substr(3, 2));
            move[2] = stringToIndex(moveString.substr(6, 2));
        }
        else
        {
            throw invalid_argument(invalidMoveStringException);
        }
        return _concatenateMove(move[0], move[1], move[2]);
    }


    // Subroutine of the perft debug function that is ran by the main perft() function
    uint64_t _perftIter(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer)
    {
        if (isWin(cells))
        {
            return 0ULL;
        }
        // Get a vector of all the available moves for the current player
        vector<uint32_t> moves = availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves.size();
        if (recursionDepth == 1)
        {
            return nMoves;
        }

        uint64_t sum = 0ULL;

        uint8_t newCells[45];
        for (size_t k = 0; k < nMoves; k++)
        {
            setState(newCells, cells);
            playManual(moves[k], newCells);
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
            vector<uint32_t> moves = availablePlayerMoves(currentPlayer, cells);
            size_t nMoves = moves.size();

            uint64_t sum = 0ULL;
#pragma omp parallel for schedule(dynamic) reduction(+ \
                                                     : sum)
            for (size_t k = 0; k < nMoves; k++)
            {
                uint8_t newCells[45];
                setState(newCells, cells);
                playManual(moves[k], newCells);
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

        // Get a vector of all the available moves for the current player
        vector<uint32_t> moves = availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves.size();

        // Converts all those moves to string format
        for (size_t k = 0; k < nMoves; k++)
        {
            results.push_back(moveToString(moves[k], cells));
        }

        if (recursionDepth == 1)
        {
            return results;
        }
        else
        {

// Add the number of leaf nodes associated to the corresponding move
#pragma omp parallel for schedule(dynamic)
            for (size_t k = 0; k < nMoves; k++)
            {
                uint8_t newCells[45];
                setState(newCells, cells);
                playManual(moves[k], newCells);
                results[k] += ": " + to_string(_perftIter(recursionDepth - 1, newCells, 1 - currentPlayer));
            }
        }
        return results;
    }

    // Copy the data from origin to target
    void setState(uint8_t target[45], const uint8_t origin[45])
    {
        copy(origin, origin + 45, target);
    }

    // Plays the selected move
    void play(uint32_t indexStart, uint32_t indexMid, uint32_t indexEnd, uint8_t cells[45])
    {
        if (indexStart > 44)
        {
            return;
        }
        uint8_t movingPiece = cells[indexStart];
        if (movingPiece != 0)
        {
            // If there is no intermediate move
            if (indexMid > 44)
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

    void playManual(uint32_t move, uint8_t cells[45])
    {
        uint32_t indexStart = move & 0x000000FF;
        uint32_t indexMid = (move >> 8) & 0x000000FF;
        uint32_t indexEnd = (move >> 16) & 0x000000FF;
        play(indexStart, indexMid, indexEnd, cells);
    }

    // Generates a random move
    uint32_t ponderRandom(uint8_t cells[45], uint8_t currentPlayer)
    {
        // Get a vector of all the available moves for the current player
        vector<uint32_t> moves = availablePlayerMoves(currentPlayer, cells);

        if (moves.size() > 0)
        {
            uniform_int_distribution<int> intDistribution(0, moves.size() / 6 - 1);

            uint32_t index = intDistribution(RNG::gen);

            return moves[index];
        }
        return 0x00FFFFFF;
    }

    // Plays a random move
    uint32_t playRandom(uint8_t cells[45], uint8_t currentPlayer)
    {
        uint32_t move = ponderRandom(cells, currentPlayer);
        // Apply move
        playManual(move, cells);

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
    vector<uint32_t> availablePieceMoves(uint32_t indexStart, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];

        vector<uint32_t> moves = vector<uint32_t>();
        moves.reserve(64);

        // If the piece is not a stack
        if (movingPiece < 16)
        {
            // 1-range first action
            for (uint32_t indexMid : neighbours[indexStart])
            {
                uint32_t halfMove = indexStart | (indexMid << 8);
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (uint32_t indexEnd : neighbours2[indexMid])
                    {
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells) || ((indexStart == (indexMid + indexEnd) / 2) && isMoveValid(movingPiece, indexEnd, cells)))
                        {
                            moves.push_back(_concatenateHalfMove(halfMove, indexEnd));
                        }
                    }

                    // stack, 0/1-range move
                    for (uint32_t indexEnd : neighbours[indexMid])
                    {
                        if (isMoveValid(movingPiece, indexEnd, cells) || (indexStart == indexEnd))
                        {
                            moves.push_back(_concatenateHalfMove(halfMove, indexEnd));
                        }
                    }

                    // stack only
                    moves.push_back(_concatenateMove(indexStart, indexStart, indexMid));
                }
                // 1-range move
                if (isMoveValid(movingPiece, indexMid, cells))
                {
                    moves.push_back(_concatenateMove(indexStart, 0x000000FF, indexMid));
                }
            }
        }
        else
        {
            // 2 range first action
            for (uint32_t indexMid : neighbours2[indexStart])
            {
                uint32_t halfMove = indexStart | (indexMid << 8);
                if (isMove2Valid(movingPiece, indexStart, indexMid, cells))
                {
                    // 2-range move, stack or unstack
                    for (uint32_t indexEnd : neighbours[indexMid])
                    {
                        // 2-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves.push_back(_concatenateHalfMove(halfMove, indexEnd));
                        }

                        // 2-range move, stack
                        if (isStackValid(movingPiece, indexEnd, cells))
                        {
                            moves.push_back(_concatenateHalfMove(halfMove, indexEnd));
                        }
                    }

                    // 2-range move
                    moves.push_back(_concatenateMove(indexStart, 0x000000FF, indexMid));
                }
            }
            // 1-range first action
            for (uint32_t indexMid : neighbours[indexStart])
            {
                uint32_t halfMove = indexStart | (indexMid << 8);
                // 1-range move, [stack or unstack] optional
                if (isMoveValid(movingPiece, indexMid, cells))
                {

                    // 1-range move, stack or unstack
                    for (uint32_t indexEnd : neighbours[indexMid])
                    {
                        // 1-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves.push_back(_concatenateHalfMove(halfMove, indexEnd));
                        }

                        // 1-range move, stack
                        if (isStackValid(movingPiece, indexEnd, cells))
                        {
                            moves.push_back(_concatenateHalfMove(halfMove, indexEnd));
                        }
                    }
                    // 1-range move, unstack on starting position
                    moves.push_back(_concatenateMove(indexStart, indexMid, indexStart));

                    // 1-range move
                    moves.push_back(_concatenateMove(indexStart, 0x000000FF, indexMid));
                }
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (uint32_t indexEnd : neighbours2[indexMid])
                    {
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells))
                        {
                            moves.push_back(_concatenateHalfMove(halfMove, indexEnd));
                        }
                    }

                    // stack, 1-range move
                    for (uint32_t indexEnd : neighbours[indexMid])
                    {
                        if (isMoveValid(movingPiece, indexEnd, cells))
                        {
                            moves.push_back(_concatenateHalfMove(halfMove, indexEnd));
                        }
                    }

                    // stack only
                    moves.push_back(_concatenateMove(indexStart, indexStart, indexMid));
                }

                // unstack
                if (isUnstackValid(movingPiece, indexMid, cells))
                {
                    // unstack only
                    moves.push_back(_concatenateMove(indexStart, indexStart, indexMid));
                }
            }
        }

        return moves;
    }

    // Returns the list of possible moves for a player
    vector<uint32_t> availablePlayerMoves(uint8_t player, uint8_t cells[45])
    {
        vector<uint32_t> moves = vector<uint32_t>();
        // Reserve space in vector for optimization purposes
        moves.reserve(512);
        // Calculate possible moves
        for (int index = 0; index < 45; index++)
        {
            if (cells[index] != 0)
            {
                // Choose pieces of the current player's colour
                if ((cells[index] & 2) == (player << 1))
                {
                    vector<uint32_t> pieceMoves = availablePieceMoves(index, cells);
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
    void move(uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45])
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
    void stack(uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];
        uint8_t endPiece = cells[indexEnd];

        // If the moving piece is already on top of a stack, leave the bottom piece in the starting cell
        cells[indexStart] = (movingPiece >> 4);

        // Move the top piece to the target cell and set its new bottom piece
        cells[indexEnd] = (movingPiece & 15) + (endPiece << 4);
    }

    // Applies an unstack between chosen coordinates
    void unstack(uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45])
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
    bool isMoveValid(uint8_t movingPiece, uint32_t indexEnd, uint8_t cells[45])
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
    bool isMove2Valid(uint8_t movingPiece, uint32_t indexStart, uint32_t indexEnd, uint8_t cells[45])
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
    bool isStackValid(uint8_t movingPiece, uint32_t indexEnd, const uint8_t cells[45])
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
    bool isUnstackValid(uint8_t movingPiece, uint32_t indexEnd, uint8_t cells[45])
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