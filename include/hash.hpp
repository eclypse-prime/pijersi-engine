#ifndef HASH_HPP
#define HASH_HPP

namespace PijersiEngine::Hash
{
    extern uint64_t hashKeys[1530];
    void hashInit();
    uint64_t hash(uint8_t cells[45]);
}

#endif