#include <logic.hpp>
#include <rng.hpp>

using namespace std;

namespace PijersiEngine
{
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
    void indexToCoords(int index, int *i, int *j)
    {
        *i = 2 * (index / 13);
        *j = index % 13;
        if (*j > 5)
        {
            *j -= 6;
            *i += 1;
        }
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

    // Subroutine of the perft debug function that is ran by the main perft() function
    uint64_t _perftIter(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer)
    {
        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(currentPlayer, cells);

        if (recursionDepth == 0)
        {
            // uint8_t newCells[45];
            // for (int k = 0; k < moves.size() / 3; k++)
            // {
            //     _setState(newCells, cells);
            //     _playManual(moves.data() + 3 * k, newCells);
            //     sum += 1;
            // }
            return moves.size() / 3;
            // return sum;
        }

        uint64_t sum = 0ULL;

        uint8_t newCells[45];
        for (int k = 0; k < moves.size() / 3; k++)
        {
            _setState(newCells, cells);
            _playManual(moves.data() + 3 * k, newCells);
            sum += _perftIter(recursionDepth - 1, newCells, 1 - currentPlayer);
        }
        return sum;
    }

    // Perft debug function to measure the number of leaf nodes (possible moves) at a given depth
    uint64_t perft(int recursionDepth, uint8_t cells[45], uint8_t currentPlayer)
    {
        if (recursionDepth == 0)
        {
            return _perftIter(0, cells, currentPlayer);
        }
        else
        {
            // Get a vector of all the available moves for the current player
            vector<int> moves = _availablePlayerMoves(currentPlayer, cells);
            
            uint64_t sum = 0ULL;
            #pragma omp parallel for schedule(dynamic) reduction(+:sum)
            for (int k = 0; k < moves.size() / 3; k++)
            {
                uint8_t newCells[45];
                _setState(newCells, cells);
                _playManual(moves.data() + 3 * k, newCells);
                sum += _perftIter(recursionDepth - 1, newCells, 1 - currentPlayer);
            }
            return sum;
        }
    }

    void _setState(uint8_t target[45], const uint8_t origin[45])
    {
        for (int k = 0; k < 45; k++)
        {
            target[k] = origin[k];
        }
    }

    // Plays the selected move
    void _play(int indexStart, int indexMid, int indexEnd, uint8_t cells[45])
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
                _move(indexStart, indexEnd, cells);
            }
            // There is an intermediate move
            else
            {
                uint8_t midPiece = cells[indexMid];
                uint8_t endPiece = cells[indexEnd];
                // The piece at the mid coordinates is an ally : stack and move
                if (midPiece != 0 && (midPiece & 2) == (movingPiece & 2) && (indexMid != indexStart))
                {
                    _stack(indexStart, indexMid, cells);
                    _move(indexMid, indexEnd, cells);
                }
                // The piece at the end coordinates is an ally : move and stack
                else if (endPiece != 0 && (endPiece & 2) == (movingPiece & 2))
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

    void _playManual(int move[3], uint8_t cells[45])
    {
        _play(move[0], move[1], move[2], cells);
    }

    // Generates a random move
    vector<int> _ponderRandom(uint8_t cells[45], uint8_t currentPlayer)
    {
        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(currentPlayer, cells);

        if (moves.size() > 0)
        {
            uniform_int_distribution<int> intDistribution(0, moves.size() / 6 - 1);

            int index = intDistribution(gen);

            vector<int>::const_iterator first = moves.begin() + 6 * index;
            vector<int>::const_iterator last = moves.begin() + 6 * (index + 1);
            vector<int> move(first, last);
            return move;
        }
        return vector<int>({-1, -1, -1, -1, -1, -1});
    }

    // Plays a random move
    vector<int> _playRandom(uint8_t cells[45], uint8_t currentPlayer)
    {
        vector<int> move = _ponderRandom(cells, currentPlayer);
        // Apply move
        _playManual(move.data(), cells);

        return move;
    }

    // Returns true if the board is in a winning position
    bool _checkWin(const uint8_t cells[45])
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
    vector<int> _availablePieceMoves(int indexStart, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];

        vector<int> moves = vector<int>();
        moves.reserve(128);

        // If the piece is not a stack
        if (movingPiece < 16)
        {
            // 1-range first action
            for (int indexMid : _neighbours(indexStart))
            {
                // stack, [1/2-range move] optional
                if (_isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (int indexEnd : _neighbours2(indexMid))
                    {
                        if (_isMove2Valid(movingPiece, indexMid, indexEnd, cells) || ((indexStart == (indexMid + indexEnd) / 2) && _isMoveValid(movingPiece, indexEnd, cells)))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // stack, 0/1-range move
                    for (int indexEnd : _neighbours(indexMid))
                    {
                        if (_isMoveValid(movingPiece, indexEnd, cells) || (indexStart == indexEnd))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // stack only
                    moves.insert(moves.end(), {indexStart, indexStart, indexMid});
                }
                // 1-range move
                if (_isMoveValid(movingPiece, indexMid, cells))
                {
                    moves.insert(moves.end(), {indexStart, -1, indexMid});
                }
            }
        }
        else
        {
            // 2 range first action
            for (int indexMid : _neighbours2(indexStart))
            {
                if (_isMove2Valid(movingPiece, indexStart, indexMid, cells))
                {
                    // 2-range move, stack or unstack
                    for (int indexEnd : _neighbours(indexMid))
                    {
                        // 2-range move, unstack
                        if (_isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }

                        // 2-range move, stack
                        if (_isStackValid(movingPiece, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // 2-range move
                    moves.insert(moves.end(), {indexStart, -1, indexMid});
                }
            }
            // 1-range first action
            for (int indexMid : _neighbours(indexStart))
            {

                // 1-range move, [stack or unstack] optional
                if (_isMoveValid(movingPiece, indexMid, cells))
                {

                    // 1-range move, stack or unstack
                    for (int indexEnd : _neighbours(indexMid))
                    {
                        // 1-range move, unstack
                        if (_isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }

                        // 1-range move, stack
                        if (_isStackValid(movingPiece, indexEnd, cells))
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
                if (_isStackValid(movingPiece, indexMid, cells))
                {
                    // stack, 2-range move
                    for (int indexEnd : _neighbours2(indexMid))
                    {
                        if (_isMove2Valid(movingPiece, indexMid, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // stack, 1-range move
                    for (int indexEnd : _neighbours(indexMid))
                    {
                        if (_isMoveValid(movingPiece, indexEnd, cells))
                        {
                            moves.insert(moves.end(), {indexStart, indexMid, indexEnd});
                        }
                    }

                    // stack only
                    moves.insert(moves.end(), {indexStart, indexStart, indexMid});
                }

                // unstack
                if (_isUnstackValid(movingPiece, indexMid, cells))
                {
                    // unstack only
                    moves.insert(moves.end(), {indexStart, indexStart, indexMid});
                }
            }
        }

        return moves;
    }

    // Returns the list of possible moves for a player
    vector<int> _availablePlayerMoves(uint8_t player, uint8_t cells[45])
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
                    vector<int> pieceMoves = _availablePieceMoves(index, cells);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                }
            }
        }
        return moves;
    }

    // Returns whether a source piece can capture a target piece
    bool _canTake(uint8_t source, uint8_t target)
    {
        uint8_t sourceType = source & 12;
        uint8_t targetType = target & 12;
        // Scissors > Paper, Paper > Rock, Rock > Scissors
        if (sourceType == 0 && targetType == 4 || sourceType == 4 && targetType == 8 || sourceType == 8 && targetType == 0)
        {
            return true;
        }
        return false;
    }

    // Applies a move between chosen coordinates
    void _move(int indexStart, int indexEnd, uint8_t cells[45])
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
    void _stack(int indexStart, int indexEnd, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[indexStart];
        uint8_t endPiece = cells[indexEnd];

        // If the moving piece is already on top of a stack, leave the bottom piece in the starting cell
        cells[indexStart] = (movingPiece >> 4);

        // Move the top piece to the target cell and set its new bottom piece
        cells[indexEnd] = (movingPiece & 15) + (endPiece << 4);
    }

    // Applies an unstack between chosen coordinates
    void _unstack(int indexStart, int indexEnd, uint8_t cells[45])
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
    bool _isMoveValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45])
    {
        if (cells[indexEnd] != 0)
        {
            // If the end piece and the moving piece are the same colour
            if ((cells[indexEnd] & 2) == (movingPiece & 2))
            {
                return false;
            }
            if (!_canTake(movingPiece, cells[indexEnd]))
            {
                return false;
            }
        }
        return true;
    }

    // Returns whether a certain 2-range move is possible
    bool _isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd, uint8_t cells[45])
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
            if (!_canTake(movingPiece, cells[indexEnd]))
            {
                return false;
            }
        }
        return true;
    }

    // Returns whether a certain stack action is possible
    bool _isStackValid(uint8_t movingPiece, int indexEnd, const uint8_t cells[45])
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
    bool _isUnstackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45])
    {
        if (cells[indexEnd] != 0)
        {
            // If the cells are the same colour
            if ((cells[indexEnd] & 2) == (movingPiece & 2))
            {
                return false;
            }
            if (!_canTake(movingPiece, cells[indexEnd]))
            {
                return false;
            }
        }
        return true;
    }

    // Returns the 2-range neighbours of the designated cell
    vector<int> _neighbours(int index)
    {
        switch (index)
        {
        case 0:
            return vector<int>({1, 6, 7});
        case 1:
            return vector<int>({0, 2, 7, 8});
        case 2:
            return vector<int>({1, 3, 8, 9});
        case 3:
            return vector<int>({2, 4, 9, 10});
        case 4:
            return vector<int>({3, 5, 10, 11});
        case 5:
            return vector<int>({4, 11, 12});
        case 6:
            return vector<int>({0, 7, 13});
        case 7:
            return vector<int>({0, 1, 6, 8, 13, 14});
        case 8:
            return vector<int>({1, 2, 7, 9, 14, 15});
        case 9:
            return vector<int>({2, 3, 8, 10, 15, 16});
        case 10:
            return vector<int>({3, 4, 9, 11, 16, 17});
        case 11:
            return vector<int>({4, 5, 10, 12, 17, 18});
        case 12:
            return vector<int>({5, 11, 18});
        case 13:
            return vector<int>({6, 7, 14, 19, 20});
        case 14:
            return vector<int>({7, 8, 13, 15, 20, 21});
        case 15:
            return vector<int>({8, 9, 14, 16, 21, 22});
        case 16:
            return vector<int>({9, 10, 15, 17, 22, 23});
        case 17:
            return vector<int>({10, 11, 16, 18, 23, 24});
        case 18:
            return vector<int>({11, 12, 17, 24, 25});
        case 19:
            return vector<int>({13, 20, 26});
        case 20:
            return vector<int>({13, 14, 19, 21, 26, 27});
        case 21:
            return vector<int>({14, 15, 20, 22, 27, 28});
        case 22:
            return vector<int>({15, 16, 21, 23, 28, 29});
        case 23:
            return vector<int>({16, 17, 22, 24, 29, 30});
        case 24:
            return vector<int>({17, 18, 23, 25, 30, 31});
        case 25:
            return vector<int>({18, 24, 31});
        case 26:
            return vector<int>({19, 20, 27, 32, 33});
        case 27:
            return vector<int>({20, 21, 26, 28, 33, 34});
        case 28:
            return vector<int>({21, 22, 27, 29, 34, 35});
        case 29:
            return vector<int>({22, 23, 28, 30, 35, 36});
        case 30:
            return vector<int>({23, 24, 29, 31, 36, 37});
        case 31:
            return vector<int>({24, 25, 30, 37, 38});
        case 32:
            return vector<int>({26, 33, 39});
        case 33:
            return vector<int>({26, 27, 32, 34, 39, 40});
        case 34:
            return vector<int>({27, 28, 33, 35, 40, 41});
        case 35:
            return vector<int>({28, 29, 34, 36, 41, 42});
        case 36:
            return vector<int>({29, 30, 35, 37, 42, 43});
        case 37:
            return vector<int>({30, 31, 36, 38, 43, 44});
        case 38:
            return vector<int>({31, 37, 44});
        case 39:
            return vector<int>({32, 33, 40});
        case 40:
            return vector<int>({33, 34, 39, 41});
        case 41:
            return vector<int>({34, 35, 40, 42});
        case 42:
            return vector<int>({35, 36, 41, 43});
        case 43:
            return vector<int>({36, 37, 42, 44});
        case 44:
            return vector<int>({37, 38, 43});
        default:
            return vector<int>();
        }
    }

    // Returns the 2-range neighbours of the designated cell
    vector<int> _neighbours2(int index)
    {
        switch (index)
        {
        case 0:
            return vector<int>({2, 14});
        case 1:
            return vector<int>({3, 13, 15});
        case 2:
            return vector<int>({0, 4, 14, 16});
        case 3:
            return vector<int>({1, 5, 15, 17});
        case 4:
            return vector<int>({2, 16, 18});
        case 5:
            return vector<int>({3, 17});
        case 6:
            return vector<int>({8, 20});
        case 7:
            return vector<int>({9, 19, 21});
        case 8:
            return vector<int>({6, 10, 20, 22});
        case 9:
            return vector<int>({7, 11, 21, 23});
        case 10:
            return vector<int>({8, 12, 22, 24});
        case 11:
            return vector<int>({9, 23, 25});
        case 12:
            return vector<int>({10, 24});
        case 13:
            return vector<int>({1, 15, 27});
        case 14:
            return vector<int>({0, 2, 16, 26, 28});
        case 15:
            return vector<int>({1, 3, 13, 17, 27, 29});
        case 16:
            return vector<int>({2, 4, 14, 18, 28, 30});
        case 17:
            return vector<int>({3, 5, 15, 29, 31});
        case 18:
            return vector<int>({4, 16, 30});
        case 19:
            return vector<int>({7, 21, 33});
        case 20:
            return vector<int>({6, 8, 22, 32, 34});
        case 21:
            return vector<int>({7, 9, 19, 23, 33, 35});
        case 22:
            return vector<int>({8, 10, 20, 24, 34, 36});
        case 23:
            return vector<int>({9, 11, 21, 25, 35, 37});
        case 24:
            return vector<int>({10, 12, 22, 36, 38});
        case 25:
            return vector<int>({11, 23, 37});
        case 26:
            return vector<int>({14, 28, 40});
        case 27:
            return vector<int>({13, 15, 29, 39, 41});
        case 28:
            return vector<int>({14, 16, 26, 30, 40, 42});
        case 29:
            return vector<int>({15, 17, 27, 31, 41, 43});
        case 30:
            return vector<int>({16, 18, 28, 42, 44});
        case 31:
            return vector<int>({17, 29, 43});
        case 32:
            return vector<int>({20, 34});
        case 33:
            return vector<int>({19, 21, 35});
        case 34:
            return vector<int>({20, 22, 32, 36});
        case 35:
            return vector<int>({21, 23, 33, 37});
        case 36:
            return vector<int>({22, 24, 34, 38});
        case 37:
            return vector<int>({23, 25, 35});
        case 38:
            return vector<int>({24, 36});
        case 39:
            return vector<int>({27, 41});
        case 40:
            return vector<int>({26, 28, 42});
        case 41:
            return vector<int>({27, 29, 39, 43});
        case 42:
            return vector<int>({28, 30, 40, 44});
        case 43:
            return vector<int>({29, 31, 41});
        case 44:
            return vector<int>({30, 42});
        default:
            return vector<int>();
        }
    }

}