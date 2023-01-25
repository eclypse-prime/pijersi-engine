#ifndef RNG_HPP
#define RNG_HPP

#include <random>

namespace PijersiEngine::RNG
{
    extern std::random_device rd; // Obtaining random number from hardware
    extern std::mt19937 gen; // Seeding generator
    extern std::uniform_real_distribution<float> distribution; // Defining uniform distribution
}

#endif