#include <random>

using namespace std;

namespace PijersiEngine
{
    random_device rd; // Obtaining random number from hardware
    mt19937 gen(rd()); // Seeding generator
    uniform_real_distribution<float> distribution(-0.25f, 0.25f); // Defining uniform distribution
}