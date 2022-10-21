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

    Trainer trainer(batchSize);

    for (int n = 0; n < nIter; n++)
    {
        cout << "Iter " << n << endl;
        // for (int batchIndex = 0; batchIndex < batchSize/2; batchIndex++)
        for (int batchIndex = 0; batchIndex < batchSize; batchIndex++)
        {
            float target = board.evaluate();
            trainer.setInput(board.getState(), 0, target, batchIndex*2);
            // target = -board.evaluate();
            // trainer.setInput(board.getState(), 1, target, batchIndex*2+1);
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
        trainer.back(learningRate);

        float target = (board.currentPlayer == 0) ? board.evaluate() : -board.evaluate();
        trainer.network.setInput(board.getState(), board.currentPlayer);
        cout << "Target: " << target << " " << "Prediction: " << trainer.network.forward() << endl;
        // for (int i = 0; i < 32; i++)
        // {
        //     cout << trainer.network.layer4.weights[i] << " ";
        // }
        // cout << endl;
    }
}