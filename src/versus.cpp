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
    // versus [engine 1 path] [engine 2 path] [rounds]
    if (argc != 4)
    {
        std::cout << "Error" << std::endl;
        std::cout << argc << std::endl;
        return EXIT_FAILURE;
    }

    string enginePath1 = argv[1];
    string enginePath2 = argv[2];
    int nRounds = std::stoi(argv[3]);

    Board board;

    int wins = 0;

    for (int iter = 0; iter < nRounds; iter++)
    {
        std::cout << "Starting game " << iter << std::endl;
        board.init();
        while (!board.checkWin())
        {
            if ((board.currentPlayer == 0U && iter % 2 == 0) || (board.currentPlayer == 1U && iter % 2 == 1))
            {
                string command = enginePath1 + " " + board.getStringState() + " d 5";
                string output = exec(command.c_str());
                board.playManual(output);
            }
            else
            {
                string command = enginePath2 + " " + board.getStringState() + " d 4";
                string output = exec(command.c_str());
                board.playManual(output);
            }
            // if (board.currentPlayer == 0U)
            // {
            //     string command = enginePath1 + " " + board.getStringState() + " d 5";
            //     string output = exec(command.c_str());
            //     board.playManual(output);
            // }
            // else
            // {
            //     string command = enginePath1 + " " + board.getStringState() + " d 5";
            //     string output = exec(command.c_str());
            //     board.playManual(output);
            // }
        }
        // if (board.evaluate() > 0)
        if (board.evaluate() > 0 && iter % 2 == 0)
        {
            wins ++;
            std::cout << "Engine 1 wins" << std::endl;
        }
        else
        {
            std::cout << "Engine 2 wins" << std::endl;
        }
    }

    std::cout << "Player 1 won " << wins << " games out of " << nRounds << std::endl;

    return EXIT_SUCCESS;
}