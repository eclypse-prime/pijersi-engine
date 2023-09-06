#ifndef NN_HPP
#define NN_HPP
#include <cstdint>
#include <vector>
#include <array>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#define N_INPUTS 720
#define N_OUTPUTS_1 256
#define N_OUTPUTS_2 32
#define N_OUTPUTS_3 32
#define N_OUTPUTS_4 1

typedef Eigen::SparseMatrix<float> input_t;

typedef Eigen::Matrix<float, N_OUTPUTS_1, 1> output1_t;
typedef Eigen::Matrix<float, N_OUTPUTS_2, 1> output2_t;
typedef Eigen::Matrix<float, N_OUTPUTS_3, 1> output3_t;
typedef Eigen::Matrix<float, N_OUTPUTS_4, 1> output4_t;

typedef Eigen::MatrixXf weights_t;
// typedef Eigen::Matrix<float, N_OUTPUTS_1, N_INPUTS> weights1_t;
// typedef Eigen::Matrix<float, N_OUTPUTS_2, N_OUTPUTS_1> weights2_t;
// typedef Eigen::Matrix<float, N_OUTPUTS_3, N_OUTPUTS_2> weights3_t;
// typedef Eigen::Matrix<float, N_OUTPUTS_4, N_OUTPUTS_3> weights4_t;

typedef Eigen::Matrix<float, N_OUTPUTS_1, 1> bias1_t;
typedef Eigen::Matrix<float, N_OUTPUTS_2, 1> bias2_t;
typedef Eigen::Matrix<float, N_OUTPUTS_3, 1> bias3_t;
typedef Eigen::Matrix<float, N_OUTPUTS_4, 1> bias4_t;

typedef Eigen::Matrix<float, N_INPUTS, Eigen::Dynamic> inputs_trainer_t;
typedef Eigen::Matrix<float, N_OUTPUTS_1, Eigen::Dynamic> outputs_trainer1_t;
typedef Eigen::Matrix<float, N_OUTPUTS_2, Eigen::Dynamic> outputs_trainer2_t;
typedef Eigen::Matrix<float, N_OUTPUTS_3, Eigen::Dynamic> outputs_trainer3_t;
typedef Eigen::Matrix<float, N_OUTPUTS_4, Eigen::Dynamic> outputs_trainer4_t;

typedef Eigen::Matrix<float, N_OUTPUTS_1, Eigen::Dynamic> errors_trainer1_t;
typedef Eigen::Matrix<float, N_OUTPUTS_2, Eigen::Dynamic> errors_trainer2_t;
typedef Eigen::Matrix<float, N_OUTPUTS_3, Eigen::Dynamic> errors_trainer3_t;
typedef Eigen::Matrix<float, N_OUTPUTS_4, Eigen::Dynamic> errors_trainer4_t;

namespace PijersiEngine::NN
{
    input_t cellsToInput(uint8_t cells[45], uint8_t currentPlayer);
    float _sigmoid(float input);
    float _dSigmoid(float input);
    float _leakyRelu(float input);
    float _dLeakyRelu(float input);

    // struct Layer
    // {
    //     float *weights = nullptr;
    //     float *biases = nullptr;
    //     int inputSize = 0;
    //     int outputSize = 0;
    //     bool activation = false;

    //     Layer(int newInputSize, int newOutputSize, bool newUseActivation);
    //     void load();
    //     template <typename T>
    //     void forward(T *input, float *output);
    //     void update(float learningRate, float* weightError, float* biasError);
    //     ~Layer();
    // };

    // struct DenseN_INPUTSx256 : Layer
    // {
    //     DenseN_INPUTSx256();
    // };

    // struct Dense256x32 : Layer
    // {
    //     Dense256x32();
    // };

    // struct Dense32x32 : Layer
    // {
    //     Dense32x32();
    // };

    // struct Dense32x1 : Layer
    // {
    //     Dense32x1();
    // };

    struct Network
    {
        weights_t weights1;
        weights_t weights2;
        weights_t weights3;
        weights_t weights4;

        bias1_t bias1;
        bias2_t bias2;
        bias3_t bias3;
        bias4_t bias4;

        // TODO: Implement incremental updates when possible
        // void setInput(uint8_t cells[45], uint8_t currentPlayer);
        Network();
        void init();
        float forward(uint8_t cells[45], uint8_t currentPlayer);
    };

    struct Trainer
    {
        int batchSize = 1;

        weights_t weights1;
        weights_t weights2;
        weights_t weights3;
        weights_t weights4;

        bias1_t bias1;
        bias2_t bias2;
        bias3_t bias3;
        bias4_t bias4;

        inputs_trainer_t inputs;
        outputs_trainer1_t outputs1;
        outputs_trainer2_t outputs2;
        outputs_trainer3_t outputs3;
        outputs_trainer4_t outputs4;

        errors_trainer1_t errors1;
        errors_trainer2_t errors2;
        errors_trainer3_t errors3;
        errors_trainer4_t errors4;
        errors_trainer4_t targets;

        Trainer(int newBatchSize);
        void setInput(uint8_t cells[45], uint8_t currentPlayer, float target, int batchIndex);
        void forward();
        float loss();
        void back(float learningRate);
        // ~Trainer();
    };
}

#endif