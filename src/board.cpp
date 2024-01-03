#include <cfloat>
#include <cstdint>
#include <chrono>
#include <iostream>
#include <random>
#include <string>

#include <alphabeta.hpp>
#include <board.hpp>
#include <logic.hpp>
#include <openings.hpp>
#include <options.hpp>
#include <rng.hpp>
#include <utils.hpp>

using namespace std::chrono;
using std::array;
using std::cout;
using std::endl;
using std::string;
using std::vector;

namespace PijersiEngine
{

    void printInfo(int recursionDepth, float duration, int predictedScore, string moveString)
    {
        cout << "info depth " << recursionDepth << " time " << duration << " score " << predictedScore << " pv " << moveString << endl;
    }

    uint32_t Board::searchBook()
    {
        string currentStringState = getStringState();
        if (Openings::book.contains(currentStringState))
        {
            uint32_t bookMove = Openings::book[currentStringState];
            string moveString = Logic::moveToString(bookMove, cells);
            // TODO: use saved depth instead of 6
            if (Options::verbose)
            {
                cout << "info depth 6 time 0 pv " << moveString << endl;
            }
            // TODO: save depth into bookMove leftmost bits, if search depth is higher than saved depth, recalculate
            // TODO: if using iterative deepening, restart from saved depth using it as prinicipal variation
            return bookMove;
        }
        else
        {
            return NULL_MOVE;
        }
    }

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
        Logic::setState(cells, board.cells);
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

        if (Options::openingBook)
        {
            uint32_t bookMove = searchBook();
            if (bookMove != NULL_MOVE)
            {
                return bookMove;
            }
        }

        uint32_t move = NULL_MOVE;
        if (iterative)
        {
            size_t nMoves = Logic::availablePlayerMoves(currentPlayer, cells)[MAX_PLAYER_MOVES - 1];
            int64_t *scores = new int64_t[nMoves];
            for (int depth = 1; depth <= recursionDepth; depth++)
            {
                auto start = steady_clock::now();
                uint32_t proposedMove = AlphaBeta::ponderAlphaBeta(depth, random, cells, currentPlayer, move, finishTime, scores);
                auto end = steady_clock::now();
                string moveString = Logic::moveToString(proposedMove, cells);
                float duration = (float)duration_cast<microseconds>(end - start).count()/1000;
                if (proposedMove != NULL_MOVE)
                {
                    if (Options::verbose)
                    {
                        printInfo(depth, duration, AlphaBeta::predictedScore, moveString);
                        if (AlphaBeta::predictedScore < -BASE_BETA)
                        {
                            cout << "info loss in " << depth / 2 << endl;
                            break;
                        }
                    }
                    move = proposedMove;
                    if (AlphaBeta::predictedScore > BASE_BETA)
                    {
                        if (Options::verbose)
                        {
                            if (depth > 1)
                            {
                                cout << "info mate in " << depth / 2 << endl;
                            }
                            else
                            {
                                cout << "info mate" << endl;
                            }
                        }
                        break;
                    }
                }
            }
            delete [] scores;
        }
        else
        {
            auto start = steady_clock::now();
            move = AlphaBeta::ponderAlphaBeta(recursionDepth, random, cells, currentPlayer, principalVariation, finishTime);
            auto end = steady_clock::now();
            string moveString = Logic::moveToString(move, cells);
            float duration = (float)duration_cast<microseconds>(end - start).count()/1000;
            if (move != NULL_MOVE)
            {
                if (Options::verbose)
                {
                    printInfo(recursionDepth, duration, AlphaBeta::predictedScore, moveString);
                }
            }
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

        if (Options::openingBook)
        {
            uint32_t bookMove = searchBook();
            if (bookMove != NULL_MOVE)
            {
                return bookMove;
            }
        }
        // TODO: still search until time is up, but using the saved move as basis

        // Calculate finish time point
        time_point<steady_clock> finishTime;
        finishTime = steady_clock::now() + std::chrono::milliseconds(searchTimeMilliseconds);

        uint32_t move = NULL_MOVE;
        size_t nMoves = Logic::availablePlayerMoves(currentPlayer, cells)[MAX_PLAYER_MOVES - 1];
        int64_t *scores = new int64_t[nMoves];

        while (steady_clock::now() < finishTime && recursionDepth < MAX_DEPTH)
        {
            auto start = steady_clock::now();
            uint32_t proposedMove = AlphaBeta::ponderAlphaBeta(recursionDepth, random, cells, currentPlayer, move, finishTime, scores);
            auto end = steady_clock::now();
            string moveString = Logic::moveToString(proposedMove, cells);
            float duration = (float)duration_cast<microseconds>(end - start).count()/1000;
            if (proposedMove != NULL_MOVE)
            {
                if (Options::verbose)
                {
                    printInfo(recursionDepth, duration, AlphaBeta::predictedScore, moveString);
                    if (AlphaBeta::predictedScore < -BASE_BETA)
                    {
                        cout << "info loss in " << recursionDepth / 2 << endl;
                        break;
                    }
                }
                move = proposedMove;
                if (AlphaBeta::predictedScore > BASE_BETA)
                {
                    if (Options::verbose)
                    {
                        if (recursionDepth > 1)
                        {
                            cout << "info mate in " << recursionDepth / 2 << endl;
                        }
                        else
                        {
                            cout << "info mate" << endl;
                        }
                    }
                    break;
                }
            }
            recursionDepth += 1;
        }

        delete [] scores;

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
        uint32_t move = Logic::playRandom(cells, currentPlayer);
        endTurn();
        return move;
    }

