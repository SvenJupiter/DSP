#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset
#include <math.h> // expf
#include "DSP/Discrete/zStateSpace.h"


#define ZSS_SIZE sizeof(dsp_zss_t)
#define NEW_ZSS() ((dsp_zss_t*) malloc(ZSS_SIZE))

#define ARRAY_ELEMEMT(array, index) ((array)[(index)])
#define POLYNOMIAL_ELEMENT(poly, index) ((poly)->a[(index)])
#define VECTOR_ELEMENT(vec, index) ((vec)->elements[(index)])
#define MATRIX_ELEMENT(mat, row_index, column_index) ((mat)->elements[(row_index) * mat->columns + (column_index)])


// Create state space but don't initilize its internal arrrays
dsp_zss_t* dsp_zss_create(const size_t nx, const size_t nu, const size_t ny) {
    if (nx == 0 || nu == 0 || ny == 0) { return NULL; }

    // Allocate Space for a new StateSpace struct
    dsp_zss_t* const zss = NEW_ZSS();
    if (zss == NULL) { return NULL; }

    // Initilize
    zss->A = NULL;
    zss->B = NULL;
    zss->C = NULL;
    zss->D = NULL;
    zss->x = NULL;
    zss->xn = NULL;

    if (dsp_zss_allocate_internal_arrays(zss, nx, nu, ny)) {
        return zss;
    }
    else {
        free(zss);
        return NULL;
    }
}

// Create from arrays
dsp_zss_t* dsp_zss_create_from_arrays(const size_t nx, const size_t nu, const size_t ny, 
    const real_t* const a, const real_t* const b, const real_t* const c, const real_t* const d,
    const real_t* const x0) {
    if (a == NULL || b == NULL) { return NULL; }
    if (nx == 0 || nu == 0 || ny == 0) { return NULL; }

    // Allocate Space for a new StateSpace struct
    dsp_zss_t* const zss = NEW_ZSS();
    if (zss == NULL) { return NULL; }

    // Allocate space for internal arrays
    zss->A = dsp_matrix_create_from_array(nx, nx, a);
    zss->B = dsp_matrix_create_from_array(nx, nu, b);
    zss->C = ((c != NULL) ? dsp_matrix_create_from_array(ny, nx, c) : dsp_matrix_create_eye(ny, nx));
    zss->D = ((d != NULL) ? dsp_matrix_create_from_array(ny, nu, d) : dsp_matrix_create_zeros(ny, nu));
    zss->x = dsp_vector_create_from_array(nx, x0);
    zss->xn = dsp_vector_create_from_array(nx, x0);

    // Check
    if (zss->A == NULL || zss->B == NULL || zss->C == NULL || zss->D == NULL || zss->x == NULL || zss->xn == NULL) {
        dsp_matrix_destroy(zss->A);
        dsp_matrix_destroy(zss->B);
        dsp_matrix_destroy(zss->C);
        dsp_matrix_destroy(zss->D);
        dsp_vector_destroy(zss->x);
        dsp_vector_destroy(zss->xn);

        free(zss);
        return NULL;
    }
    else {
        return zss;
    }
}

// Create from matrices
dsp_zss_t* dsp_zss_create_from_matrices(const dsp_matrix_t* const A, const dsp_matrix_t* const B, const dsp_matrix_t* const C, const dsp_matrix_t* const D, const real_t* const x0) {
    if (A == NULL || B == NULL) { return NULL; }
    if (A->rows == 0 || B->columns == 0 || (C != NULL && C->rows == 0)) { return NULL; }
    if (A->rows != A->columns) { return NULL; }
    if (B->rows != A->rows) { return NULL; }
    if (C != NULL && C->columns != A->columns) { return NULL; }
    if (D != NULL && D->rows != (C != NULL ? C->rows : A->rows)) { return NULL; }
    if (D != NULL && D->columns != B->columns) { return NULL; }

    // Allocate Space for a new StateSpace struct
    dsp_zss_t* const zss = NEW_ZSS();
    if (zss == NULL) { return NULL; }

    // Allocate space for internal arrays
    zss->A = dsp_matrix_create_copy(A);
    zss->B = dsp_matrix_create_copy(B);
    zss->C = ((C != NULL) ? dsp_matrix_create_copy(C) : dsp_matrix_create_eye(A->rows, A->columns));
    zss->D = ((D != NULL) ? dsp_matrix_create_copy(D) : ((C != NULL) ? dsp_matrix_create_zeros(C->rows, B->columns) : dsp_matrix_create_zeros(A->rows, B->columns)));
    zss->x = dsp_vector_create_from_array(A->rows, x0);
    zss->xn = dsp_vector_create_from_array(A->rows, x0);

    // Check
    if (zss->A == NULL || zss->B == NULL || \
        zss->C == NULL || zss->D == NULL || \
        zss->x == NULL || zss->xn == NULL) {

        dsp_matrix_destroy(zss->A);
        dsp_matrix_destroy(zss->B);
        dsp_matrix_destroy(zss->C);
        dsp_matrix_destroy(zss->D);
        dsp_vector_destroy(zss->x);
        dsp_vector_destroy(zss->xn);

        free(zss);
        return NULL;
    }
    else {
        return zss;
    }
}

