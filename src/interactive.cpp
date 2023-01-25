#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <chrono>

#include <board.hpp>
#include <logic.hpp>

using namespace std::chrono;
using std::vector;
using std::cin;
using std::cout;
using std::endl;

using namespace PijersiEngine;

vector<string> split(string str, string token = " "){
    vector<string>result;
    while(str.size()){
        size_t index = str.find(token);
        if(index!=string::npos){
            result.push_back(str.substr(0,index));
            str = str.substr(index+token.size());
            if(str.size()==0)result.push_back(str);
        }else{
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

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
        
        vector<string> words = split(line);

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
            else if (command == "a")
            {
                if (words.size() >= 2)
                {
                    string parameter = words[1];
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
                            cout << "alphabeta depth " << k << " move: " << moveString << " duration: "<< duration << "ms" << endl;
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
                    int duration = stoi(parameter);
                    time_point<steady_clock> finishTime = steady_clock::now() + seconds(duration);
                    if (duration >= 0)
                    {
                        uint32_t move;
                        int depth = 1;
                        while (steady_clock::now() < finishTime)
                        {
                            uint64_t remainingTimeMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(finishTime - std::chrono::steady_clock::now()).count();
                            auto start = steady_clock::now();
                            uint32_t proposedMove = board.searchDepth(depth, true, remainingTimeMilliseconds);
                            if (proposedMove != 0x00FFFFFF)
                            {
                                move = proposedMove;
                                string moveString = Logic::moveToString(move, board.getState());
                                auto end = steady_clock::now();
                                int duration = duration_cast<milliseconds>(end - start).count();
                                cout << "alphabeta depth " << depth << " move: " << moveString << " duration: "<< duration << "ms" << endl;
                                depth += 1;
                            }
                        }
                        cout << "best move chosen at depth " << depth-1 << endl;
                        board.playManual(move);
                        board.print();
                        if (board.checkWin())
                        {
                            cout << "\n--- Game ended ---\n" << endl;
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
                        int duration = duration_cast<milliseconds>(end - start).count();
                        cout << "perft depth " << k << " result: " << result << " duration: "<< duration << "ms" << endl;
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
                cout << "help:\n"<< "p -> print board\n" << "a [depth] -> alpha beta\n" << "m [move] -> manual move\n" << "r -> reset board\n" << "perft [depth] -> node count" << "q -> exit\n" << endl;
            }
        }
    }
    // board.playManual(Logic::stringToMove("f5=d4-c4", board.getState()));
    // uint32_t move = board.playAlphaBeta(5);

    // cout << Logic::moveToString(move, board.getState()) << endl;
    return 0;
}