#include <random>

#include <hash.hpp>
#include <lookup.hpp>
#include <rng.hpp>

namespace PijersiEngine::Hash
{
    std::uniform_int_distribution<uint64_t> uint64rng(0ULL, UINT64_MAX);
    std::uniform_int_distribution<uint32_t> uint32rng(0U , UINT32_MAX);

    uint32_t pieceHashKeys[1575];
    uint32_t depthHashKeys[20];
    // uint64_t pieceHashKeys[1575];
    // uint64_t depthHashKeys[20];
    // LRUCache<uint64_t, uint64_t> hashTable(1024*1024*1024);
    uint64_t hashTable[1 << 24];

    void hashInit()
    {
        for (int index = 0; index < 1530; index++)
        {
            pieceHashKeys[index] = uint32rng(RNG::gen);
        }
        for (int index = 1530; index < 1545; index++)
        {
            pieceHashKeys[index] = 0;
        }
        for (int index = 0; index < 20; index++)
        {
            depthHashKeys[index] = uint32rng(RNG::gen);
        }
    }

    uint64_t hashPiece(uint8_t piece, int index)
    {
        return pieceHashKeys[Lookup::pieceToIndex[piece] * 45 + index];
    }

    // Implement incremental hashing
    uint32_t hash(uint8_t cells[45], int recursionDepth)
    {
        uint32_t result = 0;
        for (int index = 0; index < 45; index++)
        {
            result ^= hashPiece(cells[index], index);
        }
        // result ^= depthHashKeys[recursionDepth];
        return result;
    }
}