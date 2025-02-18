#include "neural_net.h"

/* I need to improve the random number generator */

static inline ELEMENT_TYPE rand_element(ELEMENT_TYPE high, ELEMENT_TYPE low);
static inline ELEMENT_TYPE sigmoid(ELEMENT_TYPE element);
static inline Mat mat_row(Mat *a, uint32_t row);
static inline void mat_copy(Mat *dst, Mat *src);

Mat *mat_alloc(uint32_t rows, uint32_t cols)
{
    Mat *a = NN_MALLOC(sizeof(Mat));
    if (!a)
    {
        return NULL;
    }

    a->rows = rows;
    a->cols = cols;
    a->es = NN_MALLOC(rows * cols * sizeof(ELEMENT_TYPE));
    if (!a->es)
    {
        NN_FREE(a);
        return NULL;
    }

    a->stride = cols;

    return a;
}

/* don't use yet */
void mat_print(Mat *a, const char *name, uint32_t padding)
{
    printf("%*s%s: [\n", (int)padding, "", name);
    for (uint32_t i = 0; i < a->rows; i++)
    {
        printf("%*s", (int)padding, "");
        for (uint32_t j = 0; j < a->cols; j++)
        {
            printf("    %8f ", MAT_AT(a, i, j));
        }
        printf("\n");
    }
    printf("%*s]\n", (int)padding, "");
}

void mat_rand(Mat *a, ELEMENT_TYPE high, ELEMENT_TYPE low)
{
    if (!high && !low)
    {
        high = MAX_RAND_ELEMENT;
        low = MIN_RAND_ELEMENT;
    }

    // #pragma omp parallel for collapse(2)
    for (uint32_t i = 0; i < a->rows; i++)
    {
        for (uint32_t j = 0; j < a->cols; j++)
        {
            MAT_AT(a, i, j) = rand_element(high, low);
        }
    }
}

/* need to improve to generate truly random numbers using some hardware entropy */
static inline ELEMENT_TYPE rand_element(ELEMENT_TYPE high, ELEMENT_TYPE low)
{
    return (((ELEMENT_TYPE)rand() / (ELEMENT_TYPE)RAND_MAX) * (high - low) + low);
}

void mat_dealloc(Mat *a)
{
    NN_FREE(a->es);
    NN_FREE(a);
    a = NULL;
}

void mat_sum(Mat *dst, Mat *a)
{
    uint32_t rows = a->rows;
    uint32_t cols = a->cols;

    // #pragma omp parallel for collapse(2)
    for (uint32_t i = 0; i < rows; i++)
    {
        for (uint32_t j = 0; j < cols; j++)
        {
            MAT_AT(dst, i, j) += MAT_AT(a, i, j);
        }
    }
}

void mat_fill(Mat *a, ELEMENT_TYPE element)
{
    uint32_t rows = a->rows;
    uint32_t cols = a->cols;

    // #pragma omp parallel for collapse(2)
    for (uint32_t i = 0; i < rows; i++)
    {
        for (uint32_t j = 0; j < cols; j++)
        {
            MAT_AT(a, i, j) = element;
        }
    }
}

void mat_dot(Mat *dst, Mat *a, Mat *b)
{
    uint32_t n = a->cols;
    uint32_t rows = dst->rows;
    uint32_t cols = dst->cols;

    // processing in smaller chunks to avoid stack overflow
    for (uint32_t i = 0; i < rows; i++)
    {
        for (uint32_t j = 0; j < cols; j++)
        {
            ELEMENT_TYPE sum = 0;
            for (uint32_t k = 0; k < n; k++)
            {
                sum += MAT_AT(a, i, k) * MAT_AT(b, k, j);
            }
            MAT_AT(dst, i, j) = sum;
        }
    }
}

static inline ELEMENT_TYPE sigmoid(ELEMENT_TYPE element)
{
    return ((ELEMENT_TYPE)1 / ((ELEMENT_TYPE)1 + EXP(-element)));
}

void mat_sig(Mat *a)
{
    uint32_t rows = a->rows;
    uint32_t cols = a->cols;

    // #pragma omp parallel for collapse(2)
    for (uint32_t i = 0; i < rows; i++)
    {
        for (uint32_t j = 0; j < cols; j++)
        {
            MAT_AT(a, i, j) = sigmoid(MAT_AT(a, i, j));
        }
    }
}