    bool Board::isMoveLegal(uint32_t move)
    {
        array<uint32_t, MAX_PLAYER_MOVES> moves = Logic::availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = moves[MAX_PLAYER_MOVES - 1];
        for (size_t k = 0; k < nMoves; k++)
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

    int64_t Board::evaluate()
    {
        return AlphaBeta::evaluatePosition(cells);
    }

    // Adds a piece to the designated coordinates
    void Board::addPiece(uint8_t piece, int i, int j)
    {
        cells[Logic::coordsToIndex(i, j)] = piece;
    }

    void Board::setStringState(string stateString)
    {
        vector<string> stateWords = Utils::split(stateString, " ");
        Logic::stringToCells(stateWords[0], cells);
        currentPlayer = (stateWords[1] == "w") ? 0U : 1U;
        lastPieceCount = countPieces();
        halfMoveCounter = std::stoi(stateWords[2]);
        moveCounter = std::stoi(stateWords[3]);
    }

    string Board::getStringState()
    {
        string cellsString = Logic::cellsToString(cells);
        string playerString = (currentPlayer == 0U) ? "w" : "b";

        return cellsString + " " + playerString + " " + std::to_string(halfMoveCounter) + " " + std::to_string(moveCounter);
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

        halfMoveCounter = 0;
        moveCounter = 1;

        lastPieceCount = countPieces();

        // Init eval NN
        // AlphaBeta::network.load();
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
        return Logic::isPositionWin(cells);
    }

    bool Board::checkDraw()
    {
        return halfMoveCounter >= 20;
    }

    // TODO
    bool Board::checkStalemate()
    {
        array<uint32_t, 512UL> availableMoves = Logic::availablePlayerMoves(currentPlayer, cells);
        size_t nMoves = availableMoves[MAX_PLAYER_MOVES - 1];
        return (nMoves == 0);
    }

    uint8_t Board::getWinner()
    {
        // TODO refactor function name below
        return Logic::getWinningPlayer(cells);
    }

    int64_t Board::getPredictedScore()
    {
        return AlphaBeta::predictedScore;
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
        if (currentPlayer == 1U)
        {
            moveCounter += 1;
        }
        currentPlayer = 1U - currentPlayer;
        uint32_t pieceCount = countPieces();
        if (lastPieceCount != pieceCount)
        {
            lastPieceCount = pieceCount;
            halfMoveCounter = 0;
        }
        else
        {
            halfMoveCounter += 1;
        }
    }

}
