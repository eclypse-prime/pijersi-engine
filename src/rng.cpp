#include <random>

namespace PijersiEngine::RNG
{
    std::random_device rd; // Obtaining random number from hardware
    std::mt19937 gen(rd()); // Seeding generator
    std::uniform_real_distribution<float> distribution(0.0f, 0.01f); // Defining uniform distribution
}