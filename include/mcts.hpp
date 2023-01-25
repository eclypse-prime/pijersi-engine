#ifndef MCTS_HPP
#define MCTS_HPP
#include <vector>

using std::vector;

namespace PijersiEngine::MCTS
{
    uint32_t ponderMCTS(int seconds, int simulationsPerRollout, uint8_t cells[45], uint8_t currentPlayer);

    struct Node
    {
        uint8_t cells[45];
        uint8_t player;
        Node *parent;
        vector<Node*> children;
        uint32_t move;

        int visits = 0;
        int score = 0;

        Node(Node *newParent, const uint32_t &newMove, uint8_t newPlayer);
        ~Node();

        void expand();
        bool isLeaf();
        bool isWin();
        void rollout(int nSimulations);

        void update(int winCount, int visitCount);
    };
}

#endif