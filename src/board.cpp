#include <board.h>

namespace PijersiEngine
{
    void Board::playManual(int move[6])
    {
        Piece *piece = cells[coordsToIndex(move[0], move[1])];
        if (piece != nullptr)
        {
            cells[coordsToIndex(move[0], move[1])] = nullptr;
            cells[coordsToIndex(move[4], move[5])] = piece;
            piece->setCoords(move[4], move[5]);
        }
    }

    int *Board::playAuto()
    {
        int *move = new int[6];
        return move;
    }

    int Board::coordsToIndex(int i, int j)
    {
        int index;
        if (i % 2 == 0)
        {
            index = 15 * i / 2 + j;
        }
        else
        {
            index = 8 + 15 * (i - 1) / 2 + j;
        }
        return index;
    }

    Piece *Board::at(int i, int j)
    {
        return cells[coordsToIndex(i, j)];
    }

    void Board::addPiece(Piece *piece)
    {
        int i = piece->getJ();
        int j = piece->getI();
        cells[coordsToIndex(i, j)] = piece;
    }

    void Board::init()
    {
        // Black pieces
        addPiece(new Piece(Black, Scissors, 0, 0));
        addPiece(new Piece(Black, Paper, 0, 1));
        addPiece(new Piece(Black, Rock, 0, 2));
        addPiece(new Piece(Black, Scissors, 0, 3));
        addPiece(new Piece(Black, Paper, 0, 4));
        addPiece(new Piece(Black, Rock, 0, 5));
        addPiece(new Piece(Black, Paper, 1, 0));
        addPiece(new Piece(Black, Rock, 1, 1));
        addPiece(new Piece(Black, Scissors, 1, 2));
        addPiece(new Piece(Black, Wise, 1, 3));
        addPiece(new Piece(Black, Rock, 1, 4));
        addPiece(new Piece(Black, Scissors, 1, 5));
        addPiece(new Piece(Black, Paper, 1, 6));

        // White pieces
        addPiece(new Piece(White, Paper, 5, 0));
        addPiece(new Piece(White, Scissors, 5, 1));
        addPiece(new Piece(White, Rock, 5, 2));
        addPiece(new Piece(White, Wise, 5, 3));
        addPiece(new Piece(White, Scissors, 5, 4));
        addPiece(new Piece(White, Rock, 5, 5));
        addPiece(new Piece(White, Paper, 5, 6));
        addPiece(new Piece(White, Rock, 6, 0));
        addPiece(new Piece(White, Paper, 6, 1));
        addPiece(new Piece(White, Scissors, 6, 2));
        addPiece(new Piece(White, Rock, 6, 3));
        addPiece(new Piece(White, Paper, 6, 4));
        addPiece(new Piece(White, Scissors, 6, 5));
    }

}