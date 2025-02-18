#include "neural_net.h"

ELEMENT_TYPE xor_data[] = {
    // Input pairs followed by output
    1, 0, 0, // 1 XOR 0 = 1
    0, 1, 1, // 0 XOR 1 = 1
    1, 1, 0, // 1 XOR 1 = 0
    0, 0, 0  // 0 XOR 0 = 0
};

int main()
{
    neo_heap_init();
    uint32_t arch[] = {2, 2, 1};
    Mat *training_input = neo_alloc(sizeof(Mat));
    *training_input = (Mat){
        .rows = 4,
        .cols = 2,
        .stride = 3,       // Each row is 3 elements apart in memory
        .es = &xor_data[0] // Points to first input
    };

    // Properly initialize output matrix
    Mat *training_output = neo_alloc(sizeof(Mat));
    *training_output = (Mat){
        .rows = 4,
        .cols = 1,
        .stride = 3,       // Each row is 3 elements apart in memory
        .es = &xor_data[2] // Points to first output (third element in each row)
    };

    NN *nn = nn_alloc(arch, ARRAY_LEN(arch));
    randomize_parameters_NN(nn, 10, 0);

    learn(nn,
          0.01,
          100000,
          training_input,
          training_output);
    ELEMENT_TYPE volatile out = 0.9;
    for (uint32_t a1 = 0; a1 < 2; a1++)
    {
        for (uint32_t a0 = 0; a0 < 2; a0++)
        {
            ELEMENT_TYPE *input = neo_alloc(sizeof(ELEMENT_TYPE) * 2);
            input[0] = (ELEMENT_TYPE)a1;
            input[1] = (ELEMENT_TYPE)a0;
            Mat input_mat = {
                .rows = 1,
                .cols = 2,
                .es = input};
            *(NN_INPUT(nn)) = input_mat;
            forward_NN(nn);
            out = NN_OUTPUT(nn)->es[0];
            neo_free(input);
            /* printf("%lu + %lu = %f\n",
       a1, a0,
       NN_OUTPUT(nn)->es[0]); */
        }
    }

    while (true)
    {
    }
}