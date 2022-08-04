#include <piece.h>

namespace PijersiEngine
{
    Piece::Piece(Type newType)
    {
        type = newType;
    }

    Type Piece::getType()
    {
        return type;
    }

}