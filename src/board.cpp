#include <board.h>
#include <iostream>
#include <string>
#include <cstdint>
#include <omp.h>
#include <algorithm>
#include <random>
#include <cfloat>
#include <chrono>

using namespace std;


namespace PijersiEngine
{
    random_device rd; // Obtaining random number from hardware
    mt19937 gen(rd()); // Seeding generator
    uniform_real_distribution<float> distribution(-0.25f, 0.25f); // Defining uniform distribution

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

    // Calculates a move using alphabeta minimax algorithm of chosen depth.
    vector<int> Board::ponderAlphaBeta(int recursionDepth, bool random)
    {

        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(currentPlayer, cells);

        if (moves.size() > 0)
        {
            int index = 0;
            int16_t *extremums = new int16_t[moves.size() / 6];

            int16_t alpha = INT16_MIN;
            int16_t beta = INT16_MAX;
            
            // Evaluate possible moves
            #pragma omp parallel for schedule(dynamic)
            for (int k = 0; k < moves.size() / 6; k++)
            {
                extremums[k] = _evaluateMove(moves.data() + 6 * k, recursionDepth, alpha, beta, cells, currentPlayer);
            }

            // Find best move
            if (currentPlayer == 0)
            {
                float maximum = -FLT_MAX;
                for (int k = 0; k < moves.size() / 6; k++)
                {
                    // Add randomness to separate equal moves if parameter active
                    float salt = random ? distribution(gen) : 0.f;
                    float extremum = salt + (float)extremums[k];
                    if (extremum > maximum)
                    {
                        maximum = extremum;
                        index = k;
                    }
                }
            }
            else
            {
                float minimum = FLT_MAX;
                for (int k = 0; k < moves.size() / 6; k++)
                {
                    // Add randomness to separate equal moves if parameter active
                    float salt = random ? distribution(gen) : 0.f;
                    float extremum = salt + (float)extremums[k];
                    if (extremum < minimum)
                    {
                        minimum = extremum;
                        index = k;
                    }
                }
            }

            forecast = extremums[index];

            delete extremums;

            vector<int>::const_iterator first = moves.begin() + 6 * index;
            vector<int>::const_iterator last = moves.begin() + 6 * (index + 1);
            vector<int> move(first, last);
            return move;
        }
        return vector<int>({-1, -1, -1, -1, -1, -1});
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

    // Chooses a random move
    vector<int> Board::ponderRandom()
    {
        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(currentPlayer, cells);

        if (moves.size()>0)
        {
            uniform_int_distribution<int> intDistribution(0, moves.size()/6 - 1);

            int index = intDistribution(gen);

            vector<int>::const_iterator first = moves.begin() + 6 * index;
            vector<int>::const_iterator last = moves.begin() + 6 * (index + 1);
            vector<int> move(first, last);
            return move;
        }
        return vector<int>({0, 0, 0, 0, 0, 0});
    }

    // Plays a random move and returns it
    vector<int> Board::playRandom()
    {
        // Calculate move
        vector<int> move = ponderRandom();
        // Apply move
        playManual(move);
        return move;
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
        return _evaluate(cells);
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
    char pieceToChar(uint8_t piece)
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
                        char1 = pieceToChar(piece);
                        // If the piece is a stack
                        if (piece >= 16)
                        {
                            char2 = pieceToChar(piece >> 4);
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
                        char1 = pieceToChar(piece);
                        // If the piece is a stack
                        if (piece >= 16)
                        {
                            char2 = pieceToChar(piece >> 4);
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

    float Board::getForecast()
    {
        return forecast;
    }








    float _UCT(float nodeWins, float nodeSimulations, float totalSimulations)
    {
        return nodeWins/nodeSimulations + 1.414f * sqrtf(logf(totalSimulations) / nodeSimulations);
    }

    Node::Node(Node *newParent, const vector<int> &newMove, uint8_t rootPlayer) : move(newMove)
    {
        parent = newParent;
        player = rootPlayer;
        if (parent != nullptr)
        {
            board = new Board(*parent->board);
            board->playManual(move);
        }
        else
        {
            board = new Board();
            board->init();
        }

        children.reserve(256);
    }

    Node::~Node()
    {
        if (board != nullptr)
        {
            delete board;
        }
        for (int k = 0; k < children.size(); k++)
        {
            if (children[k] != nullptr)
            {
                delete children[k];
            }
        }
    }

    bool Node::isLeaf()
    {
        return (children.size() == 0);
    }

    bool Node::isWin()
    {
        return board->checkWin();
    }

    void Node::update(int winCount, int visitCount)
    {
        visits += visitCount;
        score += winCount;
        if (parent != nullptr)
        {
            parent->update(visitCount - winCount, visitCount);
        }
    }

    void Node::rollout(int nSimulations)
    {
        Board newBoard(*board);
        int nWins = 0;
        for (int k = 0; k < nSimulations; k++)
        {
            newBoard.setState(board->getState());
            newBoard.currentPlayer = board->currentPlayer;
            while (!newBoard.checkWin())
            {
                newBoard.playRandom();
            }
            // if ((newBoard.evaluate() > 0 && player == 0) || (newBoard.evaluate() <= 0 && player == 1))
            // Invert currentPlayer to count wins ???
            if ((newBoard.evaluate() > 0 && board->currentPlayer == 1) || (newBoard.evaluate() <= 0 && board->currentPlayer == 0))
            {
                nWins++;
            }
        }
        update(nWins, nSimulations);
    }

    void Node::expand()
    {
        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(board->currentPlayer, board->getState());
        if (moves.size() > 0)
        {
            for (int k = 0; k < moves.size() / 6; k++)
            {
                vector<int>::const_iterator first = moves.begin() + 6 * k;
                vector<int>::const_iterator last = moves.begin() + 6 * (k + 1);
                vector<int> chosenMove(first, last);
                children.push_back(new Node(this, chosenMove, player));
            }
        }
    }

    vector<int> Board::ponderMCTS(int seconds, int simulationsPerRollout)
    {
        int nThreads = omp_get_max_threads();
        vector<int> moves = _availablePlayerMoves(currentPlayer, cells);
        int nMoves = moves.size()/6;

        vector<int> visitsPerThreads(nMoves*nThreads);


        if (nMoves > 0)
        {
            #pragma omp parallel for num_threads(nThreads)
            for (int k = 0; k < nThreads; k++)
            {
                Node root(nullptr, vector<int>(), currentPlayer);
                root.board->setState(getState());
                root.board->currentPlayer = currentPlayer;
                root.expand();

                auto finish = chrono::system_clock::now() + chrono::seconds(seconds);

                Node *current = &root;
                do
                {
                    if (current->isLeaf())
                    {
                        if (current->visits == 0)
                        {
                            current->rollout(simulationsPerRollout);
                            current = &root;
                        }
                        else
                        {
                            if (!current->isWin())
                            {
                                current->expand();
                                if (current->children.size() > 0)
                                {
                                    current = current->children[0];
                                }
                            }
                            else
                            {
                                current->rollout(simulationsPerRollout);
                                current = &root;
                            }
                        }
                    }
                    else
                    {
                        float uctScore = -FLT_MAX;
                        int index = 0;
                        for (int i = 0; i < current->children.size(); i++)
                        {
                            if (current->children[k]->visits == 0)
                            {
                                index = i;
                                break;
                            }
                            else
                            {
                                float childScore = _UCT(current->children[k]->score, current->children[k]->visits, root.visits);
                                if (childScore > uctScore)
                                {
                                    index = i;
                                    uctScore = childScore;
                                }
                            }
                        }
                        current = current->children[index];
                    }
                } while (chrono::system_clock::now() < finish);
                for (int n = 0; n < nMoves; n++)
                {
                    visitsPerThreads[k*nMoves+n] = root.children[n]->visits;
                }
            }


            vector<int> visitsPerNode(nMoves);
            #pragma omp parallel for
            for (int k = 0; k < nThreads; k++)
            {
                for (int n = 0; n < nMoves; n++)
                {
                    visitsPerNode[n] += visitsPerThreads[k*nMoves+n];
                }
            }

            for (int k = 0; k < nMoves; k++)
            {
                cout << visitsPerNode[k] << ", ";
            }
            cout << endl;

            // Get child with max visits from root
            int maxVisits = 0;
            int index = 0;
            for (int k = 0; k < nMoves; k++)
            {
                if (visitsPerNode[k] > maxVisits)
                {
                    index = k;
                    maxVisits = visitsPerNode[k];
                }
            }

            // Select the corresponding move
            vector<int>::const_iterator first = moves.begin() + 6 * index;
            vector<int>::const_iterator last = moves.begin() + 6 * (index + 1);
            vector<int> move(first, last);

            return move;
        }
        return vector<int>({-1, -1, -1, -1, -1, -1});
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
