#include <algorithm>
#include <chrono>
#include <cfloat>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "Eigen/Dense"
#include "Eigen/Sparse"

#include "alphabeta.hpp"
#include "board.hpp"
#include "nn.hpp"
#include "lookup.hpp"

using namespace PijersiEngine;
using std::cout;
using std::flush;
using std::endl;
using std::string;
using std::vector;
using namespace std::chrono;

// Score line 1 to 7, stack bonus, wise fixed score
float scoresEval[80] = {90, 90, 90, 90, 90, 90, 100, 100, 100, 100, 100, 100, 100, 110, 110, 110, 110, 110, 110, 120, 120, 120, 120, 120, 120, 120, 130, 130, 130, 130, 130, 130, 140, 140, 140, 140, 140, 140, 140,
                        -90, -90, -90, -90, -90, -90, -100, -100, -100, -100, -100, -100, -100, -110, -110, -110, -110, -110, -110, -120, -120, -120, -120, -120, -120, -120, -130, -130, -130, -130, -130, -130, -140, -140, -140, -140, -140, -140, -140,
                        30,
                        80};

std::random_device rd;
std::mt19937 gen(rd());
std::normal_distribution<float> deltaDistribution(0, 10);
std::uniform_real_distribution<float> probabilityDistribution(0, 1);
std::uniform_int_distribution<size_t> intDistribution(0, 7);

vector<string> readFile(string fileName)
{
    std::ifstream fileStream(fileName);
    string line;
    vector<string> lines;
    while (std::getline(fileStream, line))
    {
        lines.push_back(line);
    }
    return lines;
}

void fillTable()
{
    for (size_t piece = 0; piece < 256; piece++)
    {
        for (size_t index = 0; index < 45; index++)
        {
            if (Lookup::pieceToIndex[piece] != 34)
            {
                float score = 0;
                if ((piece & 12) != 12)
                {
                    if ((piece & 2) == 0 && Logic::indexToLine[index] == 0)
                    {
                        score = 512*1024;
                    }
                    else if ((piece & 2) == 2 && Logic::indexToLine[index] == 6)
                    {
                        score = -512*1024;
                    }
                    else
                    {
                        if ((piece & 2) == 0)
                        {
                            score = scoresEval[44 - index];
                        }
                        else
                        {
                            score = scoresEval[39 + index];
                        }
                        if (piece > 16)
                        {
                            score *= 2;
                            if ((piece & 2) == 0)
                            {
                                score += scoresEval[78];
                            }
                            else
                            {
                                score -= scoresEval[78];
                            }
                        }
                    }
                }
                else
                {
                    if ((piece & 2) == 0)
                    {
                        score = scoresEval[79];
                    }
                    else
                    {
                        score = -scoresEval[79];
                    }
                    if (piece > 16)
                    {
                        score *= 2;
                        if ((piece & 2) == 0)
                        {
                            score += scoresEval[78];
                        }
                        else
                        {
                            score -= scoresEval[78];
                        }
                    }
                }
                Lookup::pieceScores[1575 + Lookup::pieceToIndex[piece] * 45 + index] = score;
            }
        }
    }
}

void setWeights(float weights[80])
{
    for (size_t k = 0; k < 80; k++)
    {
        scoresEval[k] = weights[k];
    }
    fillTable();
}

void setWeightsFull(float weights[1575])
{
    for (size_t k = 0; k < 1575; k++)
    {
        Lookup::pieceScores[1575 + k] = weights[k];
    }
}

void checkMismatch()
{
    for (size_t k = 0; k < 1575; k++)
    {
        float originalWeight = Lookup::pieceScores[k];
        float newWeight = Lookup::pieceScores[1575 + k];
        if (newWeight != originalWeight)
        {
            cout << "Mismatch: " << newWeight << " " << originalWeight << endl;
        }
    }
}

float playGames(Board &board, int depth, size_t nRepeats)
{
    size_t side = 0;
    size_t starting_player = 0;
    uint64_t winCount[2] = {0, 0};

    vector<string> openings = readFile("ply1.txt");
    size_t nGames = openings.size() * nRepeats * 2;

    for (size_t iter = 0; iter < nGames; iter++)
    {
        // if (iter % (nGames / 20) == 0)
        // {
        //     cout << "Starting game " << iter << "/" << nGames << '\r' << flush;
        // }
        board.init();
        board.setStringState(openings[(iter/2)%openings.size()]);
        side = starting_player;
        while (!board.checkWin() && !board.checkDraw() && !board.checkStalemate())
        { 
            board.playDepth(depth, false, side);
            if (board.checkWin() || board.checkStalemate())
            {
                winCount[side] += 1;
                // if (board.checkStalemate())
                // {
                //     cout << "Stalemate" << endl;
                // }
            }
            side = 1 - side;
        }
        starting_player = 1 - starting_player;
    }

    float winRate[2] = {0, 0};
    winRate[0] = (float)winCount[0] / nGames;
    winRate[1] = (float)winCount[1] / nGames;

    uint64_t nDraws = nGames - winCount[0] - winCount[1];
    float drawRate = (float)nDraws / nGames;

    float winRateDelta = winRate[1] - winRate[0];

    // cout << "Engine 1: " << winRate[0] * 100 << "% (" << winCount[0] << ") | Engine 2: " << winRate[1] * 100 << "% (" << winCount[1] << ")" << endl;
    // cout << "Draws: " << drawRate * 100 << "% (" << nDraws << ")" << endl;

    return winRateDelta;
}