static inline Mat mat_row(Mat *a, uint32_t row)
{
    return (Mat){
        .rows = 1,
        .cols = a->cols,
        .stride = a->cols,
        .es = &MAT_AT(a, row, 0),
    };
}

static inline void mat_copy(Mat *dst, Mat *src)
{
    uint32_t rows = dst->rows;
    uint32_t cols = dst->cols;

    // #pragma omp parallel for collapse(2)
    for (uint32_t i = 0; i < rows; i++)
    {
        for (uint32_t j = 0; j < cols; j++)
        {
            MAT_AT(dst, i, j) = MAT_AT(src, i, j);
        }
    }
}

/* don't use yet */
void nn_print(NN *nn, const char *name, uint32_t padding)
{
    char buffer[64];
    printf("%s: [\n", name);
    for (uint32_t i = 1; i < nn->arch_count; i++)
    {
        sprintf(buffer, "Weigth Matrix, Layer No. %lu", i);
        mat_print(nn->ws[i], buffer, padding);
        sprintf(buffer, "Bias Matrix, Layer No. %lu", i);
        mat_print(nn->bs[i], buffer, padding);
    }
    printf("]\n");
}

NN *nn_alloc(uint32_t *arch, uint32_t arch_count)
{
    NN *nn = NN_MALLOC(sizeof(NN));
    if (!nn)
    {
        return NULL;
    }

    nn->ws = NN_MALLOC(sizeof(Mat *) * (arch_count));
    if (!(nn->ws))
    {
        return NULL;
    }

    nn->bs = NN_MALLOC(sizeof(Mat *) * (arch_count));
    if (!(nn->bs))
    {
        return NULL;
    }

    nn->as = NN_MALLOC(sizeof(Mat *) * (arch_count));
    if (!(nn->as))
    {
        return NULL;
    }

    nn->ds = NN_MALLOC(sizeof(Mat *) * arch_count);
    if (!(nn->ds))
    {
        return NULL;
    }

    for (uint32_t i = 0; i < arch_count; i++)
    {
        if (!i)
        {
            nn->ws[i] = NULL;
            nn->bs[i] = NULL;
            nn->as[i] = mat_alloc(1, arch[i]);
            nn->ds[i] = mat_alloc(1, arch[i]);
        }
        else
        {
            nn->as[i] = mat_alloc(1, arch[i]);
            nn->ws[i] = mat_alloc(arch[i - 1], arch[i]);
            nn->bs[i] = mat_alloc(1, arch[i]);
            nn->ds[i] = mat_alloc(1, arch[i]);
        }
    }

    nn->arch = arch;
    nn->arch_count = arch_count;
    return nn;
}

void randomize_parameters_NN(NN *nn, int high, int low)
{
    uint32_t arch_count = nn->arch_count;

    // #pragma omp parallel for
    for (uint32_t i = 1; i < arch_count; i++)
    {
        mat_rand(nn->ws[i], high, low);
        mat_rand(nn->bs[i], high, low);
    }
}

void forward_NN(NN *nn)
{
    uint32_t arch_count = nn->arch_count;
    for (uint32_t i = 1; i < arch_count; i++)
    {
        mat_dot(nn->as[i], nn->as[i - 1], nn->ws[i]);
        mat_sum(nn->as[i], nn->bs[i]);
        mat_sig(nn->as[i]);
    }
}

ELEMENT_TYPE cost_NN(NN *nn, Mat *training_input, Mat *training_output)
{
    uint32_t rows = training_input->rows;
    uint32_t cols = training_output->cols;

    ELEMENT_TYPE result = (ELEMENT_TYPE)0;
    // #pragma omp parallel for reduction(+:result)
    for (uint32_t i = 0; i < rows; i++)
    {
        Mat x = mat_row(training_input, i);
        Mat y = mat_row(training_output, i);
        Mat *y_ = &y;

        *(NN_INPUT(nn)) = x;
        forward_NN(nn);

        for (uint32_t j = 0; j < cols; j++)
        {
            ELEMENT_TYPE d = MAT_AT(NN_OUTPUT(nn), 0, j) - MAT_AT(y_, 0, j); // correct row access
            result += d * d;
        }
    }
    return (result / rows) + 1e-10;
}

