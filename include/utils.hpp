#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <unordered_map>
#include <vector>

namespace PijersiEngine::Utils
{
    std::vector<std::string> split(std::string str, std::string token = " ");
    std::string strip(std::string str);
    void sortPrincipalVariation(std::vector<uint32_t>& moves, uint32_t principalVariation);
}

#endif