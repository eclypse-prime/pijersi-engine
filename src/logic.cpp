#include <logic.hpp>
#include <lookup.hpp>
#include <rng.hpp>
#include <utils.hpp>

#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>
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
    uint64_t coordsToIndex(uint64_t i, uint64_t j)
    {
        uint64_t index;
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
    Coords indexToCoords(uint64_t index)
    {
        uint64_t i = 2 * (index / 13);
        uint64_t j = index % 13;
        if (j > 5)
        {
            j -= 6;
            i += 1;
        }
        return Coords(i, j);
    }

    // Converts a cell index to its associated row index
    uint64_t indexToLine[] = {
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

    inline uint64_t _concatenateMove(uint64_t indexStart, uint64_t indexMid, uint64_t indexEnd)
    {
        return (indexStart) | (indexMid << INDEX_WIDTH) | (indexEnd << (2*INDEX_WIDTH));
    }

    inline uint64_t _concatenateHalfMove(uint64_t halfMove, uint64_t indexEnd)
    {
        return halfMove | (indexEnd << (2*INDEX_WIDTH));
    }

    inline uint64_t _concatenatePieces(uint64_t pieceStart, uint64_t pieceMid, uint64_t pieceEnd)
    {
        return (pieceStart << (3*INDEX_WIDTH)) | (pieceMid << (4*INDEX_WIDTH)) | (pieceEnd << (5*INDEX_WIDTH)) ;
    }

    inline uint64_t _concatenateVictory(uint64_t move)
    {
        return 0x80000000U | move;
    }

    // Converts a native index into a "a1" style string
    string indexToString(uint64_t index)
    {
        Coords coords = indexToCoords(index);
        string cellString = rowLetters[coords.first] + std::to_string(coords.second + 1);
        return cellString;
    }

    // Converts a "a1" style string coordinate into an index
    uint64_t stringToIndex(string cellString)
    {
        uint64_t i = UINT64_MAX;
        uint64_t j = UINT64_MAX;
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
    string moveToString(uint64_t move, const uint8_t cells[45])
    {
        uint64_t indexStart = move & INDEX_MASK;
        uint64_t indexMid = (move >> INDEX_WIDTH) & INDEX_MASK;
        uint64_t indexEnd = (move >> (2*INDEX_WIDTH)) & INDEX_MASK;

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
    uint64_t stringToMove(string moveString, const uint8_t cells[45])
    {
        vector<uint64_t> move(3, NULL_ACTION);
        if (moveString.size() == 4 || (moveString.size() == 6 && moveString[4] == '-'))
        {
            move[0] = stringToIndex(moveString.substr(0, 2));
            move[2] = stringToIndex(moveString.substr(2, 2));
            if (cells[move[2]] != 0 && ((cells[move[0]] & COLOUR_MASK) == (cells[move[2]] & COLOUR_MASK)))
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
                move[1] = NULL_ACTION;
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
                    if (piece >= 16)
                    {
                        cellsString += Logic::pieceToChar[piece >> HALF_PIECE_WIDTH];
                        cellsString += Logic::pieceToChar[piece & TOP_MASK];
                    }
                    else
                    {
                        cellsString += Logic::pieceToChar[piece & TOP_MASK];
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
                        newCells[cursor] = Logic::charToPiece[cellLines[i][j+1]] | (Logic::charToPiece[cellLines[i][j]] << HALF_PIECE_WIDTH);
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
    uint64_t _countPieceMoves(uint64_t indexStart, const uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];

        uint64_t count = 0ULL;

        // If the piece is not a stack
        if (movingPiece < 16)
        {
            // 1-range first action
            for (size_t indexMidLoop = 7 * indexStart + 1; indexMidLoop < 7 * indexStart + Lookup::neighbours[7 * indexStart] + 1; indexMidLoop++)
            {
                uint64_t indexMid = Lookup::neighbours[indexMidLoop];
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours2[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours2[indexEndLoop];
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells) || ((indexStart == (indexMid + indexEnd) / 2) && isMoveValid(movingPiece, indexEnd, cells)))
                        {
                            count++;
                        }
                    }

                    // stack, 0/1-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours[indexEndLoop];
                        if (isMoveValid(movingPiece, indexEnd, cells) || (indexStart == indexEnd))
                        {
                            count++;
                        }
                    }

                    // stack only
                    count++;
                }
                // 1-range move
                else if (isMoveValid(movingPiece, indexMid, cells))
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
                uint64_t indexMid = Lookup::neighbours2[indexMidLoop];
                if (isMove2Valid(movingPiece, indexStart, indexMid, cells))
                {
                    // 2-range move, stack or unstack
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours[indexEndLoop];
                        // 2-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            count++;
                        }

                        // 2-range move, stack
                        else if (isStackValid(movingPiece, indexEnd, cells))
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
                uint64_t indexMid = Lookup::neighbours[indexMidLoop];
                // 1-range move, [stack or unstack] optional
                if (isMoveValid(movingPiece, indexMid, cells))
                {

                    // 1-range move, stack or unstack
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours[indexEndLoop];
                        // 1-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            count++;
                        }

                        // 1-range move, stack
                        else if (isStackValid(movingPiece, indexEnd, cells))
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
                else if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours2[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours2[indexEndLoop];
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells))
                        {
                            count++;
                        }
                    }

                    // stack, 1-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours[indexEndLoop];
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
                if ((cells[index] & COLOUR_MASK) == (player << 1))
                {
                    count += _countPieceMoves(index, cells);
                }
            }
        }
        return count;
    }

    // Subroutine of the perft debug function that is ran by the main perft() function
    uint64_t _perftIter(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer)
    {
        if (recursionDepth == 1)
        {
            return _countPlayerMoves(currentPlayer, cells);
        }
        // Get a vector of all the available moves for the current player
        array<uint64_t, MAX_PLAYER_MOVES> moves = availablePlayerMoves(currentPlayer, cells);
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
    uint64_t perft(int recursionDepth, const uint8_t cells[45], uint8_t currentPlayer)
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
            uint8_t newCells[45];
            setState(newCells, cells);
            return _perftIter(1, newCells, currentPlayer);
        }
        else
        {
            // Get a vector of all the available moves for the current player
            array<uint64_t, MAX_PLAYER_MOVES> moves = availablePlayerMoves(currentPlayer, cells);
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
        array<uint64_t, MAX_PLAYER_MOVES> moves = availablePlayerMoves(currentPlayer, cells);
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
        std::memcpy(target, origin, 45);
    }

    // Drops a piece on top of an existing piece
    inline void _drop(uint8_t piece, uint64_t target, uint8_t cells[45])
    {
        // This is refactorizable if captures are stored.
        if ((cells[target] & COLOUR_MASK) != (piece & COLOUR_MASK))
        {
            cells[target] = 0U;
        }
        cells[target] = (cells[target] << HALF_PIECE_WIDTH) | piece;
    }

    // Applies a move between chosen coordinates
    inline void _move(uint64_t indexStart, uint64_t indexEnd, uint8_t cells[45])
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
    inline void _stack(uint64_t indexStart, uint64_t indexEnd, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];
        uint8_t pieceEnd = cells[indexEnd];

        // If the moving piece is already on top of a stack, leave the bottom piece in the starting cell
        cells[indexStart] = (movingPiece >> HALF_PIECE_WIDTH);

        // Move the top piece to the target cell and set its new bottom piece
        cells[indexEnd] = (movingPiece & TOP_MASK) + (pieceEnd << HALF_PIECE_WIDTH);
    }

    // Applies an unstack between chosen coordinates
    inline void _unstack(uint64_t indexStart, uint64_t indexEnd, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];

        // Leave the bottom piece in the starting cell
        cells[indexStart] = (movingPiece >> HALF_PIECE_WIDTH);
        // Remove the bottom piece from the moving piece
        // Move the top piece to the target cell
        // Will overwrite the eaten piece if there is one
        cells[indexEnd] = (movingPiece & TOP_MASK);
    }

    // Plays the selected move
    void play(uint64_t move, uint8_t cells[45])
    {
        uint64_t indexStart = move & 0x000000FF;
        uint64_t indexMid = (move >> INDEX_WIDTH) & 0x000000FF;
        uint64_t indexEnd = (move >> (2*INDEX_WIDTH)) & 0x000000FF;
        
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
                _move(indexStart, indexEnd, cells);
            }
            // There is an intermediate move
            else
            {
                uint8_t midPiece = cells[indexMid];
                uint8_t endPiece = cells[indexEnd];
                // The piece at the mid coordinates is an ally : stack and move
                if (midPiece != 0 && (midPiece & COLOUR_MASK) == (movingPiece & COLOUR_MASK) && (indexMid != indexStart))
                {
                    _stack(indexStart, indexMid, cells);
                    _move(indexMid, indexEnd, cells);
                }
                // The piece at the end coordinates is an ally : move and stack
                else if (endPiece != 0 && (endPiece & COLOUR_MASK) == (movingPiece & COLOUR_MASK))
                {
                    _move(indexStart, indexMid, cells);
                    _stack(indexMid, indexEnd, cells);
                }
                // The end coordinates contain an enemy or no piece : move and unstack
                else
                {
                    _move(indexStart, indexMid, cells);
                    _unstack(indexMid, indexEnd, cells);
                }
            }
        }
    }

    void unplay(uint64_t move, uint8_t cells[45])
    {
        uint64_t indexStart = move & 0x000000FF;
        uint64_t indexMid = (move >> INDEX_WIDTH) & 0x000000FF;
        uint64_t indexEnd = (move >> (2*INDEX_WIDTH)) & 0x000000FF;
        uint8_t pieceStart = (move >> (3*INDEX_WIDTH)) & 0x000000FF;
        uint8_t pieceMid = (move >> (4*INDEX_WIDTH)) & 0x000000FF;
        uint8_t pieceEnd = (move >> (5*INDEX_WIDTH)) & 0x000000FF;
        cells[indexStart] = pieceStart;
        if (indexMid <= 44)
        {
            cells[indexMid] = pieceMid;
        }
        cells[indexEnd] = pieceEnd;
    }

    void playManual(uint64_t move, uint8_t cells[45])
    {
        play(move, cells);
    }

    // Generates a random move
    uint64_t searchRandom(const uint8_t cells[45], uint8_t currentPlayer)
    {
        // Get a vector of all the available moves for the current player
        array<uint64_t, MAX_PLAYER_MOVES> moves = availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves[MAX_PLAYER_MOVES - 1];

        if (nMoves > 0)
        {
            std::uniform_int_distribution<int> intDistribution(0, nMoves - 1);

            uint64_t index = intDistribution(RNG::gen);

            return moves[index];
        }
        return NULL_MOVE;
    }

    // Plays a random move
    uint64_t playRandom(uint8_t cells[45], uint8_t currentPlayer)
    {
        uint64_t move = searchRandom(cells, currentPlayer);
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
                if ((cells[k] & COLOUR_MASK) == COLOUR_WHITE && (cells[k] & TYPE_MASK) != TYPE_WISE)
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
                if ((cells[k] & COLOUR_MASK) == COLOUR_BLACK && (cells[k] & TYPE_MASK) != TYPE_WISE)
                {
                    return true;
                }
            }
        }
        return false;
    }

    // Returns true if the move leads to a win
    bool isMoveWin(uint64_t move, const uint8_t cells[45])
    {
        size_t indexStart = move & 0x000000FF;
        size_t indexEnd = (move >> (2 * INDEX_WIDTH)) & 0x000000FF;
        uint8_t movingPiece = cells[indexStart];
        
        if ((movingPiece & TYPE_MASK) != TYPE_WISE)
        {
            if (((movingPiece & COLOUR_MASK) == COLOUR_WHITE && (indexEnd <= 5)) || ((movingPiece & COLOUR_MASK) == COLOUR_BLACK && (indexEnd >= 39)))
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
                if ((cells[k] & COLOUR_MASK) == COLOUR_WHITE && (cells[k] & TYPE_MASK) != TYPE_WISE)
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
                if ((cells[k] & COLOUR_MASK) == COLOUR_BLACK && (cells[k] & TYPE_MASK) != TYPE_WISE)
                {
                    return 1U;
                }
            }
        }
        return 0xFFU;
    }

    // Returns the list of possible moves for a specific piece
    void availablePieceMoves(uint64_t indexStart, const uint8_t cells[45], array<uint64_t, MAX_PLAYER_MOVES> &moves)
    {
        uint8_t movingPiece = cells[indexStart];
        size_t indexMoves = moves[MAX_PLAYER_MOVES - 1];

        // If the piece is not a stack
        if (movingPiece < 16)
        {
            // 1-range first action
            for (size_t indexMidLoop = 7 * indexStart + 1; indexMidLoop < 7 * indexStart + Lookup::neighbours[7 * indexStart] + 1; indexMidLoop++)
            {
                uint64_t indexMid = Lookup::neighbours[indexMidLoop];
                uint64_t halfMove = indexStart | (indexMid << INDEX_WIDTH);
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours2[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours2[indexEndLoop];
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells) || ((indexStart == (indexMid + indexEnd) / 2) && isMoveValid(movingPiece, indexEnd, cells)))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }
                    }

                    // stack, 0/1-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours[indexEndLoop];
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
                else if (isMoveValid(movingPiece, indexMid, cells))
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
                uint64_t indexMid = Lookup::neighbours2[indexMidLoop];
                uint64_t halfMove = indexStart | (indexMid << INDEX_WIDTH);
                if (isMove2Valid(movingPiece, indexStart, indexMid, cells))
                {
                    // 2-range move, stack or unstack
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours[indexEndLoop];
                        // 2-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }

                        // 2-range move, stack
                        else if (isStackValid(movingPiece, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }
                    }

                    // 2-range move
                    moves[indexMoves] =_concatenateMove(indexStart, 0x000000FF, indexMid);
                    indexMoves++;
                }
            }
            // 1-range first action
            for (size_t indexMidLoop = 7 * indexStart + 1; indexMidLoop < 7 * indexStart + Lookup::neighbours[7 * indexStart] + 1; indexMidLoop++)
            {
                uint64_t indexMid = Lookup::neighbours[indexMidLoop];
                uint64_t halfMove = indexStart | (indexMid << INDEX_WIDTH);
                // 1-range move, [stack or unstack] optional
                if (isMoveValid(movingPiece, indexMid, cells))
                {

                    // 1-range move, stack or unstack
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours[indexEndLoop];
                        // 1-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }

                        // 1-range move, stack
                        else if (isStackValid(movingPiece, indexEnd, cells))
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
                else if (isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours2[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours2[indexEndLoop];
                        if (isMove2Valid(movingPiece, indexMid, indexEnd, cells))
                        {
                            moves[indexMoves] = _concatenateHalfMove(halfMove, indexEnd);
                            indexMoves++;
                        }
                    }

                    // stack, 1-range move
                    for (size_t indexEndLoop = 7 * indexMid + 1; indexEndLoop < 7 * indexMid + Lookup::neighbours[7 * indexMid] + 1; indexEndLoop++)
                    {
                        uint64_t indexEnd = Lookup::neighbours[indexEndLoop];
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
    array<uint64_t, MAX_PLAYER_MOVES> availablePlayerMoves(uint8_t player, const uint8_t cells[45])
    {
        array<uint64_t, MAX_PLAYER_MOVES> moves;
        moves[MAX_PLAYER_MOVES - 1] = 0;

        // Calculate possible moves
        for (int index = 0; index < 45; index++)
        {
            if (cells[index] != 0)
            {
                // Choose pieces of the current player's colour
                if ((cells[index] & COLOUR_MASK) == (player << 1))
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
        uint8_t sourceType = source & TYPE_MASK;
        uint8_t targetType = target & TYPE_MASK;
        // Scissors > Paper, Paper > Rock, Rock > Scissors
        return ((sourceType == TYPE_SCISSORS && targetType == TYPE_PAPER) || (sourceType == TYPE_PAPER && targetType == TYPE_ROCK) || (sourceType == TYPE_ROCK && targetType == TYPE_SCISSORS));
    }

    // Returns whether a certain 1-range move is possible
    inline bool isMoveValid(uint8_t movingPiece, uint64_t indexEnd, const uint8_t cells[45])
    {
        if (cells[indexEnd] != 0)
        {
            // If the end piece and the moving piece are the same colour
            if ((cells[indexEnd] & COLOUR_MASK) == (movingPiece & COLOUR_MASK))
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
    inline bool isMove2Valid(uint8_t movingPiece, uint64_t indexStart, uint64_t indexEnd, const uint8_t cells[45])
    {
        // If there is a piece blocking the move (cell between the start and end positions)
        if (cells[(indexEnd + indexStart) / 2] != 0)
        {
            return false;
        }
        if (cells[indexEnd] != 0)
        {
            // If the end piece and the moving piece are the same colour
            if ((cells[indexEnd] & COLOUR_MASK) == (movingPiece & COLOUR_MASK))
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
    inline bool isStackValid(uint8_t movingPiece, uint64_t indexEnd, const uint8_t cells[45])
    {
        // If the end cell is not empty
        // If the target piece and the moving piece are the same colour
        // If the end piece is not a stack
        if ((cells[indexEnd] != 0) && ((cells[indexEnd] & COLOUR_MASK) == (movingPiece & COLOUR_MASK)) && (cells[indexEnd] < 16))
        {
            // If the upper piece is Wise and the target piece is not Wise
            if ((movingPiece & TYPE_MASK) == TYPE_WISE && (cells[indexEnd] & TYPE_MASK) != TYPE_WISE)
            {
                return false;
            }
            return true;
        }
        return false;
    }

    // Returns whether a certain unstack action is possible
    inline bool isUnstackValid(uint8_t movingPiece, uint64_t indexEnd, const uint8_t cells[45])
    {
        if (cells[indexEnd] != 0)
        {
            // If the cells are the same colour
            if ((cells[indexEnd] & COLOUR_MASK) == (movingPiece & COLOUR_MASK))
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
                if (currentPlayer == 0 && (cells[index] & COLOUR_MASK) == COLOUR_WHITE)
                {
                    countWhite[index] += _countPieceMoves(index, cells);
                }
                else if (currentPlayer == 1 && (cells[index] & COLOUR_MASK) == COLOUR_BLACK)
                {
                    countBlack[index] += _countPieceMoves(index, cells);
                }
            }
        }
    }
}