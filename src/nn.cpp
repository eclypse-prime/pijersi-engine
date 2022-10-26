#include <cstdint>

#include <nn.hpp>

namespace PijersiEngine::NN
{
    int8_t *cellsToTensor(uint8_t cells[45])
    {
        int8_t input[INPUT_TENSOR_SIZE];

        for (int k = 0; k < 45; k++)
        {
            if (cells[k] != 0)
            {
                switch (cells[k] & 14)
                {
                case 0:
                    input[k * 16] = 1;
                    break;
                case 4:
                    input[k * 16 + 1] = 1;
                    break;
                case 8:
                    input[k * 16 + 2] = 1;
                    break;
                case 12:
                    input[k * 16 + 3] = 1;
                    break;
                case 2:
                    input[k * 16 + 4] = 1;
                    break;
                case 6:
                    input[k * 16 + 5] = 1;
                    break;
                case 10:
                    input[k * 16 + 6] = 1;
                    break;
                case 14:
                    input[k * 16 + 7] = 1;
                    break;
                }
                if (cells[k] >= 16)
                {
                    switch (cells[k] & 224)
                    {
                    case 0:
                        input[k * 16 + 8] = 1;
                        break;
                    case 64:
                        input[k * 16 + 9] = 1;
                        break;
                    case 128:
                        input[k * 16 + 10] = 1;
                        break;
                    case 192:
                        input[k * 16 + 11] = 1;
                        break;
                    case 32:
                        input[k * 16 + 12] = 1;
                        break;
                    case 96:
                        input[k * 16 + 13] = 1;
                        break;
                    case 160:
                        input[k * 16 + 14] = 1;
                        break;
                    case 224:
                        input[k * 16 + 15] = 1;
                        break;
                    }
                }
            }
        }

        return input;
    }

    inline int8_t _relu(int8_t input)
    {
        if (input >= 0)
        {
            return input;
        }
        return 0;
    }

    

    int8_t Dense720::forward(int8_t input[INPUT_TENSOR_SIZE])
    {
        int8_t sum = 0;
        for (int k = 0; k < INPUT_TENSOR_SIZE; k++)
        {
            sum += weights[k] * input[k];
        }
        sum += bias;
        return _relu(sum);
    }

    int8_t Dense32::forward(int8_t input[32])
    {
        int8_t sum = 0;
        for (int k = 0; k < 32; k++)
        {
            sum += weights[k] * input[k];
        }
        sum += bias;
        return _relu(sum);
    }
}