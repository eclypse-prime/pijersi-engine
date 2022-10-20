#include <algorithm>
#include <cstdint>
#include <vector>

#include <iostream>
#include <omp.h>

#include <nn.hpp>
#include <rng.hpp>

using namespace std;

uniform_real_distribution<float> uniform(-1.f, 1.f);

namespace PijersiEngine
{
    void cellsToTensor(uint8_t cells[45], uint8_t currentPlayer, uint8_t input[720])
    {
        if (currentPlayer == 0)
        {
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
        }
        else
        {
            for (int k = 0; k < 45; k++)
            {
                if (cells[44-k] != 0)
                {
                    switch (cells[44-k] & 14)
                    {
                    case 2:
                        input[k * 16] = 1;
                        break;
                    case 6:
                        input[k * 16 + 1] = 1;
                        break;
                    case 10:
                        input[k * 16 + 2] = 1;
                        break;
                    case 14:
                        input[k * 16 + 3] = 1;
                        break;
                    case 0:
                        input[k * 16 + 4] = 1;
                        break;
                    case 4:
                        input[k * 16 + 5] = 1;
                        break;
                    case 8:
                        input[k * 16 + 6] = 1;
                        break;
                    case 12:
                        input[k * 16 + 7] = 1;
                        break;
                    }
                    if (cells[44-k] >= 16)
                    {
                        switch (cells[k] & 224)
                        {
                        case 32:
                            input[k * 16 + 8] = 1;
                            break;
                        case 96:
                            input[k * 16 + 9] = 1;
                            break;
                        case 160:
                            input[k * 16 + 10] = 1;
                            break;
                        case 224:
                            input[k * 16 + 11] = 1;
                            break;
                        case 0:
                            input[k * 16 + 12] = 1;
                            break;
                        case 64:
                            input[k * 16 + 13] = 1;
                            break;
                        case 128:
                            input[k * 16 + 14] = 1;
                            break;
                        case 192:
                            input[k * 16 + 15] = 1;
                            break;
                        }
                    }
                }
            }
        }
    }

    inline float _leakyRelu(float input)
    {
        // return max(0.f, input);
        return max(0.01f*input, input);
    }

    // inline float _dLeakyRelu(float input)
    // {
    //     return (input > 0) ? input : 0.1f*input;
    // }

    void Dense720::init()
    {
        for (int k = 0; k < INPUT_TENSOR_SIZE; k++)
        {
            weights[k] = uniform(gen);
        }
        bias = uniform(gen);
    }

    float Dense720::forward(array<uint8_t, INPUT_TENSOR_SIZE> input)
    {
        float sum = 0;
        for (int k = 0; k < INPUT_TENSOR_SIZE; k++)
        {
            sum += weights[k] * input[k];
        }
        sum += bias;
        return _leakyRelu(sum);
    }

    void Dense720::update(float learning_rate, float biasError, float weightError[INPUT_TENSOR_SIZE])
    {
        for (int k = 0; k < INPUT_TENSOR_SIZE; k++)
        {
            weights[k] -= learning_rate * weightError[k];
        }
        bias -= learning_rate * biasError;
    }

    void Dense32::init()
    {
        for (int k = 0; k < 32; k++)
        {
            weights[k] = uniform(gen);
        }
        bias = uniform(gen);
    }

    float Dense32::forward(array<float, 32> input)
    {
        float sum = 0;
        for (int k = 0; k < 32; k++)
        {
            sum += weights[k] * input[k];
        }
        sum += bias;
        return _leakyRelu(sum);
    }

    void Dense32::update(float learning_rate, float biasError, float weightError[32])
    {
        for (int k = 0; k < 32; k++)
        {
            weights[k] -= learning_rate * weightError[k];
        }
        bias -= learning_rate * biasError;
    }

    void Dense32L::init()
    {
        for (int k = 0; k < 32; k++)
        {
            weights[k] = uniform(gen);
        }
        bias = uniform(gen);
    }

    float Dense32L::forward(array<float, 32> input)
    {
        float sum = 0;
        for (int k = 0; k < 32; k++)
        {
            sum += weights[k] * input[k];
        }
        sum += bias;
        return sum;
    }

    void Dense32L::update(float learning_rate, float biasError, float weightError[32])
    {
        for (int k = 0; k < 32; k++)
        {
            weights[k] -= learning_rate * weightError[k];
            // cout << "weight: "<< weights[k] << endl;
        }
        bias -= learning_rate * biasError;
        // cout << "bias: "<< bias << endl;
    }

    void Network::init()
    {
        for (int k = 0; k < 32; k++)
        {
            layer1[k].init();
            layer2[k].init();
            layer3[k].init();
            final.init();
        }
    }

    void Network::load()
    {

    }

    float Network::forward(array<uint8_t, INPUT_TENSOR_SIZE> input)
    {
        array<float, 32> output1;
        array<float, 32> output2;
        array<float, 32> output3;
        for (int k = 0; k < 32; k++)
        {
            output1[k] = layer1[k].forward(input);
        }
        for (int k = 0; k < 32; k++)
        {
        
            output2[k] = layer2[k].forward(output1);
        }
        for (int k = 0; k < 32; k++)
        {
            output3[k] = layer3[k].forward(output2);
        }
        return final.forward(output3);
    }

    Trainer::Trainer(int newBatchSize)
    {
        init(newBatchSize);
    }