// Create from transfer function G(Z) = num(z^-1) / den(z^-1) = (b0 + b1 * z^-1 + b2 * z^-2 + ... bn * z^-n) / (a0 + a1 * z^-1 + a2 * z^-2 + ... an * z^-n)
dsp_zss_t* dsp_zss_create_from_tf(const size_t order, const real_t* const num, const real_t* const den, const real_t* const x0) {
    if (num == NULL || den ==  NULL) { return NULL; }

    // Allocate Space for a new StateSpace struct
    dsp_zss_t* zss = dsp_zss_create(order, 1, 1);
    if (zss == NULL) { return NULL; }

    // Init
    dsp_matrix_set_to_zero(zss->A);
    dsp_matrix_set_to_zero(zss->B);
    dsp_matrix_set_to_zero(zss->C);
    dsp_matrix_set_to_zero(zss->D);

    // Init
    if (x0 == NULL) { dsp_vector_set_to_zero(zss->xn); }
    else { dsp_vector_copy_assign_array(zss->xn, x0); }

    // Initilize
    MATRIX_ELEMENT(zss->B, 0, 0) = 1;
    MATRIX_ELEMENT(zss->D, 0, 0) = ARRAY_ELEMEMT(num, 0) / ARRAY_ELEMEMT(den, 0);
    for (size_t k = 0; k < order; ++k) {
        // Init A
        MATRIX_ELEMENT(zss->A, 0, k) = (-1) * ARRAY_ELEMEMT(den, k+1) / ARRAY_ELEMEMT(den, 0);
        if (k < order - 1) { MATRIX_ELEMENT(zss->A, k+1, k) = 1; }

        // Init C
        MATRIX_ELEMENT(zss->C, 0, k) = ARRAY_ELEMEMT(num, k+1) / ARRAY_ELEMEMT(den, 0) + MATRIX_ELEMENT(zss->D, 0, 0) * MATRIX_ELEMENT(zss->A, 0, k);
    }
    return zss;
}

// Create from polynomials G(Z) = num(z^-1) / den(z^-1) = (b0 + b1 * z^-1 + b2 * z^-2 + ... bn * z^-n) / (a0 + a1 * z^-1 + a2 * z^-2 + ... an * z^-n)
dsp_zss_t* dsp_zss_create_from_poly(const dsp_poly_t* const num, const dsp_poly_t* const den, const real_t* const x0) {
    if (num == NULL || den ==  NULL) { return NULL; }
    if (num->order != den->order) { return NULL; }
    return dsp_zss_create_from_tf(den->order, num->a, den->a, x0);
}

// Create from zTransferFuntion object
dsp_zss_t* dsp_zss_create_from_ztf(const dsp_ztf_t* const ztf, const real_t* const x0) {
    if (ztf == NULL) { return NULL; }
    return dsp_zss_create_from_tf(ztf->order, ztf->b, ztf->a, x0);
}

// Create a PT1 system
dsp_zss_t* dsp_zss_create_pt1(const real_t K, const real_t T, const real_t Ts, const real_t x0) {

    const real_t a = expf(-Ts / T);
    const real_t b = K * (1 - a);

    const real_t num[] = {0, b};
    const real_t den[] = {1, -a};
    return dsp_zss_create_from_tf(1, num, den, &x0);
}

// Create a discrete-time lead-lag compensator
dsp_zss_t* dsp_zss_create_lead_lag_compensator(const real_t T1, const real_t T2, const real_t Ts, const real_t x0) {

    const real_t num[] = {T1, (Ts - T1)};
    const real_t den[] = {T2, (Ts - T2)};
    return dsp_zss_create_from_tf(1, num, den, &x0);
}

// Create a discrete-time low-pass filter
dsp_zss_t* dsp_zss_create_lowpass_filter(const real_t K, const real_t T, const real_t Ts, const real_t x0) {

    const real_t num[] = {0, (K * Ts) / T};
    const real_t den[] = {1, (Ts / T - 1)};
    return dsp_zss_create_from_tf(1, num, den, &x0);
}

