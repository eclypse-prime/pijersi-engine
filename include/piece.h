#ifndef PIECE_H
#define PIECE_H

namespace PijersiEngine
{
    enum PieceType
    {
        Scissors,
        Paper,
        Rock,
        Wise
    };

    enum PieceColour
    {
        White,
        Black
    };

    struct Piece
    {
        Piece(PieceColour newColour, PieceType newType);
        PieceColour colour;
        PieceType type;
        Piece *bottom;
        ~Piece();
    };
}
#endif