    void Trainer::init(int newBatchSize)
    {
        batchSize = newBatchSize;

        input.resize(batchSize);
        output1.resize(batchSize);
        output2.resize(batchSize);
        output3.resize(batchSize);
        outputFinal.resize(batchSize);

        expected.resize(batchSize);

        error1.resize(batchSize);
        error2.resize(batchSize);
        error3.resize(batchSize);
        errorFinal.resize(batchSize);
        
        for (int k = 0; k < 32; k++)
        {
            layer1[k].init();
            layer2[k].init();
            layer3[k].init();
            final.init();
        }
    }

    void Trainer::forward(uint8_t cells[45], uint8_t currentPlayer, float expectedOutput, int i)
    {
        for (int j = 0; j < INPUT_TENSOR_SIZE; j++)
        {
            input[i][j] = 0;
        }
        cellsToTensor(cells, currentPlayer, input[i].data());

        for (int k = 0; k < 32; k++)
        {
            output1[i][k] = layer1[k].forward(input[i]);
        }
        for (int k = 0; k < 32; k++)
        {
        
            output2[i][k] = layer2[k].forward(output1[i]);
        }
        for (int k = 0; k < 32; k++)
        {
            output3[i][k] = layer3[k].forward(output2[i]);
        }
        outputFinal[i] = final.forward(output3[i]);
        expected[i] = expectedOutput;
    }

    float Trainer::loss()
    {
        float sum = 0.f;
        for (int k = 0; k < batchSize; k++)
        {
            sum += (outputFinal[k] - expected[k]) * (outputFinal[k] - expected[k]);
        }
        return sum * 0.01f / batchSize;
    }

    void Trainer::back(float learning_rate)
    {
        #pragma omp parallel for
        for (int batch = 0; batch < batchSize; batch++)
        {
            // Final layer error
            errorFinal[batch] = (outputFinal[batch] - expected[batch]) * (2.f/(float)batchSize) * 0.01f;
            // cout << "errorFinal[" << i << "] = " << errorFinal[i] << endl;

            // Layer 3 error
            for (int i = 0; i < 32; i++)
            {
                error3[batch][i] = _leakyRelu(final.weights[i] * errorFinal[batch]) * (2.f/(float)batchSize);
                // cout << "error3[" << i << "][" << j << "] = " << error3[i][j] << endl;
            }
            
            // Layer 2 error
            for (int i = 0; i < 32; i++)
            {
                error2[batch][i] = 0.f;
                for (int j = 0; j < 32; j++)
                {
                    error2[batch][i] += _leakyRelu(layer3[j].weights[i] * error3[batch][j]) * (2.f/(float)batchSize);
                }
                // cout << "error2[" << i << "][" << j << "] = " << error2[i][j] << endl;
            }
            
            // Layer 1 error
            for (int i = 0; i < 32; i++)
            {
                error1[batch][i] = 0.f;
                for (int j = 0; j < 32; j++)
                {
                    error1[batch][i] += _leakyRelu(layer2[j].weights[i] * error2[batch][j]) * (2.f/(float)batchSize);
                }
                // cout << "error1[" << i << "][" << j << "] = " << error1[i][j] << endl;
            }
        }

        float biasError1[32] = {0};
        float biasError2[32] = {0};
        float biasError3[32] = {0};
        float biasErrorFinal = 0;

        // Calculating bias error for all filters
        for (int batch = 0; batch < batchSize; batch++)
        {
            biasErrorFinal += errorFinal[batch];
            for (int i = 0; i < 32; i++)
            {
                biasError3[i] += error3[batch][i];
                biasError2[i] += error2[batch][i];
                biasError1[i] += error1[batch][i];
            }
        }

        biasErrorFinal *= (2.f/(float)batchSize);
        for (int i = 0; i < 32; i++)
        {
            biasError3[i] *= (2.f/(float)batchSize);
            biasError2[i] *= (2.f/(float)batchSize);
            biasError1[i] *= (2.f/(float)batchSize);
        }



        float weightError1[INPUT_TENSOR_SIZE*32] = {0};
        float weightError2[32*32] = {0};
        float weightError3[32*32] = {0};
        float weightErrorFinal[32] = {0};

        // Final Layer update
        for (int i = 0; i < 32; i++)
        {
            for (int batch = 0; batch < batchSize; batch++)
            {
                weightErrorFinal[i] += errorFinal[batch] * output3[batch][i];
            }
            weightErrorFinal[i] *= (2.f/(float)batchSize);
        }

        // Hidden Layers update
        for (int i = 0; i < 32; i++)
        {
            for (int j = 0; j < 32; j++)
            {
                for (int batch = 0; batch < batchSize; batch++)
                {
                    weightError3[i*32+j] += error3[batch][i] * output2[batch][j];
                    weightError2[i*32+j] += error2[batch][i] * output1[batch][j];
                }
                weightError3[i*32+j] *= (2.f/(float)batchSize);
                weightError2[i*32+j] *= (2.f/(float)batchSize);

            }
            for (int j = 0; j < INPUT_TENSOR_SIZE; j++)
            {
                for (int batch = 0; batch < batchSize; batch++)
                {
                    weightError1[i*INPUT_TENSOR_SIZE+j] += error1[batch][i] * input[batch][j];
                }
                weightError1[i*INPUT_TENSOR_SIZE+j] *= (2.f/(float)batchSize);
            }
        }

        final.update(learning_rate, biasErrorFinal, weightErrorFinal);

        for (int k  = 0; k < 32; k++)
        {
            layer3[k].update(learning_rate, biasError3[k], weightError3 + 32*k);
            layer2[k].update(learning_rate, biasError2[k], weightError2 + 32*k);
            layer1[k].update(learning_rate, biasError1[k], weightError1 + INPUT_TENSOR_SIZE*k);
        }
    }
}