#ifndef RNG_HPP
#define RNG_HPP

#include <random>

using namespace std;

namespace PijersiEngine::RNG
{
    extern random_device rd; // Obtaining random number from hardware
    extern mt19937 gen; // Seeding generator
    extern uniform_real_distribution<float> distribution; // Defining uniform distribution
}

#endif