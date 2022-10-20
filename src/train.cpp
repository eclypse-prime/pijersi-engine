#include <iostream>
#include <nn.hpp>
#include <board.hpp>
#include <alphabeta.hpp>

using namespace std;
using namespace PijersiEngine;

int main(int argc, char **argv)
{
    int niter = 1000;
    int batch_size = 1;
    float learning_rate = 0.1f;
    if (argc < 4)
    {
        cout << "Use train.exe [iterations] [batch size] [learning rate]" << endl;
    }
    else
    {
        niter = stoi(argv[1]);
        batch_size = stoi(argv[2]);
        learning_rate = stof(argv[3]);
    }

    cout << "Training : " << endl;
    cout << "Iterations: " << niter << endl;
    cout << "Batch size: " << batch_size << endl;
    cout << "Learning rate: " << learning_rate << endl;

    Board board;
    board.init();

    Trainer trainer(batch_size);
    uint8_t cells[45] = {1};
    int16_t expected;

    for (int i = 0; i < niter; i++)
    {
        for (int j = 0; j < batch_size; j++)
        {
            if (board.checkWin())
            {
                board.init();
            }

            // expected = (board.currentPlayer == 0) ? _evaluateFuturePosition(0, board.getState(), board.currentPlayer) : -_evaluateFuturePosition(0, board.getState(), board.currentPlayer);
            expected = (board.currentPlayer == 0) ? board.evaluate() : -board.evaluate();
            // expected = 0;
            trainer.forward(board.getState(), board.currentPlayer, expected, j);
            
            board.playRandom();
        }
        cout << "Iter " << i << endl;
        cout << "Loss: " << trainer.loss() << endl;
        trainer.back(learning_rate/(float)(i+1));
        // trainer.back(learning_rate);

        // expected = _evaluateFuturePosition(2, board.getState(), board.currentPlayer);
        // trainer.forward(board.getState(), board.currentPlayer, expected, 0);
        // trainer.back(learning_rate);
        // cout << "Loss: " << trainer.loss() << endl;

        // expected = _evaluateFuturePosition(2, board.getState(), 1);
        // network.set_input(board.getState(), 1);
        // network.forward();
        // network.back(expected, learning_rate);
        // cout << "Output: " << network.output << " Expected: " << expected << endl;
        // cout << "Loss: " << network.loss(expected) << endl;
    }
    for (int i = 0; i < 16; i++)
    {
        if (board.checkWin())
        {
            board.init();
        }

        // expected = (board.currentPlayer == 0) ? _evaluateFuturePosition(0, board.getState(), board.currentPlayer) : -_evaluateFuturePosition(0, board.getState(), board.currentPlayer);
        expected = (board.currentPlayer == 0) ? board.evaluate() : -board.evaluate();
        trainer.forward(board.getState(), board.currentPlayer, expected, i);
        cout << "Output: " << trainer.outputFinal[i] << " Expected: " << trainer.expected[i] << endl;
        
        cout << endl;
        board.playRandom();
    }

    for (int i = 0; i < 720; i++)
    {
        cout << trainer.layer1[0].weights[i] << " ";
    }
    cout << endl;
}