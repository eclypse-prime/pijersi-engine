#include <board.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <omp.h>
#include <algorithm>
#include <random>
#include <cfloat>
#include <chrono>

using namespace std;


namespace PijersiEngine
{
    random_device rd; // Obtaining random number from hardware
    mt19937 gen(rd()); // Seeding generator
    uniform_real_distribution<float> distribution(-0.25f, 0.25f); // Defining uniform distribution

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
        int i = 2 * (index/13);
        if ((index%13) > 5)
        {
            i += 1;
        }
        return i;
    }

    // Adds a bottom piece to the selected piece
    uint8_t addBottom(uint8_t piece, uint8_t newBottom)
    {
        return piece + (newBottom << 4);
    }

    // Creates a piece of chosen colour and type
    uint8_t createPiece(PieceColour colour, PieceType type)
    {
        uint8_t piece = 1;
        if (colour == Black) {
            piece += 2;
        }
        switch (type)
        {
            case Scissors:
                break;
            case Paper:
                piece += 4;
                break;
            case Rock:
                piece += 8;
                break;
            case Wise:
                piece += 12;
                break;
        }
        return piece;
    }

    // Board constructor
    Board::Board()
    {
        for (int k = 0; k < 45; k++)
        {
            cells[k] = 0;
        }
    }

    // Board copy constructor
    Board::Board(Board &board)
    {
        setState(board.cells);
        currentPlayer = board.currentPlayer;
    }

    // Calculates a move using alphabeta minimax algorithm of chosen depth.
    vector<int> Board::ponder(int recursionDepth, bool random)
    {

        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(currentPlayer, cells);

        cout << moves.size()/6 << endl;
        if (moves.size() > 0)
        {
            int index = 0;
            float *extremums = new float[moves.size() / 6];

            float alpha = -FLT_MAX;
            float beta = FLT_MAX;
            
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
                    float extremum = salt + extremums[k];
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
                    float extremum = salt + extremums[k];
                    if (extremum < minimum)
                    {
                        minimum = extremum;
                        index = k;
                    }
                }
            }

            forecast = extremums[index];

            delete extremums;

            vector<int>::const_iterator first = moves.begin() + 6 * index;
            vector<int>::const_iterator last = moves.begin() + 6 * (index + 1);
            vector<int> move(first, last);
            return move;
        }
        return vector<int>({0, 0, 0, 0, 0, 0});
    }

    // Plays a move and returns it
    vector<int> Board::playAuto(int recursionDepth, bool random)
    {
        // Calculate move
        vector<int> move = ponder(recursionDepth, random);
        // vector<int> move = ponderMCTS(1);
        // Apply move
        playManual(move);
        return move;
    }

    // Chooses a random move
    vector<int> Board::ponderRandom()
    {
        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(currentPlayer, cells);

        uniform_int_distribution<int> intDistribution(0, moves.size()/6 - 1);

        int index = intDistribution(gen);

        vector<int>::const_iterator first = moves.begin() + 6 * index;
        vector<int>::const_iterator last = moves.begin() + 6 * (index + 1);
        vector<int> move(first, last);
        return move;
    }

    // Plays a random move and returns it
    vector<int> Board::playRandom()
    {
        // Calculate move
        vector<int> move = ponderRandom();
        // Apply move
        playManual(move);
        return move;
    }

    bool Board::isMoveLegal(vector<int> move)
    {
        if (cells[coordsToIndex(move[0], move[1])] == 0)
        {
            return false;
        }
        if ((cells[coordsToIndex(move[0], move[1])] & 2) != currentPlayer << 1)
        {
            return false;
        }
        vector<int> moves = _availablePieceMoves(move[0], move[1], cells);
        for (int k = 0; k < moves.size()/6; k++)
        {
            bool legal = true;
            for (int m = 0; m < 6; m++)
            {
                if (moves[k*6+m] != move[m])
                {
                    legal = false;
                }
            }
            if (legal)
            {
                return true;
            }
        }
        return false;
    }

    // Applies a move between chosen coordinates
    void _move(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45])
    {
        // Do nothing if start and end coordinate are identical
        if (iStart != iEnd || jStart != jEnd)
        {
            // Move the piece to the target cell
            cells[coordsToIndex(iEnd, jEnd)] = cells[coordsToIndex(iStart, jStart)];

            // Set the starting cell as empty
            cells[coordsToIndex(iStart, jStart)] = 0;
        }
    }

    // Applies a stack between chosen coordinates
    void _stack(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[coordsToIndex(iStart, jStart)];
        uint8_t endPiece = cells[coordsToIndex(iEnd, jEnd)];

        // If the moving piece is already on top of a stack, leave the bottom piece in the starting cell
        cells[coordsToIndex(iStart, jStart)] = (movingPiece >> 4);

        // Move the top piece to the target cell and set its new bottom piece
        cells[coordsToIndex(iEnd, jEnd)] = (movingPiece & 15) + (endPiece << 4);
    }

    // Applies an unstack between chosen coordinates
    void _unstack(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[coordsToIndex(iStart, jStart)];

        // Leave the bottom piece in the starting cell
        cells[coordsToIndex(iStart, jStart)] = (movingPiece >> 4);
        // Remove the bottom piece from the moving piece
        // Move the top piece to the target cell
        // Will overwrite the eaten piece if there is one
        cells[coordsToIndex(iEnd, jEnd)] = (movingPiece & 15);
    }

    // Plays the selected move
    void _play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd, uint8_t cells[45])
    {
        uint8_t movingPiece = cells[coordsToIndex(iStart, jStart)];
        if (movingPiece != 0)
        {
            // If there is no intermediate move
            if (iMid < 0 || jMid < 0)
            {
                // Simple move
                _move(iStart, jStart, iEnd, jEnd, cells);
            }
            // There is an intermediate move
            else
            {
                uint8_t midPiece = cells[coordsToIndex(iMid, jMid)];
                uint8_t endPiece = cells[coordsToIndex(iEnd, jEnd)];
                // The piece at the mid coordinates is an ally : stack and move
                if (midPiece != 0 && (midPiece & 2) == (movingPiece & 2) && (iMid != iStart || jMid != jStart))
                {
                    _stack(iStart, jStart, iMid, jMid, cells);
                    _move(iMid, jMid, iEnd, jEnd, cells);
                }
                // The piece at the end coordinates is an ally : move and stack
                else if (endPiece != 0 && (endPiece & 2) == (movingPiece & 2))
                {
                    _move(iStart, jStart, iMid, jMid, cells);
                    _stack(iMid, jMid, iEnd, jEnd, cells);
                }
                // The end coordinates contain an enemy or no piece : move and unstack
                else
                {
                    _move(iStart, jStart, iMid, jMid, cells);
                    _unstack(iMid, jMid, iEnd, jEnd, cells);
                }
            }
        }
    }

    void Board::playManual(vector<int> move)
    {
        _play(move[0], move[1], move[2], move[3], move[4], move[5], cells);
        // Set current player to the other colour.
        currentPlayer = 1 - currentPlayer;
    }

    void _playManual(int move[6], uint8_t cells[45])
    {
        _play(move[0], move[1], move[2], move[3], move[4], move[5], cells);
    }

    uint8_t Board::at(int i, int j)
    {
        return cells[coordsToIndex(i, j)];
    }

    // Evaluate piece according to its position, colour and type
    float _evaluatePiece(uint8_t piece, int i)
    {

        float score;
        // If the piece isn't Wise
        if ((piece & 12) != 12)
        {
            score = 15.f - 12.f * (piece & 2) - i;

            // If the piece is in a winning position
            if ((i == 0 && (piece & 2) == 0 ) || (i == 6 && (piece & 2) == 2))
            {
                score *= 10000.f;
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

    float Board::evaluate()
    {
        return _evaluate(cells);
    }

    // Evaluates the board
    float _evaluate(uint8_t cells[45])
    {
        int score = 0;
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != 0)
            {
                score += _evaluatePiece(cells[k], indexToLine(k));
            }
        }
        return score;
    }

    // Adds a piece to the designated coordinates
    void Board::addPiece(uint8_t piece, int i, int j)
    {
        cells[coordsToIndex(i, j)] = piece;
    }

    void _setState(uint8_t cells[45], uint8_t newState[45])
    {
        for (int k = 0; k < 45; k++)
        {
            cells[k] = newState[k];
        }
    }

    // Sets the board to a chosen state
    void Board::setState(uint8_t newState[45])
    {
        _setState(cells, newState);
    }

    uint8_t *Board::getState()
    {
        return cells;
    }

    // Initializes the board to the starting position
    void Board::init()
    {
        // Reset board
        for (int k = 0; k < 45; k++)
        {
            cells[k] = 0;
        }

        // Black pieces
        addPiece(createPiece(Black, Scissors), 0, 0);
        addPiece(createPiece(Black, Paper), 0, 1);
        addPiece(createPiece(Black, Rock), 0, 2);
        addPiece(createPiece(Black, Scissors), 0, 3);
        addPiece(createPiece(Black, Paper), 0, 4);
        addPiece(createPiece(Black, Rock), 0, 5);
        addPiece(createPiece(Black, Paper), 1, 0);
        addPiece(createPiece(Black, Rock), 1, 1);
        addPiece(createPiece(Black, Scissors), 1, 2);
        addPiece(addBottom(createPiece(Black, Wise),createPiece(Black, Wise)), 1, 3);
        addPiece(createPiece(Black, Rock), 1, 4);
        addPiece(createPiece(Black, Scissors), 1, 5);
        addPiece(createPiece(Black, Paper), 1, 6);

        // White pieces
        addPiece(createPiece(White, Paper), 5, 0);
        addPiece(createPiece(White, Scissors), 5, 1);
        addPiece(createPiece(White, Rock), 5, 2);
        addPiece(addBottom(createPiece(White, Wise),createPiece(White, Wise)), 5, 3);
        addPiece(createPiece(White, Scissors), 5, 4);
        addPiece(createPiece(White, Rock), 5, 5);
        addPiece(createPiece(White, Paper), 5, 6);
        addPiece(createPiece(White, Rock), 6, 0);
        addPiece(createPiece(White, Paper), 6, 1);
        addPiece(createPiece(White, Scissors), 6, 2);
        addPiece(createPiece(White, Rock), 6, 3);
        addPiece(createPiece(White, Paper), 6, 4);
        addPiece(createPiece(White, Scissors), 6, 5);

        // Set active player to White
        currentPlayer = 0;
    }

    // Converts a piece to char format
    // Used for debug purposes
    char pieceToChar(uint8_t piece)
    {
        char res = ' ';
        // If the piece is White
        if ((piece & 2) == 0)
        {
            // Read the piece's type
            switch (piece & 12)
            {
            case 0:
                res = 'S';
                break;
            case 4:
                res = 'P';
                break;
            case 8:
                res = 'R';
                break;
            case 12:
                res = 'W';
                break;
            default:
                break;
            }
        }
        // If the piece is Black
        else if ((piece & 2) == 2)
        {
            // Read the piece's type
            switch (piece & 12)
            {
            case 0:
                res = 's';
                break;
            case 4:
                res = 'p';
                break;
            case 8:
                res = 'r';
                break;
            case 12:
                res = 'w';
                break;
            default:
                break;
            }
        }
        return res;
    }

    // Prints the board
    // Used for debug purposes
    void Board::print()
    {
        cout << toString();
    }

    // Returns the state of the board in string format
    // Used for debug purposes
    string Board::toString()
    {
        string output = "";
        for (int i = 0; i < 7; i++)
        {
            if (i % 2 == 0)
            {
                output += ' ';
                for (int j = 0; j < 6; j++)
                {
                    char char1 = '.';
                    char char2 = ' ';
                    uint8_t piece = cells[coordsToIndex(i, j)];
                    if (piece != 0)
                    {
                        char1 = pieceToChar(piece);
                        // If the piece is a stack
                        if (piece >= 16)
                        {
                            char2 = pieceToChar(piece >> 4);
                        }
                    }
                    output += char1;
                    output += char2;
                }
                output += '\n';
            }
            else
            {
                for (int j = 0; j < 7; j++)
                {
                    char char1 = '.';
                    char char2 = ' ';
                    uint8_t piece = cells[coordsToIndex(i, j)];
                    if (piece != 0)
                    {
                        char1 = pieceToChar(piece);
                        // If the piece is a stack
                        if (piece >= 16)
                        {
                            char2 = pieceToChar(piece >> 4);
                        }
                    }
                    output += char1;
                    output += char2;
                }
                output += '\n';
            }
        }
        return output;
    }

    // Returns true if the board is in a winning position
    bool _checkWin(uint8_t cells[45])
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

    // Returns true if the board is in a winning position
    bool Board::checkWin()
    {
        return _checkWin(cells);
    }

    float Board::getForecast()
    {
        return forecast;
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

    bool canTake(uint8_t source, uint8_t target)
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

    bool _isMoveValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45])
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
            if (!canTake(movingPiece, cells[indexEnd]))
            {
                return false;
            }
        }
        return true;
    }

    bool _isStackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45])
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

    bool _isUnstackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45])
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

    vector<int> _availablePlayerMoves(uint8_t player, uint8_t cells[45])
    {
        vector<int> moves = vector<int>();
        // Reserve space in vector for optimization purposes
        moves.reserve(2048);
        // Calculate possible moves
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != 0)
            {
                // Choose pieces of the current player's colour
                if ((cells[k] & 2) == (player << 1))
                {
                    int i, j;
                    indexToCoords(k, &i, &j);
                    vector<int> pieceMoves = _availablePieceMoves(i, j, cells);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                }
            }
        }
        return moves;
    }

    // Returns the list of possible moves for a specific piece
    vector<int> _availablePieceMoves(int iStart, int jStart, uint8_t cells[45])
    {
        int indexStart = coordsToIndex(iStart, jStart);

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
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);

                    // stack, 2-range move
                    for (int indexEnd : _neighbours2(indexMid))
                    {
                        if (_isMove2Valid(movingPiece, indexMid, indexEnd, cells) || ((indexStart == (indexMid + indexEnd)/2) && _isMoveValid(movingPiece, indexEnd, cells)))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }

                    // stack, 0/1-range move
                    for (int indexEnd : _neighbours(indexMid))
                    {
                        if (_isMoveValid(movingPiece, indexEnd, cells) || (indexStart == indexEnd))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }

                    // stack only
                    moves.insert(moves.end(), {iStart, jStart, iStart, jStart, iMid, jMid});

                }
                // 1-range move
                if (_isMoveValid(movingPiece, indexMid, cells))
                {
                    int iEnd, jEnd;
                    indexToCoords(indexMid, &iEnd, &jEnd);
                    moves.insert(moves.end(), {iStart, jStart, -1, -1, iEnd, jEnd});
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
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);

                    // 2-range move, stack or unstack
                    for (int indexEnd : _neighbours(indexMid))
                    {
                        // 2-range move, unstack
                        if (_isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }

                        // 2-range move, stack
                        if (_isStackValid(movingPiece, indexEnd, cells))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }

                    // 2-range move
                    moves.insert(moves.end(), {iStart, jStart, -1, -1, iMid, jMid});
                }
            }
            // 1-range first action
            for (int indexMid : _neighbours(indexStart))
            {

                // 1-range move, [stack or unstack] optional
                if (_isMoveValid(movingPiece, indexMid, cells))
                {
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);


                    // 1-range move, stack or unstack
                    for (int indexEnd : _neighbours(indexMid))
                    {
                        // 1-range move, unstack
                        if (_isUnstackValid(movingPiece, indexEnd, cells))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }

                        // 1-range move, stack
                        if (_isStackValid(movingPiece, indexEnd, cells))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }

                    }
                    // 1-range move, unstack on starting position
                    moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iStart, jStart});

                    // 1-range move
                    moves.insert(moves.end(), {iStart, jStart, -1, -1, iMid, jMid});
                }
                // stack, [1/2-range move] optional
                if (_isStackValid(movingPiece, indexMid, cells))
                {
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);

                    // stack, 2-range move
                    for (int indexEnd : _neighbours2(indexMid))
                    {
                        if (_isMove2Valid(movingPiece, indexMid, indexEnd, cells))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }

                    // stack, 1-range move
                    for (int indexEnd : _neighbours(indexMid))
                    {
                        if (_isMoveValid(movingPiece, indexEnd, cells))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }

                    // stack only
                    moves.insert(moves.end(), {iStart, jStart, iStart, jStart, iMid, jMid});
                }

                // unstack
                if (_isUnstackValid(movingPiece, indexMid, cells))
                {
                    // unstack only
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);
                    moves.insert(moves.end(), {iStart, jStart, iStart, jStart, iMid, jMid});
                }
            }
        }

        return moves;
    }

    // Evaluates a move by calculating the possible subsequent moves recursively
    float _evaluateMove(int move[6], int recursionDepth, float alpha, float beta, uint8_t cells[45], int currentPlayer)
    {
        // Create a new board on which the move will be played
        uint8_t newCells[45];
        _setState(newCells, cells);
        _playManual(move, newCells);
        // Set current player to the other colour.
        currentPlayer = 1 - currentPlayer;

        float score;

        // Stop the recursion if a winning position is achieved
        if (_checkWin(newCells))
        {
            return _evaluate(newCells);
        }

        vector<int> moves = _availablePlayerMoves(currentPlayer, newCells);

        // Evaluate available moves and find the best one
        if (moves.size() > 0)
        {
            if (recursionDepth > 1)
            {
                if (currentPlayer == 0)
                {
                    float maximum = -FLT_MAX;
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
                    float minimum = FLT_MAX;
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
                    float maximum = -FLT_MAX;
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
                    float minimum = FLT_MAX;
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
    float _evaluateMoveTerminal(int move[6], uint8_t cells[45], uint8_t newCells[45])
    {
        _setState(newCells, cells);
        _playManual(move, newCells);

        return _evaluate(newCells);
    }

    float _UCT(int nodeWins, int nodeSimulations, int totalSimulations)
    {
        return nodeWins/nodeSimulations + 1.414f * sqrtf(logf(totalSimulations)) / nodeSimulations;
    }

    Node::Node(Node *newParent, vector<int> newMove, uint8_t rootPlayer)
    {
        parent = newParent;
        player = rootPlayer;
        move = newMove;
        if (parent != nullptr)
        {
            board = new Board(*parent->board);
            board->playManual(move);
        }
        else
        {
            board = new Board();
            board->init();
        }

        children = vector<Node>();
        children.reserve(256);
    }

    Node::~Node()
    {
        if (board != nullptr)
        {
            delete board;
        }
    }

    bool Node::isLeaf()
    {
        return (children.size() == 0);
    }

    bool Node::isWin()
    {
        return board->checkWin();
    }

    void Node::update(bool win)
    {
        visits++;
        if (win)
        {
            score++;
        }
        if (parent != nullptr)
        {
            parent->update(win);
        }
    }

    void Node::rollout()
    {
        Board newBoard(*board);
        while (!newBoard.checkWin())
        {
            newBoard.playRandom();
        }
        update(((newBoard.evaluate() > 0 && player == 0) || (newBoard.evaluate() <= 0 && player == 1)));
    }

    void Node::expand()
    {
        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(board->currentPlayer, board->getState());
        if (moves.size() > 0)
        {
            for (int k = 0; k < moves.size() / 6; k++)
            {
                vector<int>::const_iterator first = moves.begin() + 6 * k;
                vector<int>::const_iterator last = moves.begin() + 6 * (k + 1);
                vector<int> move(first, last);
                Node child(this, move, player);
                children.push_back(child);
            }
        }
    }

    vector<int> Board::ponderMCTS(int seconds)
    {
        cout << "1" << endl;
        Node root(nullptr, vector<int>(), currentPlayer);
        root.board->setState(getState());
        root.expand();

        auto finish = chrono::system_clock::now() + chrono::seconds(seconds);

        cout << "2" << endl;

        Node *current = &root;
        cout << "3" << endl;
        do
        {

            if (current->isLeaf())
            {
                cout << "leaf" << endl;
                if (current->visits == 0)
                {
                    cout << "rollout" << endl;
                    current->rollout();
                    current = &root;
                }
                else
                {
                    cout << "expand" << endl;
                    current->expand();
                    if (current->children.size() > 0) // If node is final AND win, wtf do I do?
                    {
                        current = &current->children[0];
                    }
                }
            }
            else
            {
                cout << "select uct" << endl;
                float uctScore = -FLT_MAX;
                int index = 0;
                // for (Node child : current->children)
                for (int k = 0; k < current->children.size(); k++)
                {
                    Node child = current->children[k];
                    if (child.visits == 0)
                    {
                        current = &child;
                        break;
                    }
                    float childScore = _UCT(child.score, child.visits, root.visits);
                    if (childScore > uctScore)
                    {
                        index = k;
                        uctScore = childScore;
                    }
                }
                current = &current->children[index];
            }
        } while (chrono::system_clock::now() < finish);


        // Get child with max visits from root
        int maxVisits = 0;
        int index = 0;
        for (int k = 0; k < root.children.size(); k++)
        {
            if (root.children[k].visits > maxVisits)
            {
                index = k;
                maxVisits = root.children[k].visits;
            }
        }
        
        // delete current, protect root !!!;
        delete current;
        // Copy the vector
        return root.children[index].move;

        // return vector<int>({0,0,0,0,0,0});
    }

    // Plays a move and returns it
    vector<int> Board::playMCTS(int seconds)
    {
        // Calculate move
        vector<int> move = ponderMCTS(1);
        // Apply move
        playManual(move);
        return move;
    }


}
