#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <chrono>

#include <alphabeta.hpp>
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
    Board board;
    board.init();

    bool quit = false;

    while (!quit)
    {
        cout << ">>> ";
        string line;
        std::getline(cin, line);
        
        vector<string> words = Utils::split(line);

        if (words.size() >= 1)
        {
            string command = words[0];
            if (command == "q")
            {
                cout << "Exiting engine..." << endl;
                quit = true;
            }
            else if (command == "p")
            {
                board.print();
            }
            else if (command == "d")
            {
                if (words.size() >= 2)
                {
                    string parameter = words[1];
                    int recursionDepth = stoi(parameter);
                    if (recursionDepth >= 1)
                    {
                        uint32_t move = NULL_MOVE;
                        size_t nMoves = Logic::availablePlayerMoves(board.currentPlayer, board.getState()).size();
                        int16_t *scores = new int16_t[nMoves];
                        for (int k = 1; k <= recursionDepth; k++)
                        {
                            auto start = steady_clock::now();
                            move = AlphaBeta::ponderAlphaBeta(k, false, board.getState(), board.currentPlayer, move, time_point<steady_clock>::max(), scores);
                            string moveString = Logic::moveToString(move, board.getState());
                            auto end = steady_clock::now();
                            int duration = duration_cast<microseconds>(end - start).count();
                            cout << "alphabeta depth " << k << " move: " << moveString << " duration: "<< (float)duration/1000 << "ms" << endl;
                        }
                        board.playManual(move);
                        board.print();
                        if (board.checkWin())
                        {
                            cout << "\n--- Game ended ---\n" << endl;
                        }
                    }
                }
            }
            else if (command == "t")
            {
                if (words.size() >= 2)
                {
                    string parameter = words[1];
                    int durationMilliseconds = stoi(parameter);
                    time_point<steady_clock> finishTime = steady_clock::now() + milliseconds(durationMilliseconds);
                    if (durationMilliseconds >= 0)
                    {
                        uint32_t move = NULL_MOVE;
                        int depth = 1;
                        while (steady_clock::now() < finishTime && depth <= MAX_DEPTH)
                        {
                            uint64_t remainingTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - std::chrono::steady_clock::now()).count();
                            auto start = steady_clock::now();
                            uint32_t proposedMove = board.searchDepth(depth, true, move, remainingTimeMilliseconds, false);
                            if (proposedMove != NULL_MOVE)
                            {
                                move = proposedMove;
                                string moveString = Logic::moveToString(move, board.getState());
                                auto end = steady_clock::now();
                                int duration = duration_cast<microseconds>(end - start).count();
                                cout << "alphabeta depth " << depth << " move: " << moveString << " duration: "<< (float)duration/1000 << "ms" << endl;
                                depth += 1;
                            }
                        }
                        if (move != NULL_MOVE)
                        {
                            cout << "best move chosen at depth " << depth-1 << endl;
                            board.playManual(move);
                            board.print();
                            if (board.checkWin())
                            {
                                cout << "\n--- Game ended ---\n" << endl;
                            }
                        }
                        else
                        {
                            cout << "no move found" << endl;
                        }
                    }
                }
            }
            else if (command == "perft")
            {
                if (words.size() >= 2)
                {
                    string parameter = words[1];
                    int depth = stoi(parameter);
                    for (int k = 0; k <= depth; k++)
                    {
                        auto start = steady_clock::now();
                        uint64_t result = Logic::perft(k, board.getState(), board.currentPlayer);
                        auto end = steady_clock::now();
                        int duration = duration_cast<microseconds>(end - start).count();
                        cout << "perft depth " << k << " result: " << result << " duration: "<< (float)duration/1000 << "ms" << endl;
                    }
                }
            }
            else if (command == "m")
            {
                if (words.size() >= 2)
                {
                    string parameter = words[1];
                    uint32_t move = Logic::stringToMove(parameter, board.getState());
                    board.playManual(move);
                }
            }
            else if (command == "r")
            {
                board.init();
            }
            else if (command == "h")
            {
                cout << "help:\n"<< "p -> print board\n" << "d [depth] -> alpha beta to depth\n" << "t [time] -> alpha beta to time (in seconds)\n" << "m [move] -> manual move\n" << "r -> reset board\n" << "perft [depth] -> node count" << "q -> exit\n" << endl;
            }
            else if (command == "f")
            {
                cout << board.getStringState() << endl;
            }
        }
    }
    // board.playManual(Logic::stringToMove("f5=d4-c4", board.getState()));
    // uint32_t move = board.playAlphaBeta(5);

    // cout << Logic::moveToString(move, board.getState()) << endl;
    return 0;
}