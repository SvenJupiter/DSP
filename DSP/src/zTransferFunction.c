// #include "stdafx.h"
#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset, memmove
#include <math.h> // expf
#include "DSP/Discrete/zTransferFunction.h"

#define ZFT_SIZE sizeof(dsp_ztf_t)
#define NEW_ZTF() ((dsp_ztf_t*) malloc(ZFT_SIZE))

#define REAL_SIZE sizeof(real_t)
#define ARRAY_SIZE(order) ((order+1) * REAL_SIZE)
#define NEW_ARRAY(order) ((real_t*) malloc(ARRAY_SIZE(order)))


static real_t dot_product(const real_t* const v1, const real_t* const v2, const size_t size) {
    real_t sum = 0;
    for (size_t i = 0; i < size; ++i) {
        sum += v1[i] * v2[i];
    }
    return sum;
}


// Create
dsp_ztf_t* dsp_ztf_create(const size_t order) {

    // Allocate memory for 'zTransferFunction'
    dsp_ztf_t* const ztf = NEW_ZTF();
    if (ztf == NULL) { return NULL; }

    // Allocate memory for its members
    ztf->order = order;
    ztf->a = NEW_ARRAY(order+1);
    ztf->b = NEW_ARRAY(order+1);
    ztf->u = NEW_ARRAY(order+1);
    ztf->y = NEW_ARRAY(order+1);

    // If memeory allocation failed
    if (ztf->a == NULL || ztf->b == NULL || ztf->u == NULL || ztf->y == NULL) {
        if (ztf->a != NULL) { free(ztf->a); }
        if (ztf->b != NULL) { free(ztf->b); }
        if (ztf->u != NULL) { free(ztf->u); }
        if (ztf->y != NULL) { free(ztf->y); }
        free(ztf);
        return NULL;
    }
    else {
        return ztf;
    }
}

dsp_ztf_t* dsp_ztf_create_from_arrays(const size_t order, const real_t* const num, const real_t* const den, const real_t* initial_u, const real_t* initial_y) {

    // Allocate memory for 'z_transfer_function_struct'
    dsp_ztf_t* const ztf = dsp_ztf_create(order);
    if (ztf == NULL) { return NULL; }
    else {
        dsp_ztf_set_initial_condition(ztf, initial_u, initial_y);
        dsp_ztf_set_coefficients(ztf, num, den);
        return ztf;
    }
}

dsp_ztf_t* dsp_ztf_create_PT1(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y) {

    const real_t a = expf(- Ts / T);
    const real_t b = K * (1 - a);

    const real_t num[2] = {0, b};
    const real_t den[2] = {1, -a};
    return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
}

dsp_ztf_t* dsp_ztf_create_lead_lag_compensator(const real_t T1, const real_t T2, const real_t Ts, const real_t initial_u, const real_t initial_y) {

    const real_t num[2] = {T1, (Ts - T1)};
    const real_t den[2] = {T2, (Ts - T2)};
    return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
}

dsp_ztf_t* dsp_ztf_create_lowpass_filter(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y) {

    const real_t num[2] = {0, (K * Ts) / T};
    const real_t den[2] = {1, (Ts / T - 1)};
    return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
}

dsp_ztf_t* dsp_ztf_create_highpass_filter(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y) {

    const real_t num[2] = {K, -K};
    const real_t den[2] = {1, (Ts / T - 1)};
    return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
}

dsp_ztf_t* dsp_ztf_create_integrator(const real_t K, const real_t Ts, const real_t initial_u, const real_t initial_y, const s_approximation_t IF) {
    switch (IF) {
        case ForwardEuler: {
            const real_t num[2] = {0, K * Ts};
            const real_t den[2] = {1, -1};
            return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
        }
        case BackwardEuler: {
            const real_t num[2] = {K * Ts, 0};
            const real_t den[2] = {1, -1};
            return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
        }
        case Trapezoidal: {
            const real_t num[2] = {K * Ts, K * Ts};
            const real_t den[2] = {2, -2};
            return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
        }
        default: {
            return NULL;
        }
    }
}

