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
float scoresCurrent[6 + 1 + 1] = {63.6369, 98.5258, 111.57, 112.084, 133.013, 132.617, -10.9016, 68.3554};
float scoresEval[2][6 + 1 + 1] = {{90, 100, 110, 120, 130, 140, 30, 80}, {63.6369, 98.5258, 111.57, 112.084, 133.013, 132.617, -10.9016, 68.3554}};
// {71.6253, 92.3781, 99.3675, 117.182, 137.446, 168.55, 150, -14.4487, 80}
// {94.1396 95.0996 110 124.838 130 140 150 10 75.6163}
// {63.6804 100 111.324 119.4 130 143.845 150 -16.2713 62.5864} -> 0.2076
// {63.6804, 100, 111.324, 119.4, 130, 143.845, 150, -16.2713, 57.569} -> 0.2356, 0.1864

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
    for (size_t side = 0; side < 2; side++)
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
                                score = scoresEval[side][6 - Logic::indexToLine[index]];
                            }
                            else
                            {
                                score = -scoresEval[side][Logic::indexToLine[index]];
                            }
                            if (piece > 16)
                            {
                                score *= 2;
                                if ((piece & 2) == 0)
                                {
                                    score += scoresEval[side][6];
                                }
                                else
                                {
                                    score -= scoresEval[side][6];
                                }
                            }
                        }
                    }
                    else
                    {
                        if ((piece & 2) == 0)
                        {
                            score = scoresEval[side][7];
                        }
                        else
                        {
                            score = -scoresEval[side][7];
                        }
                        if (piece > 16)
                        {
                            score *= 2;
                            if ((piece & 2) == 0)
                            {
                                score += scoresEval[side][6];
                            }
                            else
                            {
                                score -= scoresEval[side][6];
                            }
                        }
                    }
                    Lookup::pieceScores[1575 * side + Lookup::pieceToIndex[piece] * 45 + index] = score;
                }
            }
        }
    }
}

void mutate(size_t selectedParameter, float delta)
{
    scoresEval[1][selectedParameter] += delta;

    cout << "Parameter: " << selectedParameter << "   Delta: " << delta << endl;
    cout << "Original: ";
    for (size_t k = 0; k < 8; k++)
    {
        cout << scoresEval[0][k] << " ";
    }
    cout << endl;
    cout << "Current:  ";
    for (size_t k = 0; k < 8; k++)
    {
        cout << scoresCurrent[k] << " ";
    }
    cout << endl;
    cout << "New:      ";
    for (size_t k = 0; k < 8; k++)
    {
        cout << scoresEval[1][k] << " ";
    }
    cout << endl;
}

void update(bool keep, size_t selectedParameter, float delta, float learningRate)
{
    if (keep)
    {
        scoresCurrent[selectedParameter] += delta * learningRate;
    }
    for (size_t k = 0; k < 8; k++)
    {
        scoresEval[1][k] = scoresCurrent[k];
    }
}

void setWeights(float weights[8])
{
    for (size_t k = 0; k < 8; k++)
    {
        scoresEval[1][k] = weights[k];
    }
    fillTable();
}

float playGames(Board board, int depth, size_t nRepeats, vector<string> openings)
{
    size_t side = 0;
    size_t starting_player = 0;
    uint64_t winCount[2] = {0, 0};

    size_t nGames = openings.size() * nRepeats * 2;

    for (size_t iter = 0; iter < nGames; iter++)
    {
        if (iter % (nGames / 20) == 0)
        {
            cout << "Starting game " << iter << "/" << nGames << '\r' << flush;
        }
        board.init();
        board.setStringState(openings[(iter/2)%openings.size()]);
        // board.playRandom();
        side = starting_player;
        while (!board.checkWin() && !board.checkDraw() && !board.checkStalemate())
        {
            board.playDepth(depth, true, side);
            if (board.checkWin() || board.checkStalemate())
            {
                winCount[side] += 1;
                if (board.checkStalemate())
                {
                    cout << "Stalemate" << endl;
                }
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

    cout << "Engine 1: " << winRate[0] * 100 << "% (" << winCount[0] << ") | Engine 2: " << winRate[1] * 100 << "% (" << winCount[1] << ")" << endl;
    cout << "Draws: " << drawRate * 100 << "% (" << nDraws << ")" << endl;

    return winRateDelta;
}

// int main(int argc, char **argv)
// {
//     Board board;
//     board.init();

//     vector<string> openings = readFile("ply1.txt");

//     size_t nIter = std::stoi(argv[1]);
//     size_t repeatsPerIter = std::stoi(argv[2]);
//     int depth = std::stoi(argv[3]);
//     float temperature = 0.1;
//     float alpha = 0.90;

//     bool keep = false;

//     fillTable();
//     cout << "Initial comparison" << endl;
//     cout << "Original: ";
//     for (size_t k = 0; k < 8; k++)
//     {
//         cout << scoresEval[0][k] << " ";
//     }
//     cout << endl;
//     cout << "Current:  ";
//     for (size_t k = 0; k < 8; k++)
//     {
//         cout << scoresCurrent[k] << " ";
//     }
//     cout << endl;
//     float bestWinRateDelta = playGames(board, depth, repeatsPerIter, openings);
//     cout << "Initial W/R delta: " << bestWinRateDelta << endl;


//     for (size_t iter = 0; iter < nIter; iter++)
//     {
//         cout << "Iter " << iter << endl;
//         size_t selectedParameter = intDistribution(gen);

//         float delta = deltaDistribution(gen);

//         mutate(selectedParameter, delta);
//         fillTable();

//         float winRateDelta = playGames(board, depth, repeatsPerIter, openings);
//         cout << "New W/R delta: " << winRateDelta << " | Current W/R Delta: " << bestWinRateDelta << " | Difference: " << winRateDelta - bestWinRateDelta << endl;

//         if (winRateDelta > bestWinRateDelta)
//         {
//             cout << "Win rate delta improved, keeping changes" << endl;
//             keep = true;
//             bestWinRateDelta = winRateDelta;
//         }
//         else
//         {
//             float deltaDiff = winRateDelta - bestWinRateDelta;
//             float threshold = exp(deltaDiff/temperature);
//             float roll = probabilityDistribution(gen);
//             cout << "Threshold: " << threshold << " | Roll: " << roll << endl;
//             if (roll < threshold) {
//                 cout << "Roll successful, keeping changes" << endl;
//                 bestWinRateDelta = winRateDelta;
//                 keep = true;
//             }
//             else
//             {
//                 cout << "Roll failed, cancelling changes" << endl;
//                 keep = false;
//             }
//         }
//         update(keep, selectedParameter, delta, 1);
//         temperature *= alpha;
//         cout << endl;
//     }
//     for (size_t k = 0; k < 8; k++)
//     {
//         cout << scoresCurrent[k] << ", ";
//     }
//     cout << endl;
// }