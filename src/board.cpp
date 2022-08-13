#include <board.h>
#include <iostream>
#include <string>

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
        std::cout << cells[coordsToIndex(i, j)]->colour << std::endl;
        return cells[coordsToIndex(i, j)];
    }

    int Board::evaluate()
    {
        return 0;
    }

    void Board::addPiece(Piece *piece, int i, int j)
    {
        // std::cout << piece->colour << pieceToChar(piece) << std::endl;
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
        std::cout << toString();
    }

    std::string Board::toString()
    {
        std::string output = "";
        for (int i = 0; i < 7; i++)
        {
            if (i % 2 == 0)
            {
                output +=  ' ';
                for (int j = 0; j < 6; j++)
                {
                    // std::cout << coordsToIndex(i,j) << std::endl;
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
                    // std::cout << coordsToIndex(i,j) << std::endl;
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

}