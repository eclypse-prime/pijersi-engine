#include <random>

#include <hash.hpp>
#include <rng.hpp>

namespace PijersiEngine::Hash
{
    uniform_int_distribution<uint64_t> uint64rng(0ULL, UINT64_MAX);

    uint64_t hashKeys[1530];

    void hashInit()
    {
        for (int index = 0; index < 1530; index++)
        {
            hashKeys[index] = uint64rng(gen);
        }
    }

    uint64_t hashPiece(uint8_t piece, int index)
    {
        switch (piece)
        {
        case 17:
            return hashKeys[0 + index];
        case 21:
            return hashKeys[45 + index];
        case 25:
            return hashKeys[90 + index];
        case 81:
            return hashKeys[135 + index];
        case 85:
            return hashKeys[180 + index];
        case 89:
            return hashKeys[225 + index];
        case 145:
            return hashKeys[270 + index];
        case 149:
            return hashKeys[315 + index];
        case 153:
            return hashKeys[360 + index];
        case 209:
            return hashKeys[405 + index];
        case 213:
            return hashKeys[450 + index];
        case 217:
            return hashKeys[495 + index];
        case 221:
            return hashKeys[540 + index];
        case 1:
            return hashKeys[585 + index];
        case 5:
            return hashKeys[630 + index];
        case 9:
            return hashKeys[675 + index];
        case 13:
            return hashKeys[720 + index];
        case 51:
            return hashKeys[765 + index];
        case 55:
            return hashKeys[810 + index];
        case 59:
            return hashKeys[855 + index];
        case 115:
            return hashKeys[900 + index];
        case 119:
            return hashKeys[945 + index];
        case 123:
            return hashKeys[990 + index];
        case 179:
            return hashKeys[1035 + index];
        case 183:
            return hashKeys[1080 + index];
        case 187:
            return hashKeys[1125 + index];
        case 243:
            return hashKeys[1170 + index];
        case 247:
            return hashKeys[1215 + index];
        case 251:
            return hashKeys[1260 + index];
        case 255:
            return hashKeys[1305 + index];
        case 3:
            return hashKeys[1350 + index];
        case 7:
            return hashKeys[1395 + index];
        case 11:
            return hashKeys[1440 + index];
        case 15:
            return hashKeys[1485 + index];
        default:
            return 0;
        }
    }

    // Implement incremental hashing
    uint64_t hash(uint8_t cells[45])
    {
        uint64_t result = 0;
        for (int index = 0; index < 45; index++)
        {
            if (cells[index] != 0)
            {
                result ^= hashPiece(cells[index], index);
            }
        }
        return result;
    }
}