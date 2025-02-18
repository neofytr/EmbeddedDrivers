#ifndef NN_H_
#define NN_H_

#define GRAD_DESC 100
#define DIFF 200

#ifndef DECAY_RATE
#define DECAY_RATE 1e-1
#endif

#ifndef MODEL
#define MODEL GRAD_DESC
#endif // MODEL

#ifndef NN_ASSERT
#define NN_ASSERT assert
#endif // NN_ASSERT

#ifndef ELEMENT_TYPE
#define ELEMENT_TYPE float
#endif // ELEMENT_TYPE

#ifndef NN_MALLOC
#define NN_MALLOC neo_alloc
#endif // NN_MALLOC

#ifndef NN_FREE
#define NN_FREE neo_free
#endif // NN_FREE

#ifndef MAX_RAND_ELEMENT
#define MAX_RAND_ELEMENT 1.f
#endif // MAX_RAND_ELEMENT

#ifndef MIN_RAND_ELEMENT
#define MIN_RAND_ELEMENT 0.f
#endif // MIN_RAND_ELEMENT

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include "neo_alloc.h"
#include "expf.h"

#if ELEMENT_TYPE == float
#define EXP expf
#elif ELEMENT_TYPE == double
#define EXP exp
#endif // EXP

#define SUCCESS 1
#define FAILURE 0
#define MAT_AT(m, i, j) ((m)->es[(i) * (m)->stride + (j)]) 
#define MAT_AT_POINTER(a, i, j) & (a->es[(i) * (a->stride) + (j)])
#define MAT_PRINT(m) mat_print(m, #m, 0) // #m converts the tokens in m into a string
#define OUTPUT_AT(nn, input_set_no, output_set_col) (nn->model_output[input_set_no * NEURONS_IN_LAYER(nn, 0) + output_set_col])
#define NN_PRINT(n) nn_print(n, #n, 4)
#define NUMS(a) (sizeof(a) / sizeof(a[0]))
#define NEURONS_IN_LAYER(nn, layer_no) ((nn->arch[layer_no]))
#define DS_OF_LAYER(nn, layer_no) (nn->ds[layer_no])
#define ARRAY_LEN(xs) (sizeof(xs) / sizeof(xs[0]))

typedef struct
{
    uint32_t rows;
    uint32_t cols;
    uint32_t stride;
    ELEMENT_TYPE *es;
} Mat;

Mat *mat_alloc(uint32_t rows, uint32_t cols);
void mat_dealloc(Mat *a);
void mat_rand(Mat *a, ELEMENT_TYPE high, ELEMENT_TYPE low);
void mat_dot(Mat *dst, Mat *a, Mat *b);
void mat_sum(Mat *dst, Mat *a);
void mat_print(Mat *a, const char *name, uint32_t padding);
void mat_sig(Mat *a);

typedef struct
{
    uint32_t arch_count;
    uint32_t *arch;
    ELEMENT_TYPE *model_output;
    Mat **ws; // Weights
    Mat **bs; // Biases
    Mat **as; // Activations
    Mat **ds; // Deltas
} NN;

#define NN_INPUT(n) (n->as[0])
#define NN_OUTPUT(n) (n->as[n->arch_count - 1])

void nn_print(NN *nn, const char *name, uint32_t padding);
NN *nn_alloc(uint32_t *arch, uint32_t arch_count);
void nn_dealloc(NN *nn);
void randomize_parameters_NN(NN *nn, int high, int low);
void forward_NN(NN *nn);
ELEMENT_TYPE cost_NN(NN *nn, Mat *training_input, Mat *training_output);
void gradient_descent(NN *nn, ELEMENT_TYPE learning_rate);
void diff(NN *nn, Mat *training_input, Mat *training_output, ELEMENT_TYPE eps, ELEMENT_TYPE learning_rate, Mat *temp_para, ELEMENT_TYPE cost);
void delta(NN *nn, Mat *training_input, Mat *training_output, ELEMENT_TYPE learning_rate);
void learn(NN *nn, ELEMENT_TYPE learning_rate, uint32_t learning_iterations, Mat *training_input, Mat *training_output);

#endif // NN_H_