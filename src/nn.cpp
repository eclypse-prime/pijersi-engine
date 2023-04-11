#include <algorithm>
#include <cstdint>
#include <vector>

#include <iostream>
#include <omp.h>

#include <nn.hpp>
#include <rng.hpp>

namespace PijersiEngine::NN
{
    input_t cellsToInput(uint8_t cells[45], uint8_t currentPlayer)
    {
        input_t input = input_t::Zero();
        if (currentPlayer == 0)
        {
            for (int k = 0; k < 45; k++)
            {
                if (cells[k] != 0)
                {
                    switch (cells[k] & 14)
                    {
                    case 0:
                        input(k * 16) = 1;
                        break;
                    case 4:
                        input(k * 16 + 1) = 1;
                        break;
                    case 8:
                        input(k * 16 + 2) = 1;
                        break;
                    case 12:
                        input(k * 16 + 3) = 1;
                        break;
                    case 2:
                        input(k * 16 + 4) = 1;
                        break;
                    case 6:
                        input(k * 16 + 5) = 1;
                        break;
                    case 10:
                        input(k * 16 + 6) = 1;
                        break;
                    case 14:
                        input(k * 16 + 7) = 1;
                        break;
                    }
                    if (cells[k] >= 16)
                    {
                        switch (cells[k] & 224)
                        {
                        case 0:
                            input(k * 16 + 8) = 1;
                            break;
                        case 64:
                            input(k * 16 + 9) = 1;
                            break;
                        case 128:
                            input(k * 16 + 10) = 1;
                            break;
                        case 192:
                            input(k * 16 + 11) = 1;
                            break;
                        case 32:
                            input(k * 16 + 12) = 1;
                            break;
                        case 96:
                            input(k * 16 + 13) = 1;
                            break;
                        case 160:
                            input(k * 16 + 14) = 1;
                            break;
                        case 224:
                            input(k * 16 + 15) = 1;
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
                if (cells[44 - k] != 0)
                {
                    switch (cells[44 - k] & 14)
                    {
                    case 2:
                        input(k * 16) = 1;
                        break;
                    case 6:
                        input(k * 16 + 1) = 1;
                        break;
                    case 10:
                        input(k * 16 + 2) = 1;
                        break;
                    case 14:
                        input(k * 16 + 3) = 1;
                        break;
                    case 0:
                        input(k * 16 + 4) = 1;
                        break;
                    case 4:
                        input(k * 16 + 5) = 1;
                        break;
                    case 8:
                        input(k * 16 + 6) = 1;
                        break;
                    case 12:
                        input(k * 16 + 7) = 1;
                        break;
                    }
                    if (cells[44 - k] >= 16)
                    {
                        switch (cells[k] & 224)
                        {
                        case 32:
                            input(k * 16 + 8) = 1;
                            break;
                        case 96:
                            input(k * 16 + 9) = 1;
                            break;
                        case 160:
                            input(k * 16 + 10) = 1;
                            break;
                        case 224:
                            input(k * 16 + 11) = 1;
                            break;
                        case 0:
                            input(k * 16 + 12) = 1;
                            break;
                        case 64:
                            input(k * 16 + 13) = 1;
                            break;
                        case 128:
                            input(k * 16 + 14) = 1;
                            break;
                        case 192:
                            input(k * 16 + 15) = 1;
                            break;
                        }
                    }
                }
            }
        }
        return input;
    }

    float _sigmoid(float input)
    {
        return 1.f / (1.f + exp(-input));
    }

    float _dSigmoid(float input)
    {
        return input * (1 - input);
    }

    float _leakyRelu(float input)
    {
        return std::max(0.f, input);
        // return std::max(0.01f * input, input);
    }

    float _dLeakyRelu(float input)
    {
        // return (input > 0) ? 1 : 0.01f;
        return (input > 0) ? 1 : 0.f;
    }

    Network::Network()
    {
    }

    void Network::init()
    {
        weights1 = weights_t::Random(N_OUTPUTS_1, N_INPUTS);
        weights2 = weights_t::Random(N_OUTPUTS_2, N_OUTPUTS_1);
        weights3 = weights_t::Random(N_OUTPUTS_3, N_OUTPUTS_2);
        weights4 = weights_t::Random(N_OUTPUTS_4, N_OUTPUTS_3);

        bias1 = bias1_t::Random();
        bias2 = bias2_t::Random();
        bias3 = bias3_t::Random();
        bias4 = bias4_t::Random();
    }

    float Network::forward(uint8_t cells[45], uint8_t currentPlayer)
    {
        input_t input = cellsToInput(cells, currentPlayer);
        output1_t output1 = weights1 * input + bias1;
        output1 = output1.cwiseMax(0.0f);
        output2_t output2 = weights2 * output1 + bias2;
        output2 = output2.cwiseMax(0.0f);
        output3_t output3 = weights3 * output2 + bias3;
        output3 = output3.cwiseMax(0.0f);
        output4_t output4 = weights4 * output3 + bias4;
        return output4(0);
    }

    Trainer::Trainer(int newBatchSize)
    {
        batchSize = newBatchSize;

        weights1 = weights_t::Random(N_OUTPUTS_1, N_INPUTS);
        weights2 = weights_t::Random(N_OUTPUTS_2, N_OUTPUTS_1);
        weights3 = weights_t::Random(N_OUTPUTS_3, N_OUTPUTS_2);
        weights4 = weights_t::Random(N_OUTPUTS_4, N_OUTPUTS_3);

        bias1 = bias1_t::Random();
        bias2 = bias2_t::Random();
        bias3 = bias3_t::Random();
        bias4 = bias4_t::Random();

        inputs = inputs_trainer_t::Zero(N_INPUTS, batchSize);
        outputs1 = outputs_trainer1_t::Zero(N_OUTPUTS_1, batchSize);
        outputs2 = outputs_trainer2_t::Zero(N_OUTPUTS_2, batchSize);
        outputs3 = outputs_trainer3_t::Zero(N_OUTPUTS_3, batchSize);
        outputs4 = outputs_trainer4_t::Zero(N_OUTPUTS_4, batchSize);

        errors1 = errors_trainer1_t::Zero(N_OUTPUTS_1, batchSize);
        errors2 = errors_trainer2_t::Zero(N_OUTPUTS_2, batchSize);
        errors3 = errors_trainer3_t::Zero(N_OUTPUTS_3, batchSize);
        errors4 = errors_trainer4_t::Zero(N_OUTPUTS_4, batchSize);
        targets = errors_trainer4_t::Zero(N_OUTPUTS_4, batchSize);
    }

    void Trainer::setInput(uint8_t cells[45], uint8_t currentPlayer, float target, int batchIndex)
    {
        inputs.col(batchIndex) = cellsToInput(cells, currentPlayer);
        targets(batchIndex) = target;
    }

    void Trainer::forward()
    {
        outputs1 = (weights1 * inputs).colwise() + bias1;
        outputs1 = outputs1.cwiseMax(0.0f);

        outputs2 = (weights2 * outputs1).colwise() + bias2;
        outputs2 = outputs2.cwiseMax(0.0f);

        outputs3 = (weights3 * outputs2).colwise() + bias3;
        outputs3 = outputs3.cwiseMax(0.0f);

        outputs4 = (weights4 * outputs3).colwise() + bias4;
    }

    float Trainer::loss()
    {
        errors_trainer4_t errors4 = targets - outputs4;
        return errors4.squaredNorm() / (float)batchSize;
    }

    void Trainer::back(float learningRate)
    {
        errors4 = outputs4 - targets;

        errors3 = weights4.transpose() * errors4;
        errors3 = errors3.cwiseProduct((errors_trainer3_t)(outputs3.array() > 0).cast<float>());

        errors2 = weights3.transpose() * errors3;
        errors2 = errors2.cwiseProduct((errors_trainer2_t)(outputs2.array() > 0).cast<float>());

        errors1 = weights2.transpose() * errors2;
        errors1 = errors1.cwiseProduct((errors_trainer1_t)(outputs1.array() > 0).cast<float>());

        bias1_t biasError1 = errors1.rowwise().sum() * (2 / (float)batchSize);
        bias2_t biasError2 = errors2.rowwise().sum() * (2 / (float)batchSize);
        bias3_t biasError3 = errors3.rowwise().sum() * (2 / (float)batchSize);
        bias4_t biasError4 = errors4.rowwise().sum() * (2 / (float)batchSize);

        weights_t weightsError1 = errors1 * inputs.transpose();
        weights_t weightsError2 = errors2 * outputs1.transpose();
        weights_t weightsError3 = errors3 * outputs2.transpose();
        weights_t weightsError4 = errors4 * outputs3.transpose();

        weightsError1 *= (2 / (float)batchSize);
        weightsError2 *= (2 / (float)batchSize);
        weightsError3 *= (2 / (float)batchSize);
        weightsError4 *= (2 / (float)batchSize);

        bias1 -= learningRate * biasError1;
        bias2 -= learningRate * biasError2;
        bias3 -= learningRate * biasError3;
        bias4 -= learningRate * biasError4;

        weights1 -= learningRate * weightsError1;
        weights2 -= learningRate * weightsError2;
        weights3 -= learningRate * weightsError3;
        weights4 -= learningRate * weightsError4;
    }
}