// Create a Discrete-time washout or high-pass filter
dsp_zss_t* dsp_zss_create_highpass_filter(const real_t K, const real_t T, const real_t Ts, const real_t x0) {

    const real_t num[] = {K, -K};
    const real_t den[] = {1, (Ts / T - 1)};
    return dsp_zss_create_from_tf(1, num, den, &x0);
}

// Create a Discrete time integrator
dsp_zss_t* dsp_zss_create_integrator(const real_t K, const real_t Ts, const s_approximation_t s_approx , const real_t x0) {

    switch (s_approx) {
        case ForwardEuler: {
            const real_t num[] = {0, K * Ts};
            const real_t den[] = {1, -1};
            return dsp_zss_create_from_tf(1, num, den, &x0);
        }
        case BackwardEuler: {
            const real_t num[] = {K * Ts, 0};
            const real_t den[] = {1, -1};
            return dsp_zss_create_from_tf(1, num, den, &x0);
        }
        case Trapezoidal: {
            const real_t num[] = {K * Ts, K * Ts};
            const real_t den[] = {2, -2};
            return dsp_zss_create_from_tf(1, num, den, &x0);
        }
        default: {
            const real_t num[] = {0, K * Ts};
            const real_t den[] = {1, -1};
            return dsp_zss_create_from_tf(1, num, den, &x0);
        }
    }
}

// Create a Discrete time filtered derivative
dsp_zss_t* dsp_zss_create_derivative(const real_t K, const real_t T, const real_t Ts, const s_approximation_t s_approx, const real_t x0) {

    switch (s_approx) {
        case ForwardEuler: {
            const real_t num[] = {K, -K};
            const real_t den[] = {T, (Ts - T)};
            return dsp_zss_create_from_tf(1, num, den, &x0);
        }
        case BackwardEuler: {
            const real_t num[] = {K, -K};
            const real_t den[] = {(T+Ts), -T};
            return dsp_zss_create_from_tf(1, num, den, &x0);
        }
        case Trapezoidal: {
            const real_t num[] = {(2 * K), -(2 * K)};
            const real_t den[] = {(Ts + 2 * T), (Ts - 2 * T)};
            return dsp_zss_create_from_tf(1, num, den, &x0);
        }
        default: {
            const real_t num[] = {K, -K};
            const real_t den[] = {T, (Ts - T)};
            return dsp_zss_create_from_tf(1, num, den, &x0);
        }
    }
}


// Copy
dsp_zss_t* dsp_zss_create_copy(const dsp_zss_t* const other) {
    if (other == NULL) { return NULL; }
    return dsp_zss_create_from_matrices(other->A, other->B, other->C, other->D, other->xn->elements);
}
bool dsp_zss_copy_assign(dsp_zss_t* const dest, const dsp_zss_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    if (!dsp_matrix_has_same_size(dest->A, src->A)) { return false; }
    if (!dsp_matrix_has_same_size(dest->B, src->B)) { return false; }
    if (!dsp_matrix_has_same_size(dest->C, src->C)) { return false; }
    if (!dsp_matrix_has_same_size(dest->D, src->D)) { return false; }
    if (!dsp_vector_has_same_size(dest->x, src->x)) { return false; }
    if (!dsp_vector_has_same_size(dest->xn, src->xn)) { return false; }

    // Copy
    dsp_matrix_copy_assign(dest->A, src->A);
    dsp_matrix_copy_assign(dest->B, src->B);
    dsp_matrix_copy_assign(dest->C, src->C);
    dsp_matrix_copy_assign(dest->D, src->D);
    dsp_vector_copy_assign(dest->x, src->x);
    dsp_vector_copy_assign(dest->xn, src->xn);
    return true;
}

// Move
dsp_zss_t* dsp_zss_create_move(dsp_zss_t* const other) {
    if (other == NULL) { return NULL; }

    // Allocate Space for a new StateSpace struct
    dsp_zss_t* const zss = NEW_ZSS();
    if (zss == NULL) { return NULL; }

    // Move elements array pointer
    zss->A = other->A;
    zss->B = other->B;
    zss->C = other->C;
    zss->D = other->D;
    zss->x = other->x;
    zss->xn = other->xn;

    // Invalidate other elements array pointer
    other->A = NULL;
    other->B = NULL;
    other->C = NULL;
    other->D = NULL;
    other->x = NULL;
    other->xn = NULL;

    return zss;
}
bool dsp_zss_move_assign(dsp_zss_t* const dest, dsp_zss_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    dsp_zss_release_internal_arrays(dest);

    // Move elements array pointer
    dest->A = src->A;
    dest->B = src->B;
    dest->C = src->C;
    dest->D = src->D;
    dest->x = src->x;
    dest->xn = src->xn;

    // Invalidate other elements array pointer
    src->A = NULL;
    src->B = NULL;
    src->C = NULL;
    src->D = NULL;
    src->x = NULL;
    src->xn = NULL;

    return true;
}

