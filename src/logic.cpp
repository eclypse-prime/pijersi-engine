#include <logic.hpp>
#include <lookup.hpp>
#include <rng.hpp>
#include <utils.hpp>

#include <iostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using std::array;
using std::string;
using std::vector;

namespace PijersiEngine::Logic
{
    const string rowLetters = "gfedcba";

    // Convert a char into the corresponding piece
    std::unordered_map<char, uint8_t> charToPiece = {
        {'-', 0x00U},
        {'S', 0x01U},
        {'P', 0x05U},
        {'R', 0x09U},
        {'W', 0x0DU},
        {'s', 0x03U},
        {'p', 0x07U},
        {'r', 0x0BU},
        {'w', 0x0FU}
    };

    // Convert a piece into the corresponding char
    std::unordered_map<uint8_t, char> pieceToChar =
    {
        {0x00U, '-'},
        {0x01U, 'S'},
        {0x05U, 'P'},
        {0x09U, 'R'},
        {0x0DU, 'W'},
        {0x03U, 's'},
        {0x07U, 'p'},
        {0x0BU, 'r'},
        {0x0FU, 'w'}
    };

    string invalidCellStringException = "Invalid character in coordinates string.";
    string invalidMoveStringException = "Invalid move string.";
    string invalidStateStringException = "Invalid state string.";

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

    // Converts a cell index to its associated row index
    uint32_t indexToLine[] = {
        0,
        0,
        0,
        0,
        0,
        0,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        2,
        2,
        2,
        2,
        2,
        2,
        3,
        3,
        3,
        3,
        3,
        3,
        3,
        4,
        4,
        4,
        4,
        4,
        4,
        5,
        5,
        5,
        5,
        5,
        5,
        5,
        6,
        6,
        6,
        6,
        6,
        6,
    };

    inline uint32_t _concatenateMove(uint32_t indexStart, uint32_t indexMid, uint32_t indexEnd)
    {
        return (indexStart) | (indexMid << 8) | (indexEnd << 16);
    }

    inline uint32_t _concatenateHalfMove(uint32_t halfMove, uint32_t indexEnd)
    {
        return halfMove | (indexEnd << 16);
    }

    inline uint32_t _concatenateVictory(uint32_t move)
    {
        return 0x80000000U | move;
    }

    // Converts a native index into a "a1" style string
    string indexToString(uint32_t index)
    {
        Coords coords = indexToCoords(index);
        string cellString = rowLetters[coords.first] + std::to_string(coords.second + 1);
        return cellString;
    }

