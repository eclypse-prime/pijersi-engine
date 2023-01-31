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
using std::cin;
using std::cout;
using std::endl;
using std::string;
using std::vector;

using namespace PijersiEngine;

int main(int argc, char** argv)
{
    if (argc != 4 && argc != 7)
    {
        return 0;
    }

    Board board;

    string stringState;
    string mode;
    string parameter;

    if (string(argv[1]) == "startpos")
    {
        board.init();
        mode = argv[2];
        parameter = argv[3];
    }
    else
    {
        stringState = argv[1] + string(" ") + argv[2] + string(" ") + argv[3] + string(" ") + argv[4];
        board.setStringState(stringState);
        mode = argv[5];
        parameter = argv[6];
    }

    if (mode == "d")
    {
        int recursionDepth = stoi(parameter);
        if (recursionDepth >= 1)
        {
            uint32_t move;
            move = board.searchDepth(recursionDepth, true);
            if (move != NULL_MOVE)
            {
                string moveString = Logic::moveToString(move, board.getState());
                cout << moveString << endl;
            }
        }
    }
    else if (mode == "t")
    {
        uint64_t durationMilliseconds = stoi(parameter);
        if (durationMilliseconds >= 0)
        {
            uint32_t move = NULL_MOVE;
            move = board.searchTime(true, durationMilliseconds);
            if (move != NULL_MOVE)
            {
                string moveString = Logic::moveToString(move, board.getState());
                cout << moveString << endl;
            }
        }
    }
    return 0;
}