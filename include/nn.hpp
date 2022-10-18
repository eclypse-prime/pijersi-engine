#ifndef NN_HPP
#define NN_HPP
#include <cstdint>

#define INPUT_TENSOR_SIZE 45*16

namespace PijersiEngine
{
    int8_t *cellsToTensor(uint8_t cells[45]);
    int8_t _relu(int8_t input);

    struct Network
    {
        Dense720 layer1[32];
        Dense32 layer2[32];
        Dense32 layer3;

        int16_t forward(int8_t input[INPUT_TENSOR_SIZE]);
    };

    struct Dense720
    {
        int8_t weights[INPUT_TENSOR_SIZE];
        int8_t bias;

        int8_t forward(int8_t input[INPUT_TENSOR_SIZE]);
    };

    struct Dense32
    {
        int8_t weights[32];
        int8_t bias;

        int8_t forward(int8_t input[32]);
    };
}

#endif