#include <board.h>
#include <iostream>
#include <string>

using namespace std;

namespace PijersiEngine
{
    Board::Board()
    {
        for (int k = 0; k < 45; k++)
        {
            cells[k] = nullptr;
        }
    }

    void Board::playManual(int move[6])
    {
        play(move[0], move[1], move[2], move[3], move[4], move[5]);
    }

    int *Board::playAuto()
    {
        int *move = new int[6]();
        return move;
    }

    int Board::coordsToIndex(int i, int j)
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

    void Board::move(int iStart, int jStart, int iEnd, int jEnd)
    {
        // Do nothing if start and end coordinate are identical
        if (iStart != iEnd || jStart != jEnd)
        {
            Piece *movingPiece = cells[coordsToIndex(iStart, jStart)];

            // Delete the eaten piece if there is one
            Piece *endPiece = cells[coordsToIndex(iEnd, jEnd)];
            if (endPiece != nullptr)
            {
                delete endPiece;
            }

            // Move the piece to the target cell
            cells[coordsToIndex(iEnd, jEnd)] = movingPiece;
            // Set the starting cell as empty
            cells[coordsToIndex(iStart, jStart)] = nullptr;
        }
    }

    void Board::stack(int iStart, int jStart, int iEnd, int jEnd)
    {
        Piece *movingPiece = cells[coordsToIndex(iStart, jStart)];
        Piece *endPiece = cells[coordsToIndex(iEnd, jEnd)];

        // If the moving piece is already on top of a stack, leave the bottom piece in the starting cell
        // Else, set the starting cell as empty
        Piece *bottomPiece = movingPiece->bottom;
        if (movingPiece->bottom != nullptr)
        {
            cells[coordsToIndex(iStart, jStart)] = bottomPiece;
        }
        else
        {
            cells[coordsToIndex(iStart, jStart)] = nullptr;
        }

        // Move the top piece to the target cell and set its new bottom piece
        cells[coordsToIndex(iEnd, jEnd)] = movingPiece;
        movingPiece->bottom = endPiece;
    }

    void Board::unstack(int iStart, int jStart, int iEnd, int jEnd)
    {
        Piece *movingPiece = cells[coordsToIndex(iStart, jStart)];

        // Delete the eaten piece if there is one
        Piece *endPiece = cells[coordsToIndex(iEnd, jEnd)];
        if (endPiece != nullptr)
        {
            delete endPiece;
        }
        // Leave the bottom piece in the starting cell
        cells[coordsToIndex(iStart, jStart)] = movingPiece->bottom;
        // Remove the bottom piece from the moving piece
        movingPiece->bottom = nullptr;
        // Move the top piece to the target cell
        cells[coordsToIndex(iEnd, jEnd)] = movingPiece;
    }

