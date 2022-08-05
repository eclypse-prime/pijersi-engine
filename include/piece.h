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

    class Piece
    {
    public:
        Piece(PieceColour newColour, PieceType newType, int newI, int newJ);
        PieceType getType();
        int getI();
        int getJ();
        void setCoords(int newI, int newJ);
        Piece *getBottom();
        void setBottom(Piece *newBottom);

        ~Piece();

    private:
        PieceColour colour;
        PieceType type;
        int i;
        int j;
        Piece *bottom;
    };
}
#endif