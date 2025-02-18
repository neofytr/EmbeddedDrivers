#include "expf.h"

static inline float expf_poly(float r)
{
    return 1.0f + r * (1.0f + r * (0.5f + r * (0.16666667f + r * (0.04166667f + r * 0.00833333f))));
}

float expf(float x)
{
    if (x == 0.0f)
        return 1.0f;
    if (x < -100.0f)
        return 0.0f;

    const float LOG2E = 1.4426950409f;
    const float LN2 = 0.6931471806f;

    int k = (int)(x * LOG2E + (x >= 0 ? 0.5f : -0.5f));
    float r = x - k * LN2;

    float er = expf_poly(r);

    int32_t twok = (k + 127) << 23;
    float twok_f;
    *(int32_t *)&twok_f = twok;

    return twok_f * er;
}
