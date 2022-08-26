#include <board.h>
#include <iostream>
#include <string>
#include <omp.h>
#include <algorithm>
#include <random>
#include <limits>

using namespace std;

namespace PijersiEngine
{

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

    uint8_t addBottom(uint8_t piece, uint8_t newBottom)
    {
        return piece + (newBottom << 4);
    }

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

    Board::Board()
    {
        for (int k = 0; k < 45; k++)
        {
            cells[k] = 0;
        }
    }

    Board::Board(Board &board)
    {
        setState(board.cells);
        currentPlayer = board.currentPlayer;
    }

    // Board::~Board()
    // {
    // }

    void Board::playManual(vector<int> move)
    {
        play(move[0], move[1], move[2], move[3], move[4], move[5]);
    }

    void Board::playManual(int move[6])
    {
        play(move[0], move[1], move[2], move[3], move[4], move[5]);
    }

    // Minimax, implement alphabeta later
    vector<int> Board::playAuto(int recursionDepth)
    {
        vector<int> moves = vector<int>();
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != 0)
            {
                if ((cells[k] & 2) == (static_cast<PieceColour>(currentPlayer) << 1))
                {
                    int i, j;
                    indexToCoords(k, &i, &j);
                    vector<int> pieceMoves = availableMoves(i, j);
                    moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
                }
            }
        }

        if (moves.size() > 0)
        {

            int index = 0;
            int *extremums = new int[moves.size() / 6];

            int alpha = INT_MIN;
            int beta = INT_MAX;
            int extremum = (currentPlayer == White) ? INT_MIN : INT_MAX;

            #pragma omp parallel
            {
                #pragma omp for
                for (int k = 0; k < moves.size() / 6; k++)
                {
                    extremums[k] = evaluateMove(moves.data() + 6 * k, recursionDepth, alpha, beta);
                }

                if (currentPlayer == White)
                {
                    #pragma omp for reduction(max : extremum)
                    for (int k = 0; k < moves.size() / 6; k++)
                    {
                        if ((extremums[k] > extremum))
                        {
                            extremum = extremums[k];
                        }
                    }
                }
                else
                {
                    #pragma omp for reduction(min : extremum)
                    for (int k = 0; k < moves.size() / 6; k++)
                    {
                        if (extremums[k] < extremum)
                        {
                            extremum = extremums[k];
                        }
                    }
                }
            }

            for (int k = 0; k < moves.size() / 6; k++)
            {
                if (extremums[k] == extremum)
                {
                    index = k;
                    break;
                }
            }

            delete extremums;

            vector<int>::const_iterator first = moves.begin() + 6 * index;
            vector<int>::const_iterator last = moves.begin() + 6 * (index + 1);
            vector<int> move(first, last);
            playManual(move);
            return move;
        }

        vector<int> move = vector<int>(6);
        return move;
    }

    void Board::move(int iStart, int jStart, int iEnd, int jEnd)
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

    void Board::stack(int iStart, int jStart, int iEnd, int jEnd)
    {
        uint8_t movingPiece = cells[coordsToIndex(iStart, jStart)];
        uint8_t endPiece = cells[coordsToIndex(iEnd, jEnd)];

        // If the moving piece is already on top of a stack, leave the bottom piece in the starting cell
        cells[coordsToIndex(iStart, jStart)] = (movingPiece >> 4);

        // Move the top piece to the target cell and set its new bottom piece
        cells[coordsToIndex(iEnd, jEnd)] = (movingPiece & 15) + (endPiece << 4);
    }

    void Board::unstack(int iStart, int jStart, int iEnd, int jEnd)
    {
        uint8_t movingPiece = cells[coordsToIndex(iStart, jStart)];

        // Leave the bottom piece in the starting cell
        cells[coordsToIndex(iStart, jStart)] = (movingPiece >> 4);
        // Remove the bottom piece from the moving piece
        // Move the top piece to the target cell
        // Will overwrite the eaten piece if there is one
        cells[coordsToIndex(iEnd, jEnd)] = (movingPiece & 15);
    }

    void Board::play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd)
    {
        uint8_t movingPiece = cells[coordsToIndex(iStart, jStart)];
        if (movingPiece != 0)
        {
            // If there is no intermediate move
            if (iMid < 0 || jMid < 0)
            {
                // Simple move
                move(iStart, jStart, iEnd, jEnd);
            }
            // There is an intermediate move
            else
            {
                uint8_t midPiece = cells[coordsToIndex(iMid, jMid)];
                uint8_t endPiece = cells[coordsToIndex(iEnd, jEnd)];
                // The piece at the mid coordinates is an ally : stack and move
                if (midPiece != 0 && (midPiece & 2) == (movingPiece & 2) && (iMid != iStart || jMid != jStart))
                {
                    stack(iStart, jStart, iMid, jMid);
                    move(iMid, jMid, iEnd, jEnd);
                }
                // The piece at the end coordinates is an ally : move and stack
                else if (endPiece != 0 && (endPiece & 2) == (movingPiece & 2))
                {
                    move(iStart, jStart, iMid, jMid);
                    stack(iMid, jMid, iEnd, jEnd);
                }
                // The end coordinates contain an enemy or no piece : move and unstack
                else
                {
                    move(iStart, jStart, iMid, jMid);
                    unstack(iMid, jMid, iEnd, jEnd);
                }
            }
        }
        if ((movingPiece & 2) == 0)
        {
            currentPlayer = Black;
        }
        else
        {
            currentPlayer = White;
        }
    }

    uint8_t Board::at(int i, int j)
    {
        return cells[coordsToIndex(i, j)];
    }

    int evaluatePiece(uint8_t piece, int i)
    {
        int score;
        if ((piece & 2) == 0)
        {
            score = 8;
            // If piece is not Wise
            if ((piece & 12) != 12)
            {
                score = score + 7 - i;
            }
            // If piece is on a stack
            if (piece >= 16)
            {
                score = score * 2 + 3;
            }
            // If piece is on a winning area and not Wise
            if (i == 0 && (piece & 12) != 12)
            {
                score = score * 10000;
            }
        }
        else
        {
            score = -8;
            // If piece is not Wise
            if ((piece & 12) != 12)
            {
                score = score -i - 1;
            }
            // If piece is on a stack
            if (piece >= 16)
            {
                score = score * 2 - 3;
            }
            // If piece is on a winning area and not Wise
            if (i == 6 && (piece & 12) != 12)
            {
                score = score * 10000;
            }
        }
        return score;
    }

    int Board::evaluate()
    {
        int score = 0;
        for (int i = 0; i < 7; i++)
        {
            if (i % 2 == 0)
            {
                for (int j = 0; j < 6; j++)
                {
                    uint8_t piece = cells[coordsToIndex(i, j)];
                    if (piece != 0)
                    {
                        score += evaluatePiece(piece, i);
                    }
                }
            }
            else
            {
                for (int j = 0; j < 7; j++)
                {
                    uint8_t piece = cells[coordsToIndex(i, j)];
                    if (piece != 0)
                    {
                        score += evaluatePiece(piece, i);
                    }
                }
            }
        }
        return score;
    }

    void Board::addPiece(uint8_t piece, int i, int j)
    {
        cells[coordsToIndex(i, j)] = piece;
    }

    void Board::setState(uint8_t newState[45])
    {
        for (int k = 0; k < 45; k++)
        {
            cells[k] = newState[k];
        }
    }

    void Board::init()
    {
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
    }

    char pieceToChar(uint8_t piece)
    {
        char res = ' ';
        if ((piece & 2) == 0)
        {
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
        else if ((piece & 2) == 2)
        {
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

    void Board::print()
    {
        cout << toString();
    }

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

    bool Board::checkWin()
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

    vector<int> Board::neighbours(int index)
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

    vector<int> Board::neighbours2(int index)
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

    // bool canTake(PieceType source, PieceType target)
    // {
    //     if (source == Scissors && target == Paper || source == Paper && target == Rock || source == Rock && target == Scissors)
    //     {
    //         return true;
    //     }
    //     return false;
    // }
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

    bool Board::isMoveValid(uint8_t movingPiece, int indexEnd)
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

    bool Board::isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd)
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

    bool Board::isStackValid(uint8_t movingPiece, int indexEnd)
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

    bool Board::isUnstackValid(uint8_t movingPiece, int indexEnd)
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

    vector<int> Board::availableMoves(int iStart, int jStart)
    {
        int indexStart = coordsToIndex(iStart, jStart);

        uint8_t movingPiece = cells[indexStart];

        vector<int> moves = vector<int>();

        // If the piece is not a stack
        if (movingPiece < 16)
        {
            // 1-range first action
            for (int indexMid : neighbours(indexStart))
            {
                // 1-range move
                if (isMoveValid(movingPiece, indexMid))
                {
                    int iEnd, jEnd;
                    indexToCoords(indexMid, &iEnd, &jEnd);
                    moves.insert(moves.end(), {iStart, jStart, -1, -1, iEnd, jEnd});
                }
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid))
                {
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);

                    // stack only
                    moves.insert(moves.end(), {iStart, jStart, iStart, jStart, iMid, jMid});

                    // stack, 0/1-range move
                    for (int indexEnd : neighbours(indexMid))
                    {
                        if (isMoveValid(movingPiece, indexEnd))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }

                    // stack, 2-range move
                    for (int indexEnd : neighbours2(indexMid))
                    {
                        if (isMove2Valid(movingPiece, indexMid, indexEnd))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }
                }
            }
        }
        else
        {
            // 1-range first action
            for (int indexMid : neighbours(indexStart))
            {

                // 1-range move, [stack or unstack] optional
                if (isMoveValid(movingPiece, indexMid))
                {
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);

                    // 1-range move
                    moves.insert(moves.end(), {iStart, jStart, -1, -1, iMid, jMid});

                    // 1-range move, stack or unstack
                    for (int indexEnd : neighbours(indexMid))
                    {
                        // 1-range move, stack
                        if (isStackValid(movingPiece, indexEnd))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }

                        // 1-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }
                }
                // stack, [1/2-range move] optional
                if (isStackValid(movingPiece, indexMid))
                {
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);
                    // stack, 1-range move
                    for (int indexEnd : neighbours(indexMid))
                    {
                        if (isMoveValid(movingPiece, indexEnd))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }

                    // stack, 2-range move
                    for (int indexEnd : neighbours2(indexMid))
                    {
                        if (isMove2Valid(movingPiece, indexMid, indexEnd))
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
                if (isUnstackValid(movingPiece, indexMid))
                {
                    // unstack only
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);
                    moves.insert(moves.end(), {iStart, jStart, iStart, jStart, iMid, jMid});
                }
            }

            // 2 range first action
            for (int indexMid : neighbours2(indexStart))
            {
                if (isMove2Valid(movingPiece, indexStart, indexMid))
                {
                    int iMid, jMid;
                    indexToCoords(indexMid, &iMid, &jMid);

                    // 2-range move
                    moves.insert(moves.end(), {iStart, jStart, -1, -1, iMid, jMid});

                    // 2-range move, stack or unstack
                    for (int indexEnd : neighbours(indexMid))
                    {
                        // 2-range move, stack
                        if (isStackValid(movingPiece, indexEnd))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }

                        // 2-range move, unstack
                        if (isUnstackValid(movingPiece, indexEnd))
                        {
                            int iEnd, jEnd;
                            indexToCoords(indexEnd, &iEnd, &jEnd);
                            moves.insert(moves.end(), {iStart, jStart, iMid, jMid, iEnd, jEnd});
                        }
                    }
                }
            }
        }

        return moves;
    }

    int Board::evaluateMove(int move[6], int recursionDepth, int alpha, int beta)
    {
        Board *newBoard = new Board(*this);
        newBoard->playManual(move);

        int score;

        if (newBoard->checkWin() || recursionDepth <= 0)
        {
            score = newBoard->evaluate();
            delete newBoard;
            return score;
        }

        vector<int> moves = vector<int>();
        for (int k = 0; k < 45; k++)
        {
            if (newBoard->cells[k] != 0 && (newBoard->cells[k] & 2) == (static_cast<PieceColour>(newBoard->currentPlayer) << 1))
            {
                int i, j;
                indexToCoords(k, &i, &j);
                vector<int> pieceMoves = newBoard->availableMoves(i, j);
                moves.insert(moves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
        if (moves.size() > 0)
        {
            if (newBoard->currentPlayer == White)
            {
                int maximum = INT_MIN;
                for (int k = 0; k < moves.size() / 6; k++)
                {
                    maximum = max(maximum, newBoard->evaluateMove(moves.data() + 6 * k, recursionDepth - 1, alpha, beta));
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
                int minimum = INT_MAX;
                for (int k = 0; k < moves.size() / 6; k++)
                {
                    minimum = min(minimum, newBoard->evaluateMove(moves.data() + 6 * k, recursionDepth - 1, alpha, beta));
                    if (minimum < alpha)
                    {
                        break;
                    }
                    beta = min(beta, minimum);
                }
                score = minimum;
            }
        }
        delete newBoard;

        return score;
    }

}