void diff(NN *nn, Mat *training_input, Mat *training_output, ELEMENT_TYPE eps, ELEMENT_TYPE learning_rate, Mat *temp_para, ELEMENT_TYPE c)
{
    ELEMENT_TYPE saved;
    uint32_t rows = temp_para->rows, cols = temp_para->cols;
    for (uint32_t i = 0; i < rows; i++)
    {
        for (uint32_t j = 0; j < cols; j++)
        {
            saved = MAT_AT(temp_para, i, j);
            *MAT_AT_POINTER(temp_para, i, j) += eps;
            ELEMENT_TYPE d = (cost_NN(nn, training_input, training_output) - c) / eps;
            *MAT_AT_POINTER(temp_para, i, j) = saved;
            *MAT_AT_POINTER(temp_para, i, j) -= d * learning_rate;
        }
    }
}

void delta(NN *nn, Mat *training_input, Mat *training_output, ELEMENT_TYPE learning_rate)
{
    uint32_t arch_count = nn->arch_count;
    uint32_t neurons_in_last_layer = NEURONS_IN_LAYER(nn, arch_count - 1);

    for (uint32_t j = 0; j < training_input->rows; j++)
    {
        Mat x = mat_row(training_input, j);
        *(NN_INPUT(nn)) = x;
        forward_NN(nn);

        // #pragma omp parallel for
        for (uint32_t k = 0; k < neurons_in_last_layer; k++)
        {
            ELEMENT_TYPE a = MAT_AT(NN_OUTPUT(nn), 0, k);
            ELEMENT_TYPE error = a - MAT_AT(training_output, j, k);
            MAT_AT(DS_OF_LAYER(nn, arch_count - 1), 0, k) = (2 * error * a * (1 - a)) / neurons_in_last_layer;
        }

        for (uint32_t layer = arch_count - 2; layer > 0; layer--)
        {
            for (uint32_t neuron = 0; neuron < nn->arch[layer]; neuron++)
            {
                ELEMENT_TYPE delta = (ELEMENT_TYPE)0;
                for (uint32_t next_neuron = 0; next_neuron < nn->arch[layer + 1]; next_neuron++)
                {
                    delta += MAT_AT(DS_OF_LAYER(nn, layer + 1), 0, next_neuron) * MAT_AT(nn->ws[layer + 1], neuron, next_neuron);
                }
                ELEMENT_TYPE a = MAT_AT(nn->as[layer], 0, neuron);
                MAT_AT(DS_OF_LAYER(nn, layer), 0, neuron) = delta * a * (1 - a);
            }
        }

        gradient_descent(nn, learning_rate);
    }
}

void gradient_descent(NN *nn, ELEMENT_TYPE learning_rate)
{
    uint32_t arch_count = nn->arch_count;
    // #pragma omp parallel for collapse(1)
    for (uint32_t layer = 1; layer < arch_count; layer++)
    {
        for (uint32_t neuron = 0; neuron < nn->arch[layer]; neuron++)
        {
            for (uint32_t weight = 0; weight < nn->arch[layer - 1]; weight++)
            {
                ELEMENT_TYPE a = MAT_AT(nn->as[layer - 1], 0, weight);
                ELEMENT_TYPE delta = MAT_AT(DS_OF_LAYER(nn, layer), 0, neuron);
                MAT_AT(nn->ws[layer], weight, neuron) -= learning_rate * delta * a;
            }
            MAT_AT(nn->bs[layer], 0, neuron) -= learning_rate * MAT_AT(DS_OF_LAYER(nn, layer), 0, neuron);
        }
    }
}

void learn(NN *nn, ELEMENT_TYPE learning_rate, uint32_t learning_iterations, Mat *training_input, Mat *training_output)
{

    for (uint32_t i = 0; i < learning_iterations; i++)
    {
#if MODEL == DIFF
        uint32_t arch_count = nn->arch_count;
        ELEMENT_TYPE c = cost_NN(nn, training_input, training_output);
        for (uint32_t j = 1; j < arch_count; j++)
        {
            diff(nn, training_input, training_output, eps, learning_rate, nn->ws[j], c);
            diff(nn, training_input, training_output, eps, learning_rate, nn->bs[j], c);
        }
#elif MODEL == GRAD_DESC
        delta(nn, training_input, training_output, learning_rate);
#endif // MODEL
    }
    ELEMENT_TYPE volatile cost = cost_NN(nn, training_input, training_output);
    // printf("Final Cost: %f\n", cost);
}
