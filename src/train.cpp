#include <iostream>
#include <nn.hpp>
#include <board.hpp>
#include <alphabeta.hpp>

using namespace std;
using namespace PijersiEngine;


int main(int argc, char **argv)
{
    int nIter = 1000;
    int batchSize = 1;
    float learningRate = 0.1f;
    if (argc < 4)
    {
        cout << "Use train.exe [iterations] [batch size] [learning rate]" << endl;
    }
    else
    {
        nIter = stoi(argv[1]);
        batchSize = stoi(argv[2]);
        learningRate = stof(argv[3]);
    }

    cout << "Training : " << endl;
    cout << "Iterations: " << nIter << endl;
    cout << "Batch size: " << batchSize << endl;
    cout << "Learning rate: " << learningRate << endl;

    Board board;
    board.init();

    uint8_t cells[45] = {1};

    Network network;

    // network.setInput(cells, 0);
    // for (int i = 0; i < 720; i++)
    // {
    //     cout << (int)network.input[i] << " ";
    // }
    // cout << endl;
    // network.setInput(cells, 1);

    // for (int i = 0; i < 720; i++)
    // {
    //     cout << (int)network.input[i] << " ";
    // }
    // cout << endl;

    Trainer trainer(batchSize);

    for (int n = 0; n < nIter; n++)
    {
        cout << "Iter " << n << endl;
        for (int batchIndex = 0; batchIndex < batchSize/2; batchIndex++)
        // for (int batchIndex = 0; batchIndex < batchSize; batchIndex++)
        {
            // float target = _sigmoid((float)_evaluateFuturePosition(0, board.getState(), 0)*0.01f) - 0.5f;
            float target = _sigmoid((float)board.evaluate() * 0.01f) - 0.5f;
            trainer.setInput(board.getState(), 0, target, batchIndex*2);
            // target = _sigmoid(-(float)_evaluateFuturePosition(0, board.getState(), 1)*0.01f) - 0.5f;
            target = _sigmoid(-(float)board.evaluate() * 0.01f) - 0.5f;
            trainer.setInput(board.getState(), 1, target, batchIndex*2+1);
            if (!board.checkWin())
            {
                board.playRandom();
            }
            else
            {
                board.init();
            }
        }
        trainer.forward();
        cout << "Loss: " << trainer.loss() << endl;
        trainer.back(learningRate / ((float) (n + 1)));

        // float target1 = board.evaluate();
        // trainer.network.setInput(board.getState(), 0);
        // cout << "Target: " << target1 << " " << "Prediction: " << trainer.network.forward() << endl;
        // float target2 = -board.evaluate();
        // trainer.network.setInput(board.getState(), 1);
        // cout << "Target: " << target2 << " " << "Prediction: " << trainer.network.forward() << endl;
        // for (int i = 0; i < 32; i++)
        // {
        //     cout << trainer.network.layer4.weights[i] << " ";
        // }
        // cout << endl;
    }
}