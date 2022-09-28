#ifndef BOARD_H
#define BOARD_H
#include <piece.h>
#include <string>
#include <vector>
#include <cstdint>

using namespace std;

namespace PijersiEngine
{
    void _playManual(int move[6], uint8_t *cells);
    vector<int> _availablePieceMoves(int i, int j, uint8_t cells[45]);
    vector<int> _availablePlayerMoves(uint8_t player, uint8_t cells[45]);
    void _move(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45]);
    void _stack(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45]);
    void _unstack(int iStart, int jStart, int iEnd, int jEnd, uint8_t cells[45]);
    bool _isMoveValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    bool _isMove2Valid(uint8_t movingPiece, int indexStart, int indexEnd, uint8_t cells[45]);
    bool _isStackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    bool _isUnstackValid(uint8_t movingPiece, int indexEnd, uint8_t cells[45]);
    float _evaluate(uint8_t cells[45]);
    float _evaluateMove(int move[6], int recursionDepth, float alpha, float beta, uint8_t cells[45], int currentPlayer);
    float _evaluateMoveTerminal(int move[6], uint8_t cells[45], uint8_t newCells[45]);
    void _play(int iStart, int jStart, int iMid, int jMid, int iEnd, int jEnd, uint8_t cells[45]);
    vector<int> _neighbours(int index);
    vector<int> _neighbours2(int index);

    enum Algorithm
    {
        Minimax,
        MCTS,
        Random
    };

    class Board
    {
    public:
        Board();
        Board(Board &board);
        // ~Board();

        void playManual(vector<int> move);
        vector<int> ponder(int recursionDepth, bool random);
        vector<int> playAuto(int recursionDepth, bool random = true);
        vector<int> playMCTS(int seconds);
        vector<int> ponderRandom();
        vector<int> playRandom();
        vector<int> ponderMCTS(int seconds);
        bool isMoveLegal(vector<int> move);
        float evaluate();
        void setState(uint8_t newState[45]);
        uint8_t *getState();
        void init();

        uint8_t at(int i, int j);
        void print();
        string toString();

        bool checkWin();
        float getForecast();
        uint8_t currentPlayer = 0;

    private:
        uint8_t cells[45];
        float forecast;

        void addPiece(uint8_t piece, int i, int j);

    };

    struct Node
    {
        Board *board;
        Node *parent;
        vector<Node*> children;
        vector<int> move;
        uint8_t player;

        int visits = 0;
        int score = 0;

        Node(Node *newParent, vector<int> newMove, uint8_t rootPlayer);
        ~Node();

        void expand();
        bool isLeaf();
        bool isWin();
        void rollout();

        void update(bool win);
    };

}
#endif