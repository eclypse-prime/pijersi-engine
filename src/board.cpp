#include <cfloat>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include <alphabeta.hpp>
#include <board.hpp>
#include <logic.hpp>
#include <rng.hpp>
#include <utils.hpp>

using std::cout;
using std::endl;
using std::string;
using std::vector;

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
        if (colour == Black)
        {
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

    /* Plays a move using alphabeta minimax algorithm of chosen depth.
    If a duration is provided, it will search until that time is over.
    In that case, the engine will not play and the function will return a null move. */
    uint32_t Board::playDepth(int recursionDepth, bool random, uint32_t principalVariation, uint64_t searchTimeMilliseconds, bool iterative)
    {

        // Calculate move
        uint32_t move = searchDepth(recursionDepth, random, principalVariation, searchTimeMilliseconds, iterative);
        if (move != NULL_MOVE)
        {
            playManual(move);
        }
        return move;
    }

    /* Calculates a move using alphabeta minimax algorithm of chosen depth.
    If a duration is provided, it will search until that time is over.
    In that case, the function will return a null move. */
    uint32_t Board::searchDepth(int recursionDepth, bool random, uint32_t principalVariation, uint64_t searchTimeMilliseconds, bool iterative)
    {
        // Calculate finish time point
        time_point<steady_clock> finishTime;
        if (searchTimeMilliseconds == UINT64_MAX)
        {
            finishTime = time_point<steady_clock>::max();
        }
        else
        {
            finishTime = steady_clock::now() + std::chrono::milliseconds(searchTimeMilliseconds);
        }

        uint32_t move = NULL_MOVE;
        if (iterative)
        {
            for (int depth = 1; depth <= recursionDepth; depth++)
            {
                uint32_t proposedMove = AlphaBeta::ponderAlphaBeta(depth, random, cells, currentPlayer, move, finishTime);
                if (proposedMove != NULL_MOVE)
                {
                    move = proposedMove;
                }
            }
        }
        else
        {
            move = AlphaBeta::ponderAlphaBeta(recursionDepth, random, cells, currentPlayer, principalVariation, finishTime);
        }
        return move;
    }

    /* Plays a move using alphabeta minimax algorithm. The engine will search for the provided duration in milliseconds.
    The engine will then return the best move found during that timeframe.
    If no move is found, the engine will not play and the function will return a null move. */
    uint32_t Board::playTime(bool random, uint64_t searchTimeMilliseconds)
    {
        // Calculate move
        uint32_t move = searchTime(random, searchTimeMilliseconds);
        if (move != NULL_MOVE)
        {
            playManual(move);
        }
        return move;
    }

    /* Calculates a move using alphabeta minimax algorithm. The engine will search for the provided duration in milliseconds.
    The engine will then return the best move found during that timeframe.
    If no move is found, the engine will return a null move. */
    uint32_t Board::searchTime(bool random, uint64_t searchTimeMilliseconds)
    {
        int recursionDepth = 1;

        // Calculate finish time point
        time_point<steady_clock> finishTime;
        finishTime = steady_clock::now() + std::chrono::milliseconds(searchTimeMilliseconds);

        uint32_t move = NULL_MOVE;

        while (steady_clock::now() < finishTime)
        {
            uint32_t proposedMove = AlphaBeta::ponderAlphaBeta(recursionDepth, random, cells, currentPlayer, move, finishTime);
            if (proposedMove != NULL_MOVE)
            {
                move = proposedMove;
            }
            recursionDepth += 1;
        }
        return move;
    }

    // Chooses a random move
    uint32_t Board::searchRandom()
    {
        return Logic::searchRandom(cells, currentPlayer);
    }

    // Plays a random move and returns it
    uint32_t Board::playRandom()
    {
        return Logic::playRandom(cells, currentPlayer);
    }

    bool Board::isMoveLegal(uint32_t move)
    {
        uint32_t indexStart = move & 0x000000FF;
        if (cells[indexStart] == 0)
        {
            return false;
        }
        if ((cells[indexStart] & 2) != currentPlayer << 1)
        {
            return false;
        }
        vector<uint32_t> moves = Logic::availablePieceMoves(indexStart, cells);
        for (size_t k = 0; k < moves.size(); k++)
        {
            if (move == moves[k])
            {
                return true;
            }
        }
        return false;
    }

    void Board::playManual(vector<uint32_t> move)
    {
        Logic::play(move[0], move[1], move[2], cells);
        endTurn();
    }

    void Board::playManual(uint32_t move)
    {
        Logic::playManual(move, cells);
        endTurn();
    }

    void Board::playManual(string moveString)
    {
        uint32_t move = Logic::stringToMove(moveString, cells);
        Logic::playManual(move, cells);
        endTurn();
    }

    uint8_t Board::at(int i, int j)
    {
        return cells[Logic::coordsToIndex(i, j)];
    }

    int16_t Board::evaluate()
    {
        return AlphaBeta::evaluatePosition(cells);
    }

    // Adds a piece to the designated coordinates
    void Board::addPiece(uint8_t piece, int i, int j)
    {
        cells[Logic::coordsToIndex(i, j)] = piece;
    }

    // Sets the board to a chosen state
    void Board::setState(uint8_t newState[45])
    {
        Logic::setState(cells, newState);
    }

    uint8_t *Board::getState()
    {
        return cells;
    }

    void Board::setStringState(string stateString)
    {
        vector<string> stateWords = Utils::split(stateString, "_");
        Logic::stringToCells(stateWords[0], cells);
        currentPlayer = (stateWords[1] == "w") ? 0U : 1U;
        lastPieceCount = countPieces();
        // TODO: set half move counter and move counter
    }

    string Board::getStringState()
    {
        string cellsString = Logic::cellsToString(cells);
        string playerString = (currentPlayer == 0U) ? "w" : "b";

        return cellsString + "_" + playerString;
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
        addPiece(addBottom(createPiece(Black, Wise), createPiece(Black, Wise)), 1, 3);
        addPiece(createPiece(Black, Rock), 1, 4);
        addPiece(createPiece(Black, Scissors), 1, 5);
        addPiece(createPiece(Black, Paper), 1, 6);

        // White pieces
        addPiece(createPiece(White, Paper), 5, 0);
        addPiece(createPiece(White, Scissors), 5, 1);
        addPiece(createPiece(White, Rock), 5, 2);
        addPiece(addBottom(createPiece(White, Wise), createPiece(White, Wise)), 5, 3);
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

        halfMoves = 0;
        moves = 1;

        lastPieceCount = countPieces();
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
            int nColumns;
            if (i % 2 == 0)
            {
                nColumns = 6;
                output += ' ';
            }
            else
            {
                nColumns = 7;
            }

            for (int j = 0; j < nColumns; j++)
            {
                char char1 = '.';
                char char2 = ' ';
                uint8_t piece = cells[Logic::coordsToIndex(i, j)];
                if (piece != 0)
                {
                    char1 = Logic::pieceToChar[piece & 0x0FU];
                    char2 = Logic::pieceToChar[piece >> 4];
                }
                output += char1;
                output += char2;
            }
            output += '\n';
        }
        return output;
    }

    // Returns true if the board is in a winning position
    bool Board::checkWin()
    {
        return Logic::isWin(cells);
    }

    bool Board::checkDraw()
    {
        return halfMoves >= 20;
    }

    // TODO
    bool Board::checkStalemate()
    {
        return false;
    }

    uint8_t Board::getWinner()
    {
        // TODO refactor function name below
        return Logic::winningPlayer(cells);
    }

    int16_t Board::getForecast()
    {
        return forecast;
    }

    // uint32_t Board::ponderMCTS(int seconds, int simulationsPerRollout)
    // {
    //     return MCTS::ponderMCTS(seconds, simulationsPerRollout, cells, currentPlayer);
    // }

    // // Plays a move and returns it
    // uint32_t Board::playMCTS(int seconds, int simulationsPerRollout)
    // {
    //     // Calculate move
    //     uint32_t move = ponderMCTS(seconds, simulationsPerRollout);
    //     // Apply move
    //     playManual(move);
    //     return move;
    // }

    string Board::advice(int recursionDepth, bool random)
    {
        uint32_t move = searchDepth(recursionDepth, random);
        string moveString = Logic::moveToString(move, cells);
        return moveString;
    }

    uint32_t Board::countPieces()
    {
        uint32_t count = 0U;
        for (size_t index = 0; index < 45; index++)
        {
            if (cells[index] >= 16)
            {
                count += 2;
            }
            else if (cells[index] >= 1)
            {
                count += 1;
            }
        }
        return count;
    }

    void Board::endTurn()
    {
        currentPlayer = 1U - currentPlayer;
        uint32_t pieceCount = countPieces();
        if (lastPieceCount != pieceCount)
        {
            lastPieceCount = pieceCount;
            halfMoves = 0;
        }
        else
        {
            halfMoves += 1;
        }
    }

}
