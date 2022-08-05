#include <piece.h>

namespace PijersiEngine
{
    Piece::Piece(PieceColour newColour, PieceType newType, int newI, int newJ)
    {
        colour = newColour;
        type = newType;
        i = newI;
        j = newJ;
        bottom = nullptr;
    }

    PieceType Piece::getType()
    {
        return type;
    }

    int Piece::getI()
    {
        return i;
    }

    int Piece::getJ()
    {
        return j;
    }

    void Piece::setCoords(int newI, int newJ)
    {
        i = newI;
        j = newJ;
    }

    Piece *Piece::getBottom()
    {
        return bottom;
    }

    void Piece::setBottom(Piece *newBottom)
    {
        bottom = newBottom;
    }

    Piece::~Piece()
    {
        if (bottom != nullptr)
        {
            delete bottom;
        }
    }

}