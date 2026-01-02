#include "adder.h"
#include <iostream>

#define NUM 3

int main() {

    data_t A[NUM] = {190, 232, 1123};
    data_t B[NUM] = {87, 45, 0};
    data_t C[NUM] = {905, 15, 274};

    data_t gnd_truth[NUM] = {0};
    for (int i = 0; i < NUM; i++) {
        gnd_truth[i] = A[i] + 2 * B[i] - C[i];
    }

    data_t out[NUM] = {0};
    for (int i = 0; i < NUM; i++) {
        adder(A[i], B[i], C[i], out[i]);
    }

    int ret_val = 0;
    for (int i = 0; i < NUM; i++) {
        if (gnd_truth[i] != out[i]) {
            ret_val++;
        }
    }

    std::cout << "**************************" << std::endl;
    std::cout << " There were " << ret_val << " errors" << std::endl;
    std::cout << "**************************" << std::endl;

    return ret_val;
}