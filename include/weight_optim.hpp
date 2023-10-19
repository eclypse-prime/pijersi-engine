#ifndef WEIGHT_OPTIM_HPP
#define WEIGHT_OPTIM_HPP

#include <cstdint>
#include <string>
#include <vector>

#include <board.hpp>


void setWeights(float weights[8]);
float playGames(PijersiEngine::Board &board, int depth, size_t nRepeats);

#endif