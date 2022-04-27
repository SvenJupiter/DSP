#ifndef SJ_ZTF_H
#define SJ_ZTF_H

#include "DSP/dsp_types.h" // real_t

#ifdef __cplusplus
extern "C" {
#endif



// G(z) = b(z^-1) / a(z^-1) = (b0 + b1 * z^-1 + b2 * z^-2 + ... bn * z^-n) / (a0 + a1 * z^-1 + a2 * z^-2 + ... an * z^-n)
typedef struct zTransferFunction {

    size_t order;
    real_t* a;
    real_t* b;
    real_t* u;
    real_t* y;

} dsp_ztf_t;


// Create
DSP_FUNCTION dsp_ztf_t* dsp_ztf_create(const size_t order);

/**
 * @brief Create a Z-Transfer-Function system
 * 
 * @param order Order of the system
 * 
 * @param num Array with the coefficients of the numerator (musst be 'order+1' in size)
 *            If 'num' is a NULL pointer: b0 = 1, b1,2,..,n = 0
 * 
 * @param den Array with the coefficients of the denominator (musst be 'order+1' in size) 
 *            If 'den' is a NULL pointer: a0 = 1, a1,2,..,n = 0
 * 
 * @param initial_u Array with the initial input values (musst be 'order' in size) 
 *                  If 'initial_u' is an NULL pointer: u[-1] = u[-2]... = u[-order] = 0
 * 
 * @param initial_y Array with the initial output values (musst be 'order' in size) 
 *                  If 'initial_y' is an NULL pointer: y[-1] = y[-2]... = y[-order] = 0
 * 
 * @return Pointer to the created Z-Transfer-Function system.
 *         The Pointer musst be released after use with the function 'ztf_destroy()'
 */
DSP_FUNCTION dsp_ztf_t* dsp_ztf_create_from_arrays(const size_t order, const real_t* const num, const real_t* const den, const real_t* initial_u, const real_t* initial_y);

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
 * @param initial_u Initial input
 * 
 * @param initial_y Initial output
 * 
 * @return Pointer to the created Z-Transfer-Function system.
 *         The Pointer musst be released after use with the function 'ztf_destroy()'
 */
DSP_FUNCTION dsp_ztf_t* dsp_ztf_create_PT1(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y);

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
 * @param initial_u Initial input
 * 
 * @param initial_y Initial output
 * 
 * @return Pointer to the created Z-Transfer-Function system.
 *         The Pointer musst be released after use with the function 'ztf_destroy()'
 */
DSP_FUNCTION dsp_ztf_t* dsp_ztf_create_lead_lag_compensator(const real_t T1, const real_t T2, const real_t Ts, const real_t initial_u, const real_t initial_y);

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
 * @param initial_u Initial input
 * 
 * @param initial_y Initial output
 * 
 * @return Pointer to the created Z-Transfer-Function system.
 *         The Pointer musst be released after use with the function 'ztf_destroy()'
 */
DSP_FUNCTION dsp_ztf_t* dsp_ztf_create_lowpass_filter(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y);

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
 * @param initial_u Initial input
 * 
 * @param initial_y Initial output
 * 
 * @return Pointer to the created Z-Transfer-Function system.
 *         The Pointer musst be released after use with the function 'ztf_destroy()'
 */
DSP_FUNCTION dsp_ztf_t* dsp_ztf_create_highpass_filter(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y);

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
 * @param initial_u Initial input
 * 
 * @param initial_y Initial output
 * 
 * @param IF Approximation: s = f(z)
 * 
 * @return Pointer to the created Z-Transfer-Function system.
 *         The Pointer musst be released after use with the function 'ztf_destroy()'
 */
DSP_FUNCTION dsp_ztf_t* dsp_ztf_create_integrator(const real_t K, const real_t Ts, const real_t initial_u, const real_t initial_y, const s_approximation_t IF);

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
 * @param initial_u Initial input
 * 
 * @param initial_y Initial output
 * 
 * @param DF Approximation: s = f(z)
 * 
 * @return Pointer to the created Z-Transfer-Function system.
 *         The Pointer musst be released after use with the function 'ztf_destroy()'
 */
DSP_FUNCTION dsp_ztf_t* dsp_ztf_create_derivative(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y, const s_approximation_t DF);

/**
 * @brief Release the internally allocated resources of the system
 * 
 * @param ztf A Z-Transfer-Function system 
 *            The pointer becomes invalid after the call to this function
 */
DSP_FUNCTION bool dsp_ztf_destroy(dsp_ztf_t* const ztf);




/**
 * @brief Set the coefficients of the system 
 * 
 * @param ztf A Z-Transfer-Function system 
 * 
 * @param num Array with the coefficients of the numerator (musst be 'order+1' in size)
 *            If 'num' is a NULL pointer: b0 = 1, b1,2,..,n = 0
 * 
 * @param den Array with the coefficients of the denominator (musst be 'order+1' in size) 
 *            If 'den' is a NULL pointer: a0 = 1, a1,2,..,n = 0
 */
DSP_FUNCTION bool dsp_ztf_set_coefficients(dsp_ztf_t* const ztf, const real_t* const num, const real_t* const den);

/**
 * @brief Set the initial condition of the system 
 * 
 * @param ztf A Z-Transfer-Function system 
 * 
 * @param initial_u Array with the initial input values (musst be 'order' in size) 
 *                  If 'initial_u' is an NULL pointer: u[-1] = u[-2]... = u[-order] = 0
 * 
 * @param initial_y Array with the initial output values (musst be 'order' in size) 
 *                  If 'initial_y' is an NULL pointer: y[-1] = y[-2]... = y[-order] = 0
 */
DSP_FUNCTION bool dsp_ztf_set_initial_condition(dsp_ztf_t* const ztf, const real_t* initial_u, const real_t* initial_y);

/**
 * @brief Reset the internal states of the system 
 * 
 * @param ztf A Z-Transfer-Function system 
 */
DSP_FUNCTION bool dsp_ztf_reset(dsp_ztf_t* const ztf);

/**
 * @brief Calculate the next output of the system
 * 
 * @param ztf A Z-Transfer-Function system
 * 
 * @param new_u New system input
 * 
 * @return Returns the new system output
 */
DSP_FUNCTION real_t dsp_ztf_update(dsp_ztf_t* const ztf, const real_t new_u);

/**
 * @brief Get the latest output of the system
 * 
 * @param ztf A Z-Transfer-Function system 
 * 
 * @return Returns the latest output of the system
 */
DSP_FUNCTION real_t dsp_ztf_output(dsp_ztf_t* const ztf);


#ifdef __cplusplus
};
#endif


#endif // SJ_ZTF_H