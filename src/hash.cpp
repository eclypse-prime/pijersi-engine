#include <random>

#include <hash.hpp>
#include <lookup.hpp>
#include <rng.hpp>

namespace PijersiEngine::Hash
{
    std::uniform_int_distribution<uint64_t> uint64rng(0ULL, UINT64_MAX);

    uint64_t pieceHashKeys[1575];
    uint64_t depthHashKeys[20];
    LRUCache<uint64_t, uint64_t> hashTable(1024*1024*1024);

    void hashInit()
    {
        for (int index = 0; index < 1530; index++)
        {
            pieceHashKeys[index] = uint64rng(RNG::gen);
        }
        for (int index = 1530; index < 1575; index++)
        {
            pieceHashKeys[index] = 0;
        }
        for (int index = 0; index < 20; index++)
        {
            depthHashKeys[index] = uint64rng(RNG::gen);
        }
    }

    uint64_t hashPiece(uint8_t piece, int index)
    {
        return pieceHashKeys[Lookup::pieceToIndex[piece] * 45 + index];
    }

    // TODO: Implement incremental hashing
    uint64_t hash(uint8_t cells[45], int recursionDepth)
    {
        uint64_t result = 0;
        for (int index = 0; index < 45; index++)
        {
            result ^= hashPiece(cells[index], index);
        }
        result ^= depthHashKeys[recursionDepth];
        return result;
    }
}