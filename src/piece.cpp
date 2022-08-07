#include <piece.h>

namespace PijersiEngine
{
    Piece::Piece(PieceColour newColour, PieceType newType)
    {
        colour = newColour;
        type = newType;
        bottom = nullptr;
    }

    Piece::~Piece()
    {
        if (bottom != nullptr)
        {
            delete bottom;
        }
    }

}