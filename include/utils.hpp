#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace PijersiEngine::Utils
{
    std::vector<std::string> split(std::string str, std::string token = " ");
    std::string strip(std::string str);

    void doubleSort(int64_t* scores, size_t* indices, size_t nMoves);
    void sortPrincipalVariation(std::vector<uint64_t>& moves, uint64_t principalVariation);
}

#endif