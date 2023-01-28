#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

#include <board.hpp>
#include <utils.hpp>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    result = PijersiEngine::Utils::strip(result);
    return result;
}

int main(int argc, char** argv)
{
    using namespace PijersiEngine;
    // versus [engine 1 path] [engine 1 mode] [engine 1 parameter] [engine 2 path] [engine 2 move] [engine 2 parameter] [rounds] [switch colours]
    if (argc != 9)
    {
        std::cout << "Error" << std::endl;
        return EXIT_FAILURE;
    }

    string enginePath1 = argv[1];
    string engineMode1 = argv[2];
    string engineParam1 = argv[3];
    string enginePath2 = argv[4];
    string engineMode2 = argv[5];
    string engineParam2 = argv[6];
    int nRounds = std::stoi(argv[7]);
    bool switchColours = string(argv[8]) == "y";

    std::cout << "Engine 1 path: " << enginePath1 << "\n";
    std::cout << "Engine 1 mode: " << engineMode1 << "\n";
    std::cout << "Engine 1 param: " << engineParam1 << "\n";
    std::cout << "Engine 2 path: " << enginePath2 << "\n";
    std::cout << "Engine 2 mode: " << engineMode2 << "\n";
    std::cout << "Engine 2 param: " << engineParam2 << "\n";
    std::cout << "Number of rounds: " << nRounds << "\n";
    std::cout << "Switch colours : " << switchColours << std::endl;


    Board board;

    int wins = 0;

    for (int iter = 0; iter < nRounds; iter++)
    {
        std::cout << "Starting game " << iter << std::endl;
        board.init();
        int moves = 0;
        while (!board.checkWin())
        {
            if (switchColours)
            {
                if ((board.currentPlayer == 0U && iter % 2 == 0) || (board.currentPlayer == 1U && iter % 2 == 1))
                {
                    if (moves == 0)
                    {
                        std::cout << "Engine 1 plays white" << std::endl;
                    }
                    string command = enginePath1 + " " + board.getStringState() + " " + engineMode1 + " " + engineParam1;
                    string output = exec(command.c_str());
                    board.playManual(output);
                }
                else
                {
                    if (moves == 0)
                    {
                        std::cout << "Engine 2 plays white" << std::endl;
                    }
                    string command = enginePath2 + " " + board.getStringState() + " " + engineMode2 + " " + engineParam2;
                    string output = exec(command.c_str());
                    board.playManual(output);
                }
            }
            else
            {
                if (board.currentPlayer == 0U)
                {
                    if (moves == 0)
                    {
                        std::cout << "Engine 1 plays white" << std::endl;
                    }
                    string command = enginePath1 + " " + board.getStringState() + " " + engineMode1 + " " + engineParam1;
                    string output = exec(command.c_str());
                    board.playManual(output);
                }
                else
                {
                    if (moves == 0)
                    {
                        std::cout << "Engine 2 plays white" << std::endl;
                    }
                    string command = enginePath2 + " " + board.getStringState() + " " + engineMode2 + " " + engineParam2;
                    string output = exec(command.c_str());
                    board.playManual(output);
                }
            }
            moves++;
        }
        if (switchColours)
        {
            if ((board.evaluate() > 0 && iter % 2 == 0) || (board.evaluate() < 0 && iter % 2 == 1))
            {
                wins ++;
                std::cout << "Engine 1 wins in " << moves << " moves" << std::endl;
            }
            else
            {
                std::cout << "Engine 2 wins in " << moves << " moves" << std::endl;
            }
        }
        else
        {
            if (board.evaluate() > 0)
            {
                wins ++;
                std::cout << "Engine 1 wins in " << moves << " moves" << std::endl;
            }
            else
            {
                std::cout << "Engine 2 wins in " << moves << " moves" << std::endl;
            }
        }
        std::cout << std::endl;
    }

    std::cout << "Player 1 won " << wins << " games out of " << nRounds << std::endl;
    std::cout << "Player 1 win rate: " << (float)wins/(float)nRounds*100.f << "%" << std::endl;

    return EXIT_SUCCESS;
}