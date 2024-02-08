#ifndef MCTS_HPP
#define MCTS_HPP
#include <vector>

namespace PijersiEngine::MCTS
{
    uint64_t ponderMCTS(int milliseconds, int simulationsPerRollout, uint8_t cells[45], uint8_t currentPlayer);

    struct Node
    {
        uint8_t cells[45];
        uint8_t player;
        Node *parent;
        std::vector<Node*> children;
        uint64_t move;

        int visits = 0;
        int score = 0;

        Node(Node *newParent, const uint64_t &newMove, uint8_t newPlayer);
        ~Node();

        void expand();
        bool isLeaf();
        bool isWin();
        void rollout(int nSimulations);

        void update(int winCount, int visitCount);
    };
}

#endif