    void Board::play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd)
    {
        Piece *movingPiece = cells[coordsToIndex(iStart, jStart)];
        if (movingPiece != nullptr)
        {
            // If there is no intermediate move
            if (iMid < 0 || jMid < 0)
            {
                Piece *endPiece = cells[coordsToIndex(iEnd, jEnd)];
                if (endPiece != nullptr && endPiece->colour == movingPiece->colour)
                {
                    // Simple move
                    move(iStart, jStart, iEnd, jEnd);
                }
            }
            // There is an intermediate move
            else
            {
                Piece *midPiece = cells[coordsToIndex(iMid, jMid)];
                Piece *endPiece = cells[coordsToIndex(iEnd, jEnd)];
                // The piece at the mid coordinates is an ally : stack and move
                if (midPiece != nullptr && midPiece->colour == movingPiece->colour && (iMid != iStart || jMid != jStart))
                {
                    stack(iStart, jStart, iMid, jMid);
                    move(iMid, jMid, iEnd, jEnd);
                }
                // The piece at the end coordinates is an ally : move and stack
                else if (endPiece != nullptr && endPiece->colour == movingPiece->colour)
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
        if (movingPiece->colour == White)
        {
            currentPlayer = Black;
        }
        else
        {
            currentPlayer = White;
        }
    }

    Piece *Board::at(int i, int j)
    {
        return cells[coordsToIndex(i, j)];
    }

    int evaluatePiece(Piece *piece, int i)
    {
        int score;
        if (piece->colour == White)
        {
            score = 7 - i;
            if (piece->bottom != nullptr)
            {
                score = score * 2 + 3;
            }
            if (i == 0)
            {
                score = score * 100;
            }
        }
        else
        {
            score = -i - 1;
            if (piece->bottom != nullptr)
            {
                score = score * 2 - 3;
            }
            if (i == 6)
            {
                score = score * 100;
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
                    Piece *piece = cells[coordsToIndex(i, j)];
                    if (piece != nullptr)
                    {
                        score += evaluatePiece(piece, i);
                    }
                }
            }
            else
            {
                for (int j = 0; j < 7; j++)
                {
                    Piece *piece = cells[coordsToIndex(i, j)];
                    if (piece != nullptr)
                    {
                        score += evaluatePiece(piece, i);
                    }
                }
            }
        }
        return score;
    }

    void Board::addPiece(Piece *piece, int i, int j)
    {
        cells[coordsToIndex(i, j)] = piece;
    }

    void Board::setState(int colours[45], int tops[45], int bottoms[45])
    {
        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != nullptr)
            {
                delete cells[k];
                cells[k] = nullptr;
            }
        }
        for (int k = 0; k < 45; k++)
        {
            if (colours[k] != -1 && tops[k] != -1)
            {
                PieceColour colour = static_cast<PieceColour>(colours[k]);
                PieceType topType = static_cast<PieceType>(tops[k]);
                cells[k] = new Piece(colour, topType);

                if (bottoms[k] != -1)
                {
                    PieceType bottomType = static_cast<PieceType>(bottoms[k]);
                    cells[k]->bottom = new Piece(colour, bottomType);
                }
            }
        }
    }

    void Board::init()
    {
        // Black pieces
        addPiece(new Piece(Black, Scissors), 0, 0);
        addPiece(new Piece(Black, Paper), 0, 1);
        addPiece(new Piece(Black, Rock), 0, 2);
        addPiece(new Piece(Black, Scissors), 0, 3);
        addPiece(new Piece(Black, Paper), 0, 4);
        addPiece(new Piece(Black, Rock), 0, 5);
        addPiece(new Piece(Black, Paper), 1, 0);
        addPiece(new Piece(Black, Rock), 1, 1);
        addPiece(new Piece(Black, Scissors), 1, 2);
        addPiece(new Piece(Black, Wise), 1, 3);
        addPiece(new Piece(Black, Rock), 1, 4);
        addPiece(new Piece(Black, Scissors), 1, 5);
        addPiece(new Piece(Black, Paper), 1, 6);
        cells[coordsToIndex(1, 3)]->bottom = new Piece(Black, Wise);

        // White pieces
        addPiece(new Piece(White, Paper), 5, 0);
        addPiece(new Piece(White, Scissors), 5, 1);
        addPiece(new Piece(White, Rock), 5, 2);
        addPiece(new Piece(White, Wise), 5, 3);
        addPiece(new Piece(White, Scissors), 5, 4);
        addPiece(new Piece(White, Rock), 5, 5);
        addPiece(new Piece(White, Paper), 5, 6);
        addPiece(new Piece(White, Rock), 6, 0);
        addPiece(new Piece(White, Paper), 6, 1);
        addPiece(new Piece(White, Scissors), 6, 2);
        addPiece(new Piece(White, Rock), 6, 3);
        addPiece(new Piece(White, Paper), 6, 4);
        addPiece(new Piece(White, Scissors), 6, 5);
        cells[coordsToIndex(5, 3)]->bottom = new Piece(White, Wise);
    }

    char pieceToChar(Piece *piece)
    {
        char res = ' ';
        if (piece->colour == White)
        {
            switch (piece->type)
            {
            case Scissors:
                res = 'S';
                break;
            case Paper:
                res = 'P';
                break;
            case Rock:
                res = 'R';
                break;
            case Wise:
                res = 'W';
                break;
            default:
                break;
            }
        }
        else if (piece->colour == Black)
        {
            switch (piece->type)
            {
            case Scissors:
                res = 's';
                break;
            case Paper:
                res = 'p';
                break;
            case Rock:
                res = 'r';
                break;
            case Wise:
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
                    char char1 = ' ';
                    char char2 = ' ';
                    Piece *piece = cells[coordsToIndex(i, j)];
                    if (piece != nullptr)
                    {
                        char1 = pieceToChar(piece);
                        if (piece->bottom != nullptr)
                        {
                            char2 = pieceToChar(piece->bottom);
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
                    char char1 = ' ';
                    char char2 = ' ';
                    Piece *piece = cells[coordsToIndex(i, j)];
                    if (piece != nullptr)
                    {
                        char1 = pieceToChar(piece);
                        if (piece->bottom != nullptr)
                        {
                            char2 = pieceToChar(piece->bottom);
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
            if (cells[k] != nullptr)
            {
                if (cells[k]->colour == White)
                {
                    return true;
                }
            }
        }
        for (int k = 39; k < 45; k++)
        {
            if (cells[k] != nullptr)
            {
                if (cells[k]->colour == White)
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
            return vector<int>({9, 21});
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
            return vector<int>({8, 22, 34});
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
            return vector<int>({21, 35});
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

    bool Board::isMoveValid(int indexStart, int indexEnd)
    {
        if (cells[indexEnd] != nullptr && cells[indexEnd]->colour == cells[indexStart]->colour){
            return false;
        }
        return true;
    }

    bool Board::isMove2Valid(int indexStart, int indexEnd)
    {
        if (cells[(indexEnd+indexStart)/2] != nullptr)
        {
            return false;
        }
        if (cells[indexEnd] != nullptr && cells[indexEnd]->colour == cells[indexStart]->colour){
            return false;
        }
        return true;
    }

    bool Board::isStackValid(int indexStart, int indexEnd)
    {
        if (cells[indexEnd] != nullptr && cells[indexEnd]->colour == cells[indexStart]->colour && cells[indexEnd]->bottom == nullptr)
        {
            return true;
        }
        return false;
    }

    bool Board::isUnstackValid(int indexStart, int indexEnd)
    {
        if (cells[indexEnd] == nullptr)
        {
            return true;
        }
        return false;
    }

    vector<int> Board::availableMoves(int i, int j)
    {
        return vector<int>();
    }
}
