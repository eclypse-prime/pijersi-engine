#ifndef MCTS_HPP
#define MCTS_HPP
#include <vector>

using namespace std;

namespace PijersiEngine
{
    float _UCT(float nodeWins, float nodeSimulations, float totalSimulations);

    vector<int> _ponderMCTS(int seconds, int simulationsPerRollout, uint8_t cells[45], uint8_t currentPlayer);

    struct Node
    {
        uint8_t cells[45];
        uint8_t currentPlayer;
        Node *parent;
        vector<Node*> children;
        vector<int> move;
        uint8_t rootPlayer;

        int visits = 0;
        int score = 0;

        Node(Node *newParent, const vector<int> &newMove, uint8_t newCurrentPlayer);
        ~Node();

        void expand();
        bool isLeaf();
        bool isWin();
        void rollout(int nSimulations);

        void update(int winCount, int visitCount);
    };
}

#endif