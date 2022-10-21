#ifndef NN_HPP
#define NN_HPP
#include <cstdint>
#include <vector>
#include <array>

using namespace std;

namespace PijersiEngine
{
    void cellsToInput(uint8_t cells[45], uint8_t currentPlayer, uint8_t input[720]);
    float _sigmoid(float input);
    float _dSigmoid(float input);
    float _leakyRelu(float input);
    float _dLeakyRelu(float input);

    struct Layer
    {
        float *weights = nullptr;
        float *biases = nullptr;
        int inputSize = 0;
        int outputSize = 0;
        bool activation = false;

        Layer(int newInputSize, int newOutputSize, bool newUseActivation);
        void load();
        template <typename T>
        void forward(T *input, float *output);
        void update(float learningRate, float* weightError, float* biasError);
        ~Layer();
    };

    struct Dense720x256 : Layer
    {
        Dense720x256();
    };

    struct Dense256x32 : Layer
    {
        Dense256x32();
    };

    struct Dense32x32 : Layer
    {
        Dense32x32();
    };

    struct Dense32x1 : Layer
    {
        Dense32x1();
    };

    struct Network
    {
        uint8_t input[720];
        float output1[256];
        float output2[32];
        float output3[32];
        float output4[1];

        Dense720x256 layer1;
        Dense256x32 layer2;
        Dense32x32 layer3;
        Dense32x1 layer4;

        // TODO: Implement incremental updates when possible
        void setInput(uint8_t cells[45], uint8_t currentPlayer);
        float forward();
    };

    struct Trainer
    {
        Network network;
        int batchSize = 1;

        uint8_t *inputs;
        float *outputs1;
        float *outputs2;
        float *outputs3;
        float *outputs4;

        float *targets;

        float *errors1;
        float *errors2;
        float *errors3;
        float *errors4;

        Trainer(int newBatchSize);
        void setInput(uint8_t cells[45], uint8_t currentPlayer, float target, int batchIndex);
        void forward();
        float loss();
        void back(float learningRate);
        ~Trainer();
    };
}

#endif