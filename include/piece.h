#ifndef PIECE_H
#define PIECE_H

namespace PijersiEngine
{
    enum Type
    {
        Scissors,
        Paper,
        Rock,
        Wise
    };

    class Piece
    {
        public:
            Piece(Type type);
            Type getType();
        
        private:
            Type type;
    };
}
#endif