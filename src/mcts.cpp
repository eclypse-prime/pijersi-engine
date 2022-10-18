#include <cfloat>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <ctime>

#include <omp.h>

#include <alphabeta.hpp>
#include <logic.hpp>
#include <mcts.hpp>

using namespace std;

namespace PijersiEngine
{
    float _UCT(float nodeWins, float nodeSimulations, float totalSimulations)
    {
        return nodeWins/nodeSimulations + 1.414f * sqrtf(logf(totalSimulations) / nodeSimulations);
    }

    vector<int> _ponderMCTS(int seconds, int simulationsPerRollout, uint8_t cells[45], uint8_t currentPlayer)
    {
        int nThreads = omp_get_max_threads();
        vector<int> moves = _availablePlayerMoves(currentPlayer, cells);
        int nMoves = moves.size()/6;

        vector<int> visitsPerThreads(nMoves*nThreads);


        if (nMoves > 0)
        {
            // #pragma omp parallel for num_threads(nThreads)
            // for (int k = 0; k < nThreads; k++)
            for (int k = 0; k < 1; k++)
            {
                Node root(nullptr, vector<int>(), currentPlayer);
                _setState(root.cells, cells);
                root.expand();

                auto finish = chrono::steady_clock::now() + chrono::seconds(seconds);

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
                            if (current->children[i]->visits == 0)
                            {
                                index = i;
                                break;
                            }
                            else
                            {
                                float childScore = _UCT(current->children[i]->score, current->children[i]->visits, root.visits);
                                // cout << childScore << endl;
                                if (childScore > uctScore)
                                {
                                    index = i;
                                    uctScore = childScore;
                                }
                            }
                        }
                        current = current->children[index];
                    }
                } while (chrono::steady_clock::now() <= finish);
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

    Node::Node(Node *newParent, const vector<int> &newMove, uint8_t newPlayer) : move(newMove)
    {
        parent = newParent;
        player = newPlayer;
        if (parent != nullptr)
        {
            _setState(cells, parent->cells);
            _playManual(move.data(), cells);
        }

        children.reserve(256);
    }

    Node::~Node()
    {
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
        return _checkWin(cells);
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
        uint8_t newCells[45];
        int nWins = 0;
        for (int k = 0; k < nSimulations; k++)
        {
            _setState(newCells, cells);
            uint8_t currentPlayer = player;
            while (!_checkWin(newCells))
            {
                // _playRandom(newCells, currentPlayer);
                _playManual(_ponderAlphaBeta(0, true, newCells, currentPlayer).data(), newCells);
                currentPlayer = 1 - currentPlayer;
            }
            if (currentPlayer == player)
            {
                nWins++;
            }
        }
        update(nWins, nSimulations);
    }

    void Node::expand()
    {
        // Get a vector of all the available moves for the current player
        vector<int> moves = _availablePlayerMoves(player, cells);
        if (moves.size() > 0)
        {
            for (int k = 0; k < moves.size() / 6; k++)
            {
                vector<int>::const_iterator first = moves.begin() + 6 * k;
                vector<int>::const_iterator last = moves.begin() + 6 * (k + 1);
                vector<int> chosenMove(first, last);
                children.push_back(new Node(this, chosenMove, 1-player));
            }
        }
    }
}