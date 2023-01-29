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
    Board board;
    board.init();

    bool quit = false;

    while (!quit)
    {
        string line;
        std::getline(cin, line);
        
        vector<string> words = Utils::split(line);

        if (words.size() >= 1)
        {
            string command = words[0];
            if (command == "quit")
            {
                quit = true;
            }
            else if (command == "ugi")
            {
                cout << "id name Natural-Selection" << endl;
                cout << "id author Eclypse-Prime" << endl;
                cout << "ugiok" << endl;
            }
            else if (command == "isready")
            {
                cout << "readyok" << endl;
            }
            else if (command == "go")
            {
                if (words.size() >= 3)
                {
                    string mode = words[1];
                    string parameter = words[2];
                    if (mode == "depth")
                    {
                        int depth = stoi(parameter);
                        if (depth >= 1)
                        {
                            uint32_t move;
                            for (int k = 1; k <= depth; k++)
                            {
                                auto start = steady_clock::now();
                                move = board.searchDepth(k, true);
                                string moveString = Logic::moveToString(move, board.getState());
                                auto end = steady_clock::now();
                                int duration = duration_cast<milliseconds>(end - start).count();
                                cout << "info depth " << k << " seldepth " << k << " pv " << moveString << endl;
                            }
                            cout << "bestmove " << Logic::moveToString(move, board.getState()) << endl;
                            board.playManual(move);
                        }
                    }
                    else if (mode == "movetime")
                    {
                        int durationMilliseconds = stoi(parameter);
                        time_point<steady_clock> finishTime = steady_clock::now() + milliseconds(durationMilliseconds);
                        if (durationMilliseconds >= 0)
                        {
                            uint32_t move = NULL_MOVE;
                            int depth = 1;
                            while (steady_clock::now() < finishTime)
                            {
                                uint64_t remainingTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - std::chrono::steady_clock::now()).count();
                                auto start = steady_clock::now();
                                uint32_t proposedMove = board.searchDepth(depth, true, move, remainingTimeMilliseconds, false);
                                auto end = steady_clock::now();
                                if (proposedMove != NULL_MOVE)
                                {
                                    move = proposedMove;
                                    string moveString = Logic::moveToString(move, board.getState());
                                    int duration = duration_cast<milliseconds>(end - start).count();
                                    cout << "info depth " << depth << " seldepth " << depth << " pv " << moveString << endl;
                                    depth += 1;
                                }
                            }
                            if (move != NULL_MOVE)
                            {
                                string moveString = Logic::moveToString(move, board.getState());
                                cout << "bestmove " << moveString << endl;
                                board.playManual(move);
                            }
                        }
                    }
                }
            }
            else if (command == "position")
            {
                if (words.size() >= 2)
                {
                    string mode = words[1];
                    if (words.size() >= 3)
                    {
                        string parameter = words[2];
                        
                        if (mode == "fen")
                        {
                            // TODO: need to update stringstate for ugi
                            board.setStringState(parameter);
                        }
                    }
                }
            }
        }
    }
    return 0;
}