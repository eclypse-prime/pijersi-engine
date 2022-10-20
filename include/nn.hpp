#ifndef NN_HPP
#define NN_HPP
#include <cstdint>
#include <vector>
#include <array>

#define INPUT_TENSOR_SIZE 45*16
#define BATCH_SIZE 16

using namespace std;

namespace PijersiEngine
{
    void cellsToTensor(uint8_t cells[45], uint8_t currentPlayer, uint8_t input[INPUT_TENSOR_SIZE]);
    float _leakyRelu(float input);

    struct Dense720
    {
        float weights[INPUT_TENSOR_SIZE];
        float bias;

        void init();
        float forward(array<uint8_t, INPUT_TENSOR_SIZE> input);
        void back();
        void update(float learning_rate, float biasError, float batchError[INPUT_TENSOR_SIZE]);
    };

    struct Dense32
    {
        float weights[32];
        float bias;

        void init();
        float forward(array<float, 32> input);
        void update(float learning_rate, float biasError, float batchError[32]);
    };

    struct Dense32L
    {
        float weights[32];
        float bias;

        void init();
        float forward(array<float, 32> input);
        void update(float learning_rate, float biasError, float batchError[32]);
    };

    struct Network
    {
        Dense720 layer1[32];
        Dense32 layer2[32];
        Dense32 layer3[32];
        Dense32L final;

        void init();
        void load();
        float forward(array<uint8_t, INPUT_TENSOR_SIZE> input);
    };

    struct Trainer
    {
        int batchSize = 1;

        Dense720 layer1[32];
        Dense32 layer2[32];
        Dense32 layer3[32];
        Dense32L final;

        vector<array<uint8_t, INPUT_TENSOR_SIZE>> input;
        vector<array<float, 32>> output1;
        vector<array<float, 32>> output2;
        vector<array<float, 32>> output3;
        vector<float> outputFinal;

        vector<float> expected;

        vector<array<float, 32>> error1;
        vector<array<float, 32>> error2;
        vector<array<float, 32>> error3;
        vector<float> errorFinal;

        Trainer(int newBatchSize);
        void init(int newBatchSize);
        void forward(uint8_t cells[45], uint8_t currentPlayer, float expectedOutput, int i);
        float loss();
        void back(float learning_rate);
    };
}

#endif