#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset
#include "DSP/Discrete/Derivative.h"
#include "DSP/Discrete/Discontinuous.h"


dsp_derivative_t* dsp_derivative_create(const real_t K, const real_t N, const real_t Ts, const s_approximation_t DF, const bool limit_output, const real_t upper_limit, const real_t lower_limit) {

    // Create a new derivative
    dsp_derivative_t* const derivative = malloc(sizeof(dsp_derivative_t));
    if (derivative == NULL) { return 0; }

    if (dsp_derivative_configure(derivative, K, N, Ts, DF, limit_output, upper_limit, lower_limit)) {
        return derivative;
    }
    else {
        free(derivative);
        return NULL;
    }
}

dsp_derivative_t* dsp_derivative_create_copy(const dsp_derivative_t* const other) {
    if (other == NULL) { return NULL; }

    // Create a new derivative
    dsp_derivative_t* const derivative = malloc(sizeof(dsp_derivative_t));
    if (derivative == NULL) { return NULL; }

    if (dsp_derivative_copy_assign(derivative, other)) {
        return derivative;
    }
    else {
        free(derivative);
        return NULL;
    }
}
bool dsp_derivative_copy_assign(dsp_derivative_t* const dest, const dsp_derivative_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }

    *dest = *src;
    return true;
}

dsp_derivative_t* dsp_derivative_create_move(dsp_derivative_t* const other) {
    if (other == NULL) { return NULL; }

    // Create a new derivative
    dsp_derivative_t* const derivative = dsp_derivative_create_copy(other);

    if (derivative != NULL) { memset(other, 0, sizeof(dsp_derivative_t)); }
    return derivative;
}
bool dsp_derivative_move_assign(dsp_derivative_t* const dest, dsp_derivative_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }

    dsp_derivative_copy_assign(dest, src);
    memset(src, 0, sizeof(dsp_derivative_t));
    return true;
}

bool dsp_derivative_swap(dsp_derivative_t* const a, dsp_derivative_t* const b) {
    if (a == NULL || b == NULL) { return true; }
    if (a == b) { return false; }

    // Swap
    dsp_derivative_t temp = *a;
    *a = *b;
    *b = temp;
    return true;
}

bool dsp_derivative_destroy(dsp_derivative_t* const derivative) {
    if (derivative == NULL) { return false; }

    free(derivative);
    return true;
}

bool dsp_derivative_configure(dsp_derivative_t* const derivative, const real_t K, const real_t N, const real_t Ts, const s_approximation_t DF, const bool limit_output, const real_t upper_limit, const real_t lower_limit) {
    if (derivative == NULL) { return false; }

    dsp_derivative_set_parameters(derivative, K, N, Ts, DF);
    dsp_derivative_set_output_saturation(derivative, limit_output, upper_limit, lower_limit);
    dsp_derivative_set_state(derivative, 0);
    return true;
}

bool dsp_derivative_set_parameters(dsp_derivative_t* const derivative, const real_t K, const real_t N, const real_t Ts, const s_approximation_t DF) {
    if (derivative == NULL) { return false; }

    real_t a0, a1, b0, b1;
    switch (DF) {

        case ForwardEuler:
            a0 = 1.0f;
            a1 = N * Ts - 1.0f;
            b0 = K * N;
            b1 = -K * N;
            break;

        case BackwardEuler:
            a0 = (N * Ts + 1.0f);
            a1 = -1.0f;
            b0 = K * N;
            b1 = -K * N;
            break;

        case Trapezoidal:
            a0 = (N * Ts + 2.0f);
            a1 = (N * Ts - 2.0f);
            b0 = 2.0f * K * N;
            b1 = -2.0f * K * N;
            break;

        default:
            a0 = 1.0f;
            a1 = 0.0f;
            b0 = 0.0f;
            b1 = 0.0f;
            break;
    }

    derivative->D = b0 / a0;
    derivative->A = -a1 / a0;
    derivative->B = b1 / a0 + derivative->D * derivative->A;
    return true;
}

bool dsp_derivative_set_output_saturation(dsp_derivative_t* const derivative, const bool limit_output, const real_t upper_limit, const real_t lower_limit) {
    if (derivative == NULL) { return false; }

    derivative->limit_output = limit_output;
    derivative->upper_limit = upper_limit;
    derivative->lower_limit = lower_limit;
    return true;
}




real_t dsp_derivative_get_state(dsp_derivative_t* const derivative) {
    if (derivative == NULL) { return 0; }
    return derivative->x;
}

bool dsp_derivative_set_state(dsp_derivative_t* const derivative, const real_t x0) {
    if (derivative == NULL) { return false; }

    derivative->x = x0;
    return true;
}

bool dsp_derivative_reset(dsp_derivative_t* const derivative) {
    return dsp_derivative_set_state(derivative, 0);
}



real_t dsp_derivative_get_output(dsp_derivative_t* const derivative, const real_t u) {
    if (derivative == NULL) { return 0; }

    // y(n) = x(n) + D * u(n) // C == 1
    const real_t output = derivative->x + derivative->D * u;

    return dsp_discontinuous_saturate_if(
        derivative->limit_output, output, 
        derivative->upper_limit, derivative->lower_limit
    );
}

real_t dsp_derivative_update_state(dsp_derivative_t* const derivative, const real_t u) {
    if (derivative == NULL) { return 0; }

    // x(n+1) = A * x(n) + B * u(n)
    derivative->x = derivative->A * derivative->x + derivative->B * u;

    return derivative->x;
}

real_t dsp_derivative_update(dsp_derivative_t* const derivative, const real_t u) {
    if (derivative == NULL) { return 0; }

    // y(n) = x(n) + D * u(n) // C == 1
    const real_t output = derivative->x + derivative->D * u;

    // x(n+1) = A * x(n) + B * u(n)
    derivative->x = derivative->A * derivative->x + derivative->B * u;

    return dsp_discontinuous_saturate_if(
        derivative->limit_output, output, 
        derivative->upper_limit, derivative->lower_limit
    );
}