// Swap
bool dsp_zss_swap(dsp_zss_t* const a, dsp_zss_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    if (a == b) { return false; }
    dsp_zss_t temp = *a;
    *a = *b;
    *b = temp;
    return true;
}

// Destroy
bool dsp_zss_allocate_internal_arrays(dsp_zss_t* const zss, const size_t nx, const size_t nu, const size_t ny) {
    if (zss == NULL) { return false; }
    if(zss->A != NULL) { return false; }
    if(zss->B != NULL) { return false; }
    if(zss->C != NULL) { return false; }
    if(zss->D != NULL) { return false; }
    if(zss->x != NULL) { return false; }
    if(zss->xn != NULL) { return false; }
    if (nx == 0 || nu == 0 || ny == 0) { return false; }

    // Allocate
    zss->A = dsp_matrix_create(nx, nx);
    zss->B = dsp_matrix_create(nx, nu);
    zss->C = dsp_matrix_create(ny, nx);
    zss->D = dsp_matrix_create(ny, nu);
    zss->x = dsp_vector_create(nx);
    zss->xn = dsp_vector_create(nx);

    if (zss->A == NULL || zss->B == NULL || \
        zss->C == NULL || zss->D == NULL || \
        zss->x == NULL || zss->xn == NULL) {

        dsp_matrix_destroy(zss->A);
        dsp_matrix_destroy(zss->B);
        dsp_matrix_destroy(zss->C);
        dsp_matrix_destroy(zss->D);
        dsp_vector_destroy(zss->x);
        dsp_vector_destroy(zss->xn);
        return false;
    }
    else {
        return true;
    }
}
bool dsp_zss_release_internal_arrays(dsp_zss_t* const zss) {
    if (zss == NULL) { return false; }

    dsp_matrix_destroy(zss->A);
    dsp_matrix_destroy(zss->B);
    dsp_matrix_destroy(zss->C);
    dsp_matrix_destroy(zss->D);
    dsp_vector_destroy(zss->x);
    dsp_vector_destroy(zss->xn);

    zss->A = NULL;
    zss->B = NULL;
    zss->C = NULL;
    zss->D = NULL;
    zss->x = NULL;
    zss->xn = NULL;
    return true;
}
bool dsp_zss_destroy(dsp_zss_t* const zss) {
    if (zss == NULL) { return NULL; }
    dsp_zss_release_internal_arrays(zss);
    free(zss);
    return true;
}


// Set state
bool dsp_zss_set_state_to(dsp_zss_t* const zss, const dsp_vector_t* const x0) {
    if (zss == NULL) { return false; }
    if (x0 != NULL) {
        return \
            dsp_vector_copy_assign(zss->xn, x0) && \
            dsp_vector_copy_assign(zss->x, x0);
    }
    else { 
        return dsp_zss_reset(zss);
    }
}
bool dsp_zss_set_state(dsp_zss_t* const zss, const real_t* const x0) {
    if (zss == NULL) { return false; }
    if (x0 != NULL) {
        return \
            dsp_vector_copy_assign_array(zss->xn, x0) && \
            dsp_vector_copy_assign_array(zss->x, x0);
    }
    else { 
        return dsp_zss_reset(zss);
    }
}

bool dsp_zss_set_initial_conditions_to(dsp_zss_t* const zss, const dsp_vector_t* const u0, const dsp_vector_t* const y0) {
    if (zss == NULL || u0 == NULL || y0 == NULL) { return false; }
    // x0 = inv(C) * (y0 - D * u0)

    dsp_matrix_t* const inv_C = dsp_matrix_create_pinv(zss->C);
    dsp_vector_t* const acc = dsp_vector_create_copy(y0);
    if (inv_C == NULL && acc == NULL) {
        dsp_vector_destroy(acc); // NULL safe
        dsp_matrix_destroy(inv_C); // NULL safe
        return false;
    }

    // acc = y0 - D * u0
    dsp_matrix_vector_multiply_and_subtract_from_vector(acc, zss->D, u0);

    // x0 = inv(c) * acc
    dsp_matrix_vector_multiply(zss->x, inv_C, acc);
    dsp_vector_copy_assign(zss->xn, zss->x);

    dsp_vector_destroy(acc);
    dsp_matrix_destroy(inv_C);
    return true;
}
bool dsp_zss_set_initial_conditions(dsp_zss_t* const zss, const real_t* const u0, const real_t* const y0) {
    if (zss == NULL || u0 == NULL || y0 == NULL) { return false; }
    // dsp_vector_t U0 = { .elements = (real_t*) u0, .size = zss->B->columns };
    //dsp_vector_t Y0 = { .elements = (real_t*) y0, .size = zss->C->rows };
    dsp_vector_t U0 = { zss->B->columns, (real_t*) u0 };
    dsp_vector_t Y0 = { zss->C->rows, (real_t*) y0 };
    return dsp_zss_set_initial_conditions_to(zss, &U0, &Y0);
}


// Reset state
bool dsp_zss_reset(dsp_zss_t* const zss) {
    if (zss == NULL) { return false; }
    return \
        dsp_vector_set_to_zero(zss->xn) && \
        dsp_vector_set_to_zero(zss->x);
}


bool dsp_zss_has_feedthrough(const dsp_zss_t* const zss) {
    if (zss == NULL) { return false; }
    
    // For all elements of the D-Matrix
    for (size_t k = 0; k < (zss->D->rows * zss->D->columns); ++k) {

        // Check all elements until we find a non-zero element
        if (zss->D->elements[k] != 0) { return true; }
    }

    // If we didn't find a non-zero element, the state space has no feedthrough
    return false;
}

// Output: y[k] = C * x[k] + D * u[k]
bool dsp_zss_vector_get_output(dsp_zss_t* const zss, const dsp_vector_t* const u, dsp_vector_t* const y) {
    if (zss == NULL || u == NULL || y == NULL) { return false; }

    // y[n] = C * x[n]
    dsp_matrix_vector_multiply(y, zss->C, zss->x);

    // y[n] += D * u[n]
    dsp_matrix_vector_multiply_and_add_to_vector(y, zss->D, u);

    return true;
}

// Update state: x[k+1] = A * x[k] + B * u[k]
bool dsp_zss_vector_update_state(dsp_zss_t* const zss, const dsp_vector_t* const u) {
    if (zss == NULL || u == NULL) { return false; }

    // x[n+1] = A * x[n]
    dsp_matrix_vector_multiply(zss->xn, zss->A, zss->x);

    // x[n+1] += B * u[n]
    dsp_matrix_vector_multiply_and_add_to_vector(zss->xn, zss->B, u);

    // swap 
    dsp_vector_swap(zss->x, zss->xn);

    // x now contains the updated state
    return true;
}


// Get output and update state
// Output: y[k] = C * x[k] + D * u[k]
// Update state: x[k+1] = A * x[k] + B * u[k]
bool dsp_zss_vector_update(dsp_zss_t* const zss, const dsp_vector_t* const u, dsp_vector_t* const y) {
    return dsp_zss_vector_get_output(zss, u, y) && dsp_zss_vector_update_state(zss, u);
}


// Get Output
bool dsp_zss_get_output(dsp_zss_t* const zss, const real_t* const u, real_t* const y) {
    if (zss == NULL || u == NULL || y == NULL) { return false; }

    // const dsp_vector_t u_vec = { .size = zss->B->columns, .elements =  (real_t* const) u }; // u will not be modified
    // dsp_vector_t y_vec = { .size = zss->C->rows, .elements = y };
    const dsp_vector_t u_vec = { zss->B->columns, (real_t* const) u }; // u will not be modified
    dsp_vector_t y_vec = { zss->C->rows, y };
    return dsp_zss_vector_get_output(zss, &u_vec, &y_vec);
}

// Update state
bool dsp_zss_update_state(dsp_zss_t* const zss, const real_t* const u) {
    if (zss == NULL || u == NULL) { return false; }

    // const dsp_vector_t u_vec = { .size = zss->B->columns, .elements =  (real_t* const) u }; // u will not be modified
	const dsp_vector_t u_vec = { zss->B->columns, (real_t* const) u }; // u will not be modified
    return dsp_zss_vector_update_state(zss, &u_vec);
}

// Get output and update state
bool dsp_zss_update(dsp_zss_t* const zss, const real_t* const u, real_t* const y) {
    if (zss == NULL || u == NULL || y == NULL) { return false; }

    // const dsp_vector_t u_vec = { .size = zss->B->columns, .elements = (real_t* const) u };
    // dsp_vector_t y_vec = { .size = zss->C->rows, .elements = y };
    const dsp_vector_t u_vec = { zss->B->columns, (real_t* const) u };
    dsp_vector_t y_vec = { zss->C->rows, y };
    return dsp_zss_vector_update(zss, &u_vec, &y_vec);
}