    // Converts a "a1" style string coordinate into an index
    uint32_t stringToIndex(string cellString)
    {
        uint32_t i = 0xFFFFFFFFU;
        uint32_t j = 0xFFFFFFFFU;
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
                throw std::invalid_argument(invalidCellStringException);
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
                throw std::invalid_argument(invalidCellStringException);
            }
        }
        return coordsToIndex(i, j);
    }

    // Convert a native triple-index move into the string (a1-b1=c1 style) format.
    string moveToString(uint32_t move, const uint8_t cells[45])
    {
        uint32_t indexStart = move & 0x000000FFU;
        uint32_t indexMid = (move >> 8) & 0x000000FFU;
        uint32_t indexEnd = (move >> 16) & 0x000000FFU;

        if (indexStart > 44)
        {
            return string("");
        }

        string moveString = indexToString(indexStart);

        if (indexMid < 45)
        {
            if (indexStart == indexMid) {
                if (cells[indexStart] < 16)
                {
                    moveString += indexToString(indexEnd);
                }
                else
                {
                    moveString += indexToString(indexStart) + indexToString(indexEnd);
                }
            }
            else
            {
                moveString += indexToString(indexMid) + indexToString(indexEnd);
            }
        }
        else
        {
            moveString += indexToString(indexEnd);
            if (cells[indexStart] >= 16)
            {
                moveString += indexToString(indexEnd);
            }
        }
        return moveString;
    }

    // Converts a string (a1-b1=c1 style) move to the native triple-index format
    uint32_t stringToMove(string moveString, const uint8_t cells[45])
    {
        vector<uint32_t> move(3, 0x000000FF);
        if (moveString.size() == 4 || (moveString.size() == 6 && moveString[4] == '-'))
        {
            move[0] = stringToIndex(moveString.substr(0, 2));
            move[2] = stringToIndex(moveString.substr(2, 2));
            if (cells[move[2]] != 0 && ((cells[move[0]] & 2) == (cells[move[2]] & 2)))
            {
                move[1] = move[0];
            }
        }
        else if (moveString.size() == 6)
        {
            move[0] = stringToIndex(moveString.substr(0, 2));
            move[1] = stringToIndex(moveString.substr(2, 2));
            move[2] = stringToIndex(moveString.substr(4, 2));
            if (move[1] == move[2])
            {
                move[1] = 0x000000FF;
            }
        }
        else
        {
            throw std::invalid_argument(invalidMoveStringException);
        }
        return _concatenateMove(move[0], move[1], move[2]);
    }

    string cellsToString(const uint8_t cells[45])
    {
        string cellsString = "";
        for (int i = 0; i < 7; i++)
        {
            int nColumns = (i % 2 == 0) ? 6 : 7;
            int counter = 0;
            for (int j = 0; j < nColumns; j++)
            {
                uint8_t piece = cells[coordsToIndex(i,j)];
                if (piece == 0x00U)
                {
                    counter += 1;
                }
                else
                {
                    if (counter > 0)
                    {
                        cellsString += std::to_string(counter);
                        counter = 0;
                    }
                    if (piece > 16)
                    {
                        cellsString += Logic::pieceToChar[piece >> 4];
                        cellsString += Logic::pieceToChar[piece & 0x0FU];
                    }
                    else
                    {
                        cellsString += Logic::pieceToChar[piece & 0x0FU];
                        cellsString += '-';
                    }
                }
            }
            if (counter > 0)
            {
                cellsString += std::to_string(counter);
            }
            if (i < 6)
            {
                cellsString += '/';
            }
        }
        return cellsString;
    }

    void stringToCells(string cellsString, uint8_t targetCells[45])
    {
        vector<string> cellLines = Utils::split(cellsString, "/");
        if (cellLines.size() != 7)
        {
            throw std::invalid_argument(invalidStateStringException);
        }
        size_t cursor = 0;

        uint8_t newCells[45];
        for (int k = 0; k < 45; k++)
        {
            newCells[k] = 0;
        }

        for (size_t i = 0; i < cellLines.size(); i++)
        {
            size_t j = 0;
            while (j < (cellLines[i].size()))
            {
                // If cellsLines[i][j] exists in charToPiece
                if (Logic::charToPiece.find(cellLines[i][j]) != Logic::charToPiece.end())
                {
                    if (cellLines[i][j+1] != '-')
                    {
                        newCells[cursor] = Logic::charToPiece[cellLines[i][j+1]] | (Logic::charToPiece[cellLines[i][j]] << 4);
                    }
                    else
                    {
                        newCells[cursor] = Logic::charToPiece[cellLines[i][j]];
                    }
                    j += 2;
                    cursor += 1;
                }
                else
                {
                    int jump = cellLines[i][j] - '0';
                    j += 1;
                    cursor += jump;
                }
            }
        }

        setState(targetCells, newCells);
    }

    // Returns the list of possible moves for a specific piece
    uint64_t _countPieceMoves(uint32_t indexStart, const uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];

        uint64_t count = 0ULL;

        // If the piece is not a stack
        if (movingPiece < 16)
        {
            // 1-range first action
            for (size_t indexMidLoop = 7 * indexStart + 1; indexMidLoop < 7 * indexStart + Lookup::neighbours[7 * indexStart] + 1; indexMidLoop++)
            {
                uint32_t indexMid = Lookup::neighbours[indexMidLoop];
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours2[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours2[indexEndLoop];
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells) || ((indexStart == (indexMid + indexEnd) / 2) && isMoveValid(movingPiece, indexEnd, cells)))
                        {
                            count++;
                        }
                    }

                    // stack, 0/1-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours[indexEndLoop];
                        if (isMoveValid(movingPiece, indexEnd, cells) || (indexStart == indexEnd))
                        {
                            count++;
                        }
                    }

                    // stack only
                    count++;
                }
                // 1-range move
                if (isMoveValid(movingPiece, indexMid, cells))
                {
                    count++;
                }
            }
        }
        else
        {
            // 2 range first action
            for (size_t indexMidLoop = 7 * indexStart + 1; indexMidLoop < 7 * indexStart + Lookup::neighbours2[7 * indexStart] + 1; indexMidLoop++)
            {
                uint32_t indexMid = Lookup::neighbours2[indexMidLoop];
                if (isMove2Valid(movingPiece, indexStart, indexMid, cells))
                {
                    // 2-range move, stack or unstack
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours[indexEndLoop];
                        // 2-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            count++;
                        }

                        // 2-range move, stack
                        if (isStackValid(movingPiece, indexEnd, cells))
                        {
                            count++;
                        }
                    }

                    // 2-range move
                    count++;
                }
            }
            // 1-range first action
            for (size_t indexMidLoop = 7 * indexStart + 1; indexMidLoop < 7 * indexStart + Lookup::neighbours[7 * indexStart] + 1; indexMidLoop++)
            {
                uint32_t indexMid = Lookup::neighbours[indexMidLoop];
                // 1-range move, [stack or unstack] optional
                if (isMoveValid(movingPiece, indexMid, cells))
                {

                    // 1-range move, stack or unstack
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours[indexEndLoop];
                        // 1-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            count++;
                        }

                        // 1-range move, stack
                        if (isStackValid(movingPiece, indexEnd, cells))
                        {
                            count++;
                        }
                    }
                    // 1-range move, unstack on starting position
                    count++;

                    // 1-range move
                    count++;
                }
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours2[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours2[indexEndLoop];
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells))
                        {
                            count++;
                        }
                    }

                    // stack, 1-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours[indexEndLoop];
                        if (isMoveValid(movingPiece, indexEnd, cells))
                        {
                            count++;
                        }
                    }

                    // stack only
                    count++;
                }

                // unstack
                if (isUnstackValid(movingPiece, indexMid, cells))
                {
                    // unstack only
                    count++;
                }
            }
        }

        return count;
    }

    // Returns the number of possible moves for a player
    uint64_t _countPlayerMoves(uint8_t player, const uint8_t cells[45])
    {
        uint64_t count = 0ULL;
        // Calculate possible moves
        for (int index = 0; index < 45; index++)
        {
            if (cells[index] != 0)
            {
                // Choose pieces of the current player's colour
                if ((cells[index] & 2) == (player << 1))
                {
                    count += _countPieceMoves(index, cells);
                }
            }
        }
        return count;
    }

    // Subroutine of the perft debug function that is ran by the main perft() function
    uint64_t _perftIter(int recursionDepth, const uint8_t cells[45], uint8_t currentPlayer)
    {
        if (recursionDepth == 1)
        {
            return _countPlayerMoves(currentPlayer, cells);
        }
        // Get a vector of all the available moves for the current player
        array<uint32_t, MAX_PLAYER_MOVES> moves = availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves[MAX_PLAYER_MOVES - 1];

        uint64_t sum = 0ULL;

        uint8_t newCells[45];
        for (size_t k = 0; k < nMoves; k++)
        {
            if (!isMoveWin(moves[k], cells))
            {
                setState(newCells, cells);
                playManual(moves[k], newCells);
                sum += _perftIter(recursionDepth - 1, newCells, 1 - currentPlayer);
            }
        }
        return sum;
    }

    // Perft debug function to measure the number of leaf nodes (possible moves) at a given depth
    uint64_t perft(int recursionDepth, uint8_t const cells[45], uint8_t currentPlayer)
    {
        if (recursionDepth == 0)
        {
            return 1ULL;
        }
        else if (isPositionWin(cells))
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
            array<uint32_t, MAX_PLAYER_MOVES> moves = availablePlayerMoves(currentPlayer, cells);
            size_t nMoves = moves[MAX_PLAYER_MOVES - 1];

            uint64_t sum = 0ULL;
            #pragma omp parallel for schedule(dynamic) reduction(+ \
                                                     : sum)
            for (size_t k = 0; k < nMoves; k++)
            {
                if (!isMoveWin(moves[k], cells))
                {
                    uint8_t newCells[45];
                    setState(newCells, cells);
                    playManual(moves[k], newCells);
                    sum += _perftIter(recursionDepth - 1, newCells, 1 - currentPlayer);
                }
            }
            return sum;
        }
    }

    vector<string> perftSplit(int recursionDepth, const uint8_t cells[45], uint8_t currentPlayer)
    {
        vector<string> results;
        if (recursionDepth == 0 || isPositionWin(cells))
        {
            return results;
        }

        results.reserve(256);

        // Get a vector of all the available moves for the current player
        array<uint32_t, MAX_PLAYER_MOVES> moves = availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves[MAX_PLAYER_MOVES - 1];

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
                if (!isMoveWin(moves[k], cells))
                {
                    uint8_t newCells[45];
                    setState(newCells, cells);
                    playManual(moves[k], newCells);
                    results[k] += ": " + std::to_string(_perftIter(recursionDepth - 1, newCells, 1 - currentPlayer));
                }
            }
        }
        return results;
    }

    // Copy the data from origin to target
    void setState(uint8_t target[45], const uint8_t origin[45])
    {
        std::copy(origin, origin + 45, target);
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
    uint32_t searchRandom(const uint8_t cells[45], uint8_t currentPlayer)
    {
        // Get a vector of all the available moves for the current player
        array<uint32_t, MAX_PLAYER_MOVES> moves = availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves[MAX_PLAYER_MOVES - 1];

        if (nMoves > 0)
        {
            std::uniform_int_distribution<int> intDistribution(0, nMoves - 1);

            uint32_t index = intDistribution(RNG::gen);

            return moves[index];
        }
        return NULL_MOVE;
    }

    // Plays a random move
    uint32_t playRandom(uint8_t cells[45], uint8_t currentPlayer)
    {
        uint32_t move = searchRandom(cells, currentPlayer);
        // Apply move
        playManual(move, cells);

        return move;
    }

    // Returns true if the board is in a winning position
    bool isPositionWin(const uint8_t cells[45])
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

    // Returns true if the move leads to a win
    bool isMoveWin(uint32_t move, const uint8_t cells[45])
    {
        size_t indexStart = move & 0x000000FF;
        size_t indexEnd = (move >> 16) & 0x000000FF;
        uint8_t movingPiece = cells[indexStart];
        
        if ((movingPiece & 12) != 12)
        {
            if (((movingPiece & 2) == 0 && (indexEnd <= 5)) || ((movingPiece & 2) == 2 && (indexEnd >= 39)))
            {
                return true;
            }
        }
        return false;
    }

    // Returns 0 if the winning player is white, 1 if black, 0xFF if no winning player
    uint8_t getWinningPlayer(const uint8_t cells[45])
    {
        for (int k = 0; k < 6; k++)
        {
            if (cells[k] != 0)
            {
                // If piece is White and not Wise
                if ((cells[k] & 2) == 0 && (cells[k] & 12) != 12)
                {
                    return 0U;
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
                    return 1U;
                }
            }
        }
        return 0xFFU;
    }

    // Returns the list of possible moves for a specific piece
    void availablePieceMoves(uint32_t indexStart, const uint8_t cells[45], array<uint32_t, MAX_PLAYER_MOVES> &moves)
    {
        uint8_t movingPiece = cells[indexStart];
        size_t indexMoves = moves[MAX_PLAYER_MOVES - 1];

        // If the piece is not a stack
        if (movingPiece < 16)
        {
            // 1-range first action
            for (size_t indexMidLoop = 7 * indexStart + 1; indexMidLoop < 7 * indexStart + Lookup::neighbours[7 * indexStart] + 1; indexMidLoop++)
            {
                uint32_t indexMid = Lookup::neighbours[indexMidLoop];
                uint32_t halfMove = indexStart | (indexMid << 8);
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours2[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours2[indexEndLoop];
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells) || ((indexStart == (indexMid + indexEnd) / 2) && isMoveValid(movingPiece, indexEnd, cells)))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }
                    }

                    // stack, 0/1-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours[indexEndLoop];
                        if (isMoveValid(movingPiece, indexEnd, cells) || (indexStart == indexEnd))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }
                    }

                    // stack only
                    moves[indexMoves] = _concatenateMove(indexStart, indexStart, indexMid);
                    indexMoves++;
                }
                // 1-range move
                if (isMoveValid(movingPiece, indexMid, cells))
                {
                    moves[indexMoves] = _concatenateMove(indexStart, 0x000000FF, indexMid);
                    indexMoves++;
                }
            }
        }
        else
        {
            // 2 range first action
            for (size_t indexMidLoop = 7 * indexStart + 1; indexMidLoop < 7 * indexStart + Lookup::neighbours2[7 * indexStart] + 1; indexMidLoop++)
            {
                uint32_t indexMid = Lookup::neighbours2[indexMidLoop];
                uint32_t halfMove = indexStart | (indexMid << 8);
                if (isMove2Valid(movingPiece, indexStart, indexMid, cells))
                {
                    // 2-range move, stack or unstack
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours[indexEndLoop];
                        // 2-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }

                        // 2-range move, stack
                        if (isStackValid(movingPiece, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }
                    }

                    // 2-range move
                    moves[indexMoves] = _concatenateMove(indexStart, 0x000000FF, indexMid);
                    indexMoves++;
                }
            }
            // 1-range first action
            for (size_t indexMidLoop = 7 * indexStart + 1; indexMidLoop < 7 * indexStart + Lookup::neighbours[7 * indexStart] + 1; indexMidLoop++)
            {
                uint32_t indexMid = Lookup::neighbours[indexMidLoop];
                uint32_t halfMove = indexStart | (indexMid << 8);
                // 1-range move, [stack or unstack] optional
                if (isMoveValid(movingPiece, indexMid, cells))
                {

                    // 1-range move, stack or unstack
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours[indexEndLoop];
                        // 1-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }

                        // 1-range move, stack
                        if (isStackValid(movingPiece, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }
                    }
                    // 1-range move, unstack on starting position
                    moves[indexMoves] = _concatenateMove(indexStart, indexMid, indexStart);
                    indexMoves++;

                    // 1-range move
                    moves[indexMoves] = _concatenateMove(indexStart, 0x000000FF, indexMid);
                    indexMoves++;
                }
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours2[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours2[indexEndLoop];
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }
                    }

                    // stack, 1-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint32_t indexEnd = Lookup::neighbours[indexEndLoop];
                        if (isMoveValid(movingPiece, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }
                    }

                    // stack only
                    moves[indexMoves] = _concatenateMove(indexStart, indexStart, indexMid);
                    indexMoves++;
                }

                // unstack
                if (isUnstackValid(movingPiece, indexMid, cells))
                {
                    // unstack only
                    moves[indexMoves] = _concatenateMove(indexStart, indexStart, indexMid);
                    indexMoves++;
                }
            }
        }

        moves[MAX_PLAYER_MOVES - 1] = indexMoves;
    }

    // Returns the list of possible moves for a player
    array<uint32_t, MAX_PLAYER_MOVES> availablePlayerMoves(uint8_t player, const uint8_t cells[45])
    {
        array<uint32_t, MAX_PLAYER_MOVES> moves;
        moves[MAX_PLAYER_MOVES - 1] = 0;

        // Calculate possible moves
        for (int index = 0; index < 45; index++)
        {
            if (cells[index] != 0)
            {
                // Choose pieces of the current player's colour
                if ((cells[index] & 2) == (player << 1))
                {
                    availablePieceMoves(index, cells, moves);
                }
            }
        }
        return moves;
    }

    // Returns whether a source piece can capture a target piece
    constexpr bool canTake(uint8_t source, uint8_t target)
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
    inline bool isMoveValid(uint8_t movingPiece, uint32_t indexEnd, const uint8_t cells[45])
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
    inline bool isMove2Valid(uint8_t movingPiece, uint32_t indexStart, uint32_t indexEnd, const uint8_t cells[45])
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
    inline bool isStackValid(uint8_t movingPiece, uint32_t indexEnd, const uint8_t cells[45])
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
    inline bool isUnstackValid(uint8_t movingPiece, uint32_t indexEnd, const uint8_t cells[45])
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

    void countMoves(uint8_t currentPlayer, const uint8_t cells[45], size_t countWhite[45], size_t countBlack[45])
    {
        for (size_t index = 0; index < 45; index++)
        {
            if (cells[index] != 0)
            {
                // Choose pieces of the current player's colour
                if (currentPlayer == 0 && (cells[index] & 2) == 0)
                {
                    countWhite[index] += _countPieceMoves(index, cells);
                }
                else if (currentPlayer == 1 && (cells[index] & 2) == 2)
                {
                    countBlack[index] += _countPieceMoves(index, cells);
                }
            }
        }
    }
}