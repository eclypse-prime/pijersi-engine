#include <cstdlib>
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <chrono>

#include <board.hpp>
#include <alphabeta.hpp>
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
            else if (command == "uginewgame")
            {
                board.init();
            }
            else if (command == "ugi")
            {
                cout << "id name Natural-Selection" << endl;
                cout << "id author Eclypse-Prime" << endl;
                cout << "ugiok" << endl;
            }
            else if (command == "isready")
            {
                board.init();
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
                            uint32_t move = board.searchDepth(depth, true);
                            if (move != NULL_MOVE)
                            {
                                string moveString = Logic::moveToString(move, board.getState());
                                cout << "bestmove " << Logic::moveToString(move, board.getState()) << endl;
                                board.playManual(move);
                            }
                            else
                            {
                                cout << "wtf" << endl;
                            }
                        }
                    }
                    else if (mode == "movetime")
                    {
                        int durationMilliseconds = stoi(parameter);
                        if (durationMilliseconds >= 0)
                        {
                            uint32_t move = board.searchTime(true, true, durationMilliseconds);
                            if (move != NULL_MOVE)
                            {
                                string moveString = Logic::moveToString(move, board.getState());
                                cout << "bestmove " << moveString << endl;
                                board.playManual(move);
                            }
                            else
                            {
                                cout << "wtf" << endl;
                            }
                        }
                    }
                    else if (mode == "manual")
                    {
                        board.playManual(parameter);
                    }
                }
            }
            else if (command == "position")
            {
                if (words.size() >= 2)
                {
                    string mode = words[1];
                    if (mode == "fen")
                    {
                        if (words.size() >= 6)
                        {
                            string stringState = words[2] + " " + words[3] + " " + words[4] + " " + words[5];
                            board.setStringState(stringState);
                            if (words.size() >= 7 && words[6] == "moves")
                            {
                                for (size_t k = 7; k < words.size(); k++)
                                {
                                    board.playManual(words[k]);
                                }
                            }
                        }
                    }
                    else if (mode == "startpos")
                    {
                        board.init();
                        if (words.size() >= 3 && words[2] == "moves")
                        {
                            for (size_t k = 3; k < words.size(); k++)
                            {
                                board.playManual(words[k]);
                            }
                        }
                    }
                }
            }
            else if (command == "query")
            {
                if (words.size() >= 2)
                {
                    string mode = words[1];
                    if (mode == "gameover")
                    {
                        if (board.checkWin() || board.checkDraw() || board.checkStalemate())
                        {
                            cout << "response true" << endl;
                        }
                        else
                        {
                            cout << "response false" << endl;
                        }
                    }
                    else if (mode == "p1turn")
                    {
                        cout << "response " << ((board.currentPlayer == 0) ? "true" : "false") << endl;
                    }
                    else if (mode == "result")
                    {
                        if (board.checkWin())
                        {
                            if (board.getWinner() == 0)
                            {
                                cout << "response p1win" << endl;
                            }
                            else
                            {
                                cout << "response p2win" << endl;
                            }
                        }
                        else if (board.checkDraw())
                        {
                            cout << "response draw" << endl;
                        }
                        else
                        {
                            cout << "response none" << endl;
                        }
                    }
                    else if (mode == "islegal")
                    {
                        if (words.size() >= 3)
                        {
                            string moveString = words[2];
                            uint32_t move = Logic::stringToMove(moveString, board.getState());
                            if (board.isMoveLegal(move))
                            {
                                cout << "response true" << endl;
                            }
                            else
                            {
                                cout << "response false" << endl;
                            }
                        }
                    }
                    else if (mode == "fen")
                    {
                        cout << "response " << board.getStringState() << endl;
                    }
                }
            }
        }
    }
    return 0;
}