#include <random>

using namespace std;

namespace PijersiEngine
{
    extern random_device rd; // Obtaining random number from hardware
    extern mt19937 gen; // Seeding generator
    extern uniform_real_distribution<float> distribution; // Defining uniform distribution
}