dsp_ztf_t* dsp_ztf_create_derivative(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y, const s_approximation_t DF) {
    switch (DF) {
        case ForwardEuler: {
            const real_t num[2] = {K, -K};
            const real_t den[2] = {T, (Ts - T)};
            return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
        }
        case BackwardEuler: {
            const real_t num[2] = {K, -K};
            const real_t den[2] = {(T+Ts), -T};
            return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
        }
        case Trapezoidal: {
            const real_t num[2] = {(2 * K), -(2 * K)};
            const real_t den[2] = {(Ts + 2 * T), (Ts - 2 * T)};
            return dsp_ztf_create_from_arrays(1, num, den, &initial_u, &initial_y);
        }
        default: {
            return NULL;
        }
    }
}

// Create
bool dsp_ztf_destroy(dsp_ztf_t* const ztf) {
    if (ztf == NULL) { return false; }

    if (ztf->a != NULL) { free(ztf->a); }
    if (ztf->b != NULL) { free(ztf->b); }
    if (ztf->u != NULL) { free(ztf->u); }
    if (ztf->y != NULL) { free(ztf->y); }
    free(ztf);
    return true;
}





bool dsp_ztf_set_coefficients(dsp_ztf_t* const ztf, const real_t* const num, const real_t* const den) {
    if (ztf == NULL) { return false; }

    // set initial numerator coefficients
    if (num == NULL) { ztf->b[0] = 1; memset(&(ztf->b[1]), 0, ztf->order * sizeof(real_t)); }
    else if (num != ztf->b) { memcpy(ztf->b, num, (ztf->order + 1) * sizeof(real_t)); }


    // set initial numerator coefficients
    if (den == NULL) { ztf->a[0] = 1; memset(&(ztf->a[1]), 0, ztf->order * sizeof(real_t)); }
    else if (den != ztf->a) { memcpy(ztf->a, den, (ztf->order + 1) * sizeof(real_t)); }
    
    return true;
}

bool dsp_ztf_set_initial_condition(dsp_ztf_t* const ztf, const real_t* initial_u, const real_t* initial_y) {
    if (ztf == NULL) { return false; }

    // set initial 'u' values
    if (initial_u == NULL) { memset(ztf->u, 0, ztf->order * sizeof(real_t)); }
    else if (initial_u != ztf->u) { memcpy(ztf->u, initial_u, ztf->order * sizeof(real_t)); }

    // set initial 'y' values
    if (initial_y == NULL) { memset(ztf->y, 0, ztf->order * sizeof(real_t)); }
    else if (initial_y != ztf->y) { memcpy(ztf->y, initial_y, ztf->order * sizeof(real_t)); }

    return true;
}

bool dsp_ztf_reset(dsp_ztf_t* const ztf) {
    return dsp_ztf_set_initial_condition(ztf, NULL, NULL);
}

real_t dsp_ztf_update(dsp_ztf_t* const ztf, const real_t new_u) {
    if (ztf == NULL) { return 0; }

    // shift values
    memmove(&(ztf->u[1]), &(ztf->u[0]), ztf->order * sizeof(real_t));
    memmove(&(ztf->y[1]), &(ztf->y[0]), ztf->order * sizeof(real_t));

    // calculate new value
    ztf->u[0] = new_u;
    const real_t bu = dot_product(ztf->b, ztf->u, (ztf->order + 1));
    const real_t ay = dot_product(&(ztf->a[1]), &(ztf->y[1]), ztf->order);
    ztf->y[0] = (bu - ay) / ztf->a[0];
    return ztf->y[0];
}

real_t dsp_ztf_output(dsp_ztf_t* const ztf) {
    if (ztf == NULL) { return 0; }
    return ztf->y[0];
}

