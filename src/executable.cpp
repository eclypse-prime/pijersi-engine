#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <chrono>

#include <board.hpp>
#include <logic.hpp>
#include <utils.hpp>

using namespace std::chrono;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

using namespace PijersiEngine;

int main(int argc, char** argv)
{
    if (argc != 4)
    {
        return 0;
    }

    Board board;

    string stringState = argv[1];

    string mode = argv[2];

    string parameter = argv[3];

    if (stringState == "startpos")
    {
        board.init();
    }
    else
    {
        board.setStringState(stringState);
    }

    if (mode == "d")
    {
        int depth = stoi(parameter);
        if (depth >= 1)
        {
            uint32_t move;
            move = board.searchDepth(depth, true);
            if (move != NULL_MOVE)
            {
                string moveString = Logic::moveToString(move, board.getState());
                cout << moveString << endl;
            }
        }
    }
    else if (mode == "t")
    {
        int duration = stoi(parameter);
        time_point<steady_clock> finishTime = steady_clock::now() + seconds(duration);
        if (duration >= 0)
        {
            uint32_t move = NULL_MOVE;
            int depth = 1;
            while (steady_clock::now() < finishTime)
            {
                uint64_t remainingTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - std::chrono::steady_clock::now()).count();
                uint32_t proposedMove = board.searchDepth(depth, true, remainingTimeMilliseconds);
                if (proposedMove != NULL_MOVE)
                {
                    move = proposedMove;
                    string moveString = Logic::moveToString(move, board.getState());
                    depth += 1;
                }
                
            }
            if (move != NULL_MOVE)
            {
                string moveString = Logic::moveToString(move, board.getState());
                cout << moveString << endl;
            }
        }
    }
    return 0;
}