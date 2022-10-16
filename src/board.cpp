#include <cfloat>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include <alphabeta.hpp>
#include <board.hpp>
#include <logic.hpp>
#include <mcts.hpp>
#include <rng.hpp>

using namespace std;


namespace PijersiEngine
{


    // Adds a bottom piece to the selected piece
    uint8_t addBottom(uint8_t piece, uint8_t newBottom)
    {
        return piece + (newBottom << 4);
    }

    // Creates a piece of chosen colour and type
    uint8_t createPiece(PieceColour colour, PieceType type)
    {
        uint8_t piece = 1;
        if (colour == Black) {
            piece += 2;
        }
        switch (type)
        {
            case Scissors:
                break;
            case Paper:
                piece += 4;
                break;
            case Rock:
                piece += 8;
                break;
            case Wise:
                piece += 12;
                break;
        }
        return piece;
    }

    // Board constructor
    Board::Board()
    {
        for (int k = 0; k < 45; k++)
        {
            cells[k] = 0;
        }
    }

    // Board copy constructor
    Board::Board(Board &board)
    {
        setState(board.cells);
        currentPlayer = board.currentPlayer;
    }

    // Plays a move and returns it
    vector<int> Board::playAlphaBeta(int recursionDepth, bool random)
    {
        // Calculate move
        vector<int> move = ponderAlphaBeta(recursionDepth, random);
        // Apply move
        playManual(move);
        return move;
    }

    vector<int> Board::ponderAlphaBeta(int recursionDepth, bool random)
    {
        return _ponderAlphaBeta(recursionDepth, random, cells, currentPlayer);
    }

    // Chooses a random move
    vector<int> Board::ponderRandom()
    {
        return _ponderRandom(cells, currentPlayer);
    }

    // Plays a random move and returns it
    vector<int> Board::playRandom()
    {
        return _playRandom(cells, currentPlayer);
    }

    bool Board::isMoveLegal(vector<int> move)
    {
        if (cells[coordsToIndex(move[0], move[1])] == 0)
        {
            return false;
        }
        if ((cells[coordsToIndex(move[0], move[1])] & 2) != currentPlayer << 1)
        {
            return false;
        }
        vector<int> moves = _availablePieceMoves(move[0], move[1], cells);
        for (int k = 0; k < moves.size()/6; k++)
        {
            bool legal = true;
            for (int m = 0; m < 6; m++)
            {
                if (moves[k*6+m] != move[m])
                {
                    legal = false;
                }
            }
            if (legal)
            {
                return true;
            }
        }
        return false;
    }





    void Board::playManual(vector<int> move)
    {
        _play(move[0], move[1], move[2], move[3], move[4], move[5], cells);
        // Set current player to the other colour.
        currentPlayer = 1 - currentPlayer;
    }

    uint8_t Board::at(int i, int j)
    {
        return cells[coordsToIndex(i, j)];
    }

    int16_t Board::evaluate()
    {
        return _evaluatePosition(cells);
    }

    // Adds a piece to the designated coordinates
    void Board::addPiece(uint8_t piece, int i, int j)
    {
        cells[coordsToIndex(i, j)] = piece;
    }

    // Sets the board to a chosen state
    void Board::setState(uint8_t newState[45])
    {
        _setState(cells, newState);
    }

    uint8_t *Board::getState()
    {
        return cells;
    }

    // Initializes the board to the starting position
    void Board::init()
    {
        // Reset board
        for (int k = 0; k < 45; k++)
        {
            cells[k] = 0;
        }

        // Black pieces
        addPiece(createPiece(Black, Scissors), 0, 0);
        addPiece(createPiece(Black, Paper), 0, 1);
        addPiece(createPiece(Black, Rock), 0, 2);
        addPiece(createPiece(Black, Scissors), 0, 3);
        addPiece(createPiece(Black, Paper), 0, 4);
        addPiece(createPiece(Black, Rock), 0, 5);
        addPiece(createPiece(Black, Paper), 1, 0);
        addPiece(createPiece(Black, Rock), 1, 1);
        addPiece(createPiece(Black, Scissors), 1, 2);
        addPiece(addBottom(createPiece(Black, Wise),createPiece(Black, Wise)), 1, 3);
        addPiece(createPiece(Black, Rock), 1, 4);
        addPiece(createPiece(Black, Scissors), 1, 5);
        addPiece(createPiece(Black, Paper), 1, 6);

        // White pieces
        addPiece(createPiece(White, Paper), 5, 0);
        addPiece(createPiece(White, Scissors), 5, 1);
        addPiece(createPiece(White, Rock), 5, 2);
        addPiece(addBottom(createPiece(White, Wise),createPiece(White, Wise)), 5, 3);
        addPiece(createPiece(White, Scissors), 5, 4);
        addPiece(createPiece(White, Rock), 5, 5);
        addPiece(createPiece(White, Paper), 5, 6);
        addPiece(createPiece(White, Rock), 6, 0);
        addPiece(createPiece(White, Paper), 6, 1);
        addPiece(createPiece(White, Scissors), 6, 2);
        addPiece(createPiece(White, Rock), 6, 3);
        addPiece(createPiece(White, Paper), 6, 4);
        addPiece(createPiece(White, Scissors), 6, 5);

        // Set active player to White
        currentPlayer = 0;
    }

    // Converts a piece to char format
    // Used for debug purposes
    char _pieceToChar(uint8_t piece)
    {
        char res = ' ';
        // If the piece is White
        if ((piece & 2) == 0)
        {
            // Read the piece's type
            switch (piece & 12)
            {
            case 0:
                res = 'S';
                break;
            case 4:
                res = 'P';
                break;
            case 8:
                res = 'R';
                break;
            case 12:
                res = 'W';
                break;
            default:
                break;
            }
        }
        // If the piece is Black
        else if ((piece & 2) == 2)
        {
            // Read the piece's type
            switch (piece & 12)
            {
            case 0:
                res = 's';
                break;
            case 4:
                res = 'p';
                break;
            case 8:
                res = 'r';
                break;
            case 12:
                res = 'w';
                break;
            default:
                break;
            }
        }
        return res;
    }

    // Prints the board
    // Used for debug purposes
    void Board::print()
    {
        cout << toString();
    }

    // Returns the state of the board in string format
    // Used for debug purposes
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
                    char char1 = '.';
                    char char2 = ' ';
                    uint8_t piece = cells[coordsToIndex(i, j)];
                    if (piece != 0)
                    {
                        char1 = _pieceToChar(piece);
                        // If the piece is a stack
                        if (piece >= 16)
                        {
                            char2 = _pieceToChar(piece >> 4);
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
                    char char1 = '.';
                    char char2 = ' ';
                    uint8_t piece = cells[coordsToIndex(i, j)];
                    if (piece != 0)
                    {
                        char1 = _pieceToChar(piece);
                        // If the piece is a stack
                        if (piece >= 16)
                        {
                            char2 = _pieceToChar(piece >> 4);
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



    // Returns true if the board is in a winning position
    bool Board::checkWin()
    {
        return _checkWin(cells);
    }

    int16_t Board::getForecast()
    {
        return forecast;
    }

    vector<int> Board::ponderMCTS(int seconds, int simulationsPerRollout)
    {
        return _ponderMCTS(seconds, simulationsPerRollout, cells, currentPlayer);
    }

    // Plays a move and returns it
    vector<int> Board::playMCTS(int seconds, int simulationsPerRollout)
    {
        // Calculate move
        vector<int> move = ponderMCTS(seconds, simulationsPerRollout);
        // Apply move
        playManual(move);
        return move;
    }


}
