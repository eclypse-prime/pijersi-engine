#include <random>

#include <hash.hpp>
#include <rng.hpp>

namespace PijersiEngine::Hash
{
    uniform_int_distribution<uint64_t> int64rng(0ULL, UINT64_MAX);

    void hash(uint8_t cells[45])
    {
    }
}