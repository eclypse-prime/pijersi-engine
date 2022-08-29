#ifndef PIECE_H
#define PIECE_H

namespace PijersiEngine
{
    // Piece data is stored as uint8_t (1 byte)
    // It uses the following structure :
    // TTCPTTCP
    // The last 4 bits represent the top element of the piece, and the first 4, which can be empty represent the bottom piece if there is one
    // TT are 2 bits representing the type of the piece (Scissors, Paper, Rock, Wise)
    // C is 1 bit representing the color
    // P is 1 bit set to 1 as long as there is a piece

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
}
#endif