#ifndef SJ_Z_STATE_SPACE_H
#define SJ_Z_STATE_SPACE_H

#include "DSP/dsp_types.h"
#include "DSP/Math/Matrix.h"
#include "DSP/Math/Vector.h"
#include "DSP/Math/Polynomial.h"
#include "DSP/Discrete/zTransferFunction.h"

#ifdef __cplusplus
extern "C" {
#endif


// x[k+1] = A * x[k] + B * u[k]
// y[k] = C * x[k] + D * u[k]
typedef struct zStateSpace {

    dsp_matrix_t* A; // System matrix
    dsp_matrix_t* B; // Input matrix
    dsp_matrix_t* C; // Output matrix
    dsp_matrix_t* D; // Feedthrough matrix

    dsp_vector_t* x; // State vector

    // Internal
    dsp_vector_t* xn; // State vector

} dsp_zss_t;

// Create state space but don't initilize its internal arrrays
DSP_FUNCTION dsp_zss_t* dsp_zss_create(const size_t nx,const size_t nu, const size_t ny);

// Create from arrays
DSP_FUNCTION dsp_zss_t* dsp_zss_create_from_arrays(const size_t nx, const size_t nu, const size_t ny, 
    const real_t* const a, const real_t* const b, 
    const real_t* const c, const real_t* const d,
    const real_t* const x0
);

// Create from matrices
DSP_FUNCTION dsp_zss_t* dsp_zss_create_from_matrices(
    const dsp_matrix_t* const A, const dsp_matrix_t* const B, 
    const dsp_matrix_t* const C, const dsp_matrix_t* const D, 
    const real_t* const x0
);

// Create from transfer function G(z) = num(z^-1) / den(z^-1) = (b0 + b1 * z^-1 + b2 * z^-2 + ... bn * z^-n) / (a0 + a1 * z^-1 + a2 * z^-2 + ... an * z^-n)
DSP_FUNCTION dsp_zss_t* dsp_zss_create_from_tf(const size_t order, const real_t* const num, const real_t* const den, const real_t* const x0);

// Create from polynomials G(z) = num(z^-1) / den(z^-1) = (b0 + b1 * z^-1 + b2 * z^-2 + ... bn * z^-n) / (a0 + a1 * z^-1 + a2 * z^-2 + ... an * z^-n)
DSP_FUNCTION dsp_zss_t* dsp_zss_create_from_poly(const dsp_poly_t* const num, const dsp_poly_t* const den, const real_t* const x0);

// Create from zTransferFuntion object
DSP_FUNCTION dsp_zss_t* dsp_zss_create_from_ztf(const dsp_ztf_t* const ztf, const real_t* const x0);

/**
 * @brief Create a PT1 system
 * 
 * @details F(s) = K / (T * s + 1)
 *          F(z) = b / (z - a)
 *          with a = exp(-Ts / T) and b = K * (1 - a)
 * 
 * @param K Gain
 * 
 * @param T Time constant
 * 
 * @param Ts Sample time 
 * 
 * @param x0 Initial state
 * 
 * @return zStateSpace System
 */
DSP_FUNCTION dsp_zss_t* dsp_zss_create_pt1(const real_t K, const real_t T, const real_t Ts, const real_t x0);

/**
 * @brief Create a discrete-time lead-lag compensator
 * 
 * @details Continuos: F(s) = (T1 * s + 1) / (T2 * s + 1)
 *          Discrete:  F(z) = (T1 + (Ts - T1) * z^-1) / (T2 + (Ts - T2) * z^-1)
 * 
 * @param T1 Lead time constant
 * 
 * @param T2 Lag time constant
 * 
 * @param Ts Sample time 
 * 
 * @param x0 Initial state
 * 
 * @return zStateSpace System
 */
DSP_FUNCTION dsp_zss_t* dsp_zss_create_lead_lag_compensator(const real_t T1, const real_t T2, const real_t Ts, const real_t x0);

/**
 * @brief Create a discrete-time low-pass filter
 * 
 * @details Continuous: F(s) = K / (T * s + 1)
 *          Discrete:   F(z) = K * (Ts / T) * z^-1 / (1 + (Ts / T - 1) * z^-1)
 * 
 * @param K Filter gain
 * 
 * @param T Filter time constant (T = 1 / fc)
 * 
 * @param Ts Sample time 
 * 
 * @param x0 Initial state
 * 
 * @return zStateSpace System
 */
DSP_FUNCTION dsp_zss_t* dsp_zss_create_lowpass_filter(const real_t K, const real_t T, const real_t Ts, const real_t x0);

/**
 * @brief Create a Discrete-time washout or high-pass filter
 * 
 * @details Continuous: F(s) = K * T * s / (T * s + 1)
 *          Discrete:   F(z) = K * (1 - z^-1) / (1 + (Ts / T - 1) * z^-1)
 * 
 * @param K Filter gain
 * 
 * @param T Filter time constant (T = 1 / fc)
 * 
 * @param Ts Sample time 
 * 
 * @param x0 Initial state
 * 
 * @return zStateSpace System
 */
DSP_FUNCTION dsp_zss_t* dsp_zss_create_highpass_filter(const real_t K, const real_t T, const real_t Ts, const real_t x0);

/**
 * @brief Create a Discrete time integrator
 * 
 * @details Continuous: F(s) = 1 / s
 *          Discrete:   
 *              Forward Euler:  F(z) = K * Ts * z^-1 / (1 - z^1)
 *              Backward Euler: F(z) = K * Ts / (1 - z^1)
 *              Trapezoidal:    F(z) = K * (Ts *(1 + z^-1)) / (2 * (1 - z^1))
 * 
 * @param K Integrator gain
 * 
 * @param Ts Sample time 
 * 
 * @param s_approx Approximation: s = f(z)
 * 
 * @param x0 Initial state
 * 
 * @return zStateSpace System
 */
DSP_FUNCTION dsp_zss_t* dsp_zss_create_integrator(const real_t K, const real_t Ts, const s_approximation_t s_approx , const real_t x0);

/**
 * @brief Create a Discrete time filtered derivative
 * 
 * @details Continuous: F(s) = s / (T * s + 1)
 *          Discrete:   
 *              Forward Euler:  F(z) =  K/T * (1 - z^-1) / (1 + (Ts / T - 1) * z^-1)
 *              Backward Euler: F(z) = K/T * (1 - z^-1) / ((1 + Ts / T) - z^-1)
 *              Trapezoidal:    F(z) = 2*K/T * (1 - z^-1) / ((Ts / T + 2) + (Ts / T - 2) * z^-1)
 * 
 * @param K Derivative gain
 * 
 * @param T Filter time constant (T = 1 / N)
 * 
 * @param Ts Sample time 
 * 
 * @param s_approx Approximation: s = f(z)
 * 
 * @param x0 Initial state
 * 
 * @return zStateSpace System
 */
DSP_FUNCTION dsp_zss_t* dsp_zss_create_derivative(const real_t K, const real_t T, const real_t Ts, const s_approximation_t s_approx, const real_t x0);


// Copy
DSP_FUNCTION dsp_zss_t* dsp_zss_create_copy(const dsp_zss_t* const other);
DSP_FUNCTION bool dsp_zss_copy_assign(dsp_zss_t* const dest, const dsp_zss_t* const src);

// Move
DSP_FUNCTION dsp_zss_t* dsp_zss_create_move(dsp_zss_t* const other);
DSP_FUNCTION bool dsp_zss_move_assign(dsp_zss_t* const dest, dsp_zss_t* const src);

// Swap
DSP_FUNCTION bool dsp_zss_swap(dsp_zss_t* const a, dsp_zss_t* const b);

// Destroy
DSP_FUNCTION bool dsp_zss_allocate_internal_arrays(dsp_zss_t* const zss, const size_t nx, const size_t nu, const size_t ny);
DSP_FUNCTION bool dsp_zss_release_internal_arrays(dsp_zss_t* const zss);
DSP_FUNCTION bool dsp_zss_destroy(dsp_zss_t* const zss);


// Set state
DSP_FUNCTION bool dsp_zss_set_state_to(dsp_zss_t* const zss, const dsp_vector_t* const x0);
DSP_FUNCTION bool dsp_zss_set_state(dsp_zss_t* const zss, const real_t* const x0);
DSP_FUNCTION bool dsp_zss_set_initial_conditions_to(dsp_zss_t* const zss, const dsp_vector_t* const u0, const dsp_vector_t* const y0);
DSP_FUNCTION bool dsp_zss_set_initial_conditions(dsp_zss_t* const zss, const real_t* const u0, const real_t* const y0);

// Reset state
DSP_FUNCTION bool dsp_zss_reset(dsp_zss_t* const zss);

// Check if the Feedthrough matrix is non-zero
DSP_FUNCTION bool dsp_zss_has_feedthrough(const dsp_zss_t* const zss);

// Get Output and Update State
DSP_FUNCTION bool dsp_zss_vector_get_output(dsp_zss_t* const zss, const dsp_vector_t* const u, dsp_vector_t* const y);
DSP_FUNCTION bool dsp_zss_vector_update_state(dsp_zss_t* const zss, const dsp_vector_t* const u);
DSP_FUNCTION bool dsp_zss_vector_update(dsp_zss_t* const zss, const dsp_vector_t* const u, dsp_vector_t* const y);

// Get Output and Update State
DSP_FUNCTION bool dsp_zss_get_output(dsp_zss_t* const zss, const real_t* const u, real_t* const y);
DSP_FUNCTION bool dsp_zss_update_state(dsp_zss_t* const zss, const real_t* const u);
DSP_FUNCTION bool dsp_zss_update(dsp_zss_t* const zss, const real_t* const u, real_t* const y);


#ifdef __cplusplus
};
#endif


#endif // SJ_Z_STATE_SPACE_H