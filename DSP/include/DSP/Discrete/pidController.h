#ifndef SJ_PID_CONTROLLER_H
#define SJ_PID_CONTROLLER_H

#include "DSP/dsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief PID-Controller with advanced features 
 *        like output saturation, output rate limitation, anti-windup and tracking.
 *        It uses a combination of clamping and back-calculation as anti-windup method.
 * 
 */
typedef struct pidController {

    // ----- Parameter -----

    // Sample Time
    real_t Ts;

    // PID Gains
    real_t Kp;
    real_t Ki;
    real_t Kd;
    real_t N;

    // Output Saturation
    bool limit_output;
    real_t upper_limit;
    real_t lower_limit;

    // Output Rate Limitation
    bool limit_rate;
    real_t rising_slew_rate;
    real_t falling_slew_rate;

    // Anti-Windup
    bool anti_windup_enabled;
    real_t Kb;

    // Tracking
    bool tracking_enabled;
    real_t Kt;



    // ----- Internal -----

    // Integrator (Forward Euler)
    // Xi[n+1] = Xi[n] + Ts * u[n]
    // y[n] = Xi[n]
    // real_t Ai; // 1
    // real_t Bi; // Ts
    // real_t Ci; // 1
    // real_t Di; // 0
    real_t Xi; // Integrator State

    // Derivative (Forward Euler)
    // Xd[n+1] = (1 - N * Ts) * Xd[n] + N * Ts * Kd * e[n]
    // y[n] = N * (Kd * e[n] - Xd[n])
    real_t Ad; // 1 - N * Ts
    real_t Bd; // Kd * N * Ts
    // real_t Cd; // (-N)
    // real_t Dd; // N
    real_t Xd; // Derivative Filter State

    // Rate Limiter
    real_t rising_step_size; // R+ * Ts
    real_t falling_step_size; // R- * Ts
    real_t Xr; // Rate Limiter State

    // PID States
    real_t input;
    real_t pidsum;
    real_t output;

} dsp_pid_t;





/**
 * @brief Allocate a new pidController struct.
 * 
 * @note The optained pointer musst be freed with the funtion 'dsp_pid_destroy()'.
 * 
 * @return Pointer to a new pidController struct.
 */
DSP_FUNCTION dsp_pid_t* dsp_pid_create();

/**
 * @brief Allocate a new pidController struct and 
 *        configure its parameter and internal constants.
 * 
 * @note The optained pointer musst be freed with the funtion 'dsp_pid_destroy()'.
 * 
 * @param Ts Sample Time
 * 
 * @param Kp Proportional (P) gain
 * 
 * @param Ki Integral (I) gain
 * 
 * @param Kd Derivative (D) gain
 * 
 * @param N Filter coefficient (N) (N * Ts < 1)
 * 
 * @return  Pointer to the new pidController struct.
 */
DSP_FUNCTION dsp_pid_t* dsp_pid_create_controller(const real_t Ts,
    const real_t Kp, const real_t Ki, const real_t Kd, const real_t N
);

/**
 * @brief Allocate a new pidController struct and 
 *        configure its parameter and internal constants.
 * 
 * @note The optained pointer musst be freed with the funtion 'dsp_pid_destroy()'.
 * 
 * @param Ts Sample Time
 * 
 * @param Kp Proportional (P) gain
 * 
 * @param Ki Integral (I) gain
 * 
 * @param Kd Derivative (D) gain
 * 
 * @param N Filter coefficient (N) (N * Ts < 1)
 * 
 * @param limit_output Enable/Disable pid output saturation
 * 
 * @param upper_limit Upper limit for pid output
 * 
 * @param lower_limit Lower limit for pid output
 * 
 * @param limit_rate Enable/Disable pid output rate limitation
 * 
 * @param rising_slew_rate Rising slew rate
 * 
 * @param falling_slew_rate Falling slew rate
 * 
 * @param anti_windup_enabled Enable/Disable anti-windup
 * 
 * @param Kb  Back-calculation coefficient (Kb)
 * 
 * @param tracking_enabled Enable/Disable tracking
 * 
 * @param Kt Tracking coefficient (Kt)
 * 
 * @return  Pointer to the new pidController struct.
 */
DSP_FUNCTION dsp_pid_t* dsp_pid_create_and_configure(const real_t Ts,
    const real_t Kp, const real_t Ki, const real_t Kd, const real_t N,
    const bool limit_output, const real_t upper_limit, const real_t lower_limit,
    const bool limit_rate, const real_t rising_slew_rate, const real_t falling_slew_rate,
    const bool anti_windup_enabled, const real_t Kb,
    const bool tracking_enabled, const real_t Kt
);

// Copy
DSP_FUNCTION dsp_pid_t* dsp_pid_create_copy(const dsp_pid_t* const other);
DSP_FUNCTION bool dsp_pid_copy_assign(dsp_pid_t* const dest, const dsp_pid_t* const src);

// Move
DSP_FUNCTION dsp_pid_t* dsp_pid_create_move(dsp_pid_t* const other);
DSP_FUNCTION bool dsp_pid_move_assign(dsp_pid_t* const dest, dsp_pid_t* const src);

// Swap
DSP_FUNCTION bool dsp_pid_swap(dsp_pid_t* const a, dsp_pid_t* const b);

// Destroy
DSP_FUNCTION bool dsp_pid_destroy(dsp_pid_t* const pid);

/**
 * @brief Configure the Parameters and internal constants 
 *        of an already existing pidController struct.
 * 
 * @param pid Pointer to a pidController struct
 * 
 * @param Ts Sample Time
 * 
 * @param Kp Proportional (P) gain
 * 
 * @param Ki Integral (I) gain
 * 
 * @param Kd Derivative (D) gain
 * 
 * @param N Filter coefficient (N) (N * Ts < 1)
 * 
 * @param limit_output Enable/Disable pid output saturation
 * 
 * @param upper_limit Upper limit for pid output
 * 
 * @param lower_limit Lower limit for pid output
 * 
 * @param limit_rate Enable/Disable pid output rate limitation
 * 
 * @param rising_slew_rate Rising slew rate
 * 
 * @param falling_slew_rate Falling slew rate
 * 
 * @param anti_windup_enabled Enable/Disable anti-windup
 * 
 * @param Kb  Back-calculation coefficient (Kb)
 * 
 * @param tracking_enabled Enable/Disable tracking
 * 
 * @param Kt Tracking coefficient (Kt)
 * 
 * @return 'true' if successfull and 'false' if parameters are invalid
 */
DSP_FUNCTION bool dsp_pid_configure(dsp_pid_t* const pid, const real_t Ts,
    const real_t Kp, const real_t Ki, const real_t Kd, const real_t N,
    const bool limit_output, const real_t upper_limit, const real_t lower_limit,
    const bool limit_rate, const real_t rising_slew_rate, const real_t falling_slew_rate,
    const bool anti_windup_enabled, const real_t Kb,
    const bool tracking_enabled, const real_t Kt
);

// Set Parameter
DSP_FUNCTION bool dsp_pid_set_sample_time(dsp_pid_t* const pid, const real_t Ts);
DSP_FUNCTION bool dsp_pid_set_gains(dsp_pid_t* const pid, const real_t Kp, const real_t Ki, const real_t Kd, const real_t N);
DSP_FUNCTION bool dsp_pid_set_output_saturation(dsp_pid_t* const pid, const bool limit_output, const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION bool dsp_pid_set_output_rate_limitation(dsp_pid_t* const pid, const bool limit_rate, const real_t rising_slew_rate, const real_t falling_slew_rate);
DSP_FUNCTION bool dsp_pid_set_anti_windup(dsp_pid_t* const pid, const bool anti_windup_enabled, const real_t Kb);
DSP_FUNCTION bool dsp_pid_set_tracking(dsp_pid_t* const pid, const bool tracking_enabled, const real_t Kt);

// Set State
DSP_FUNCTION bool dsp_pid_set_initial_state(dsp_pid_t* const pid, const real_t Xi, const real_t Xd, const real_t Xr);
DSP_FUNCTION bool dsp_pid_set_initial_output(dsp_pid_t* const pid, const real_t Y0);
DSP_FUNCTION bool dsp_pid_reset(dsp_pid_t* const pid);



/**
 * @brief Calculate the new PID output.
 * 
 * @note Must be called periodically with a period of 'Ts' seconds.
 *       Must be called before 'dsp_pid_update_state()'.
 *       Can't be used in combination with 'dsp_pid_update()'.
 *      
 *      // Create the PID controller
 *      dsp_pid_t* const pid = dsp_pid_create_and_configure(...);
 * 
 *       while (true) {
 * 
 *          // Get a new input from the ADC.
 *          u = adc_get_new_input();
 * 
 *          // Calculate the new PID output.
 *          y = dsp_pid_get_output(pid, u);
 * 
 *          // Do something with 'y'.
 *          modified_y = some_function(y, ...)
 * 
 *          // Send 'y to the DAC.
 *          dac_set_next_output(modified_y);
 * 
 *          // Finally update the state of the PID.
 *          // Feed back the modified 'y' to use the 'Tracking' feature.
 *          dsp_pid_update_state(pid, modified_y);
 * 
 *          // Pause the loop for 'Ts' seconds.
 *          sleep(Ts);
 *      }
 * 
 * 
 * @param pid Pointer to a pidController struct
 * 
 * @param u New PID input
 * 
 * @return New PID output
 */
DSP_FUNCTION real_t dsp_pid_get_output(dsp_pid_t* const pid, const real_t u);

/**
 * @brief Update the internal state of the PID controller.
 * 
 * @note Must be called periodically with a period of 'Ts' seconds.
 *       Must be called after 'dsp_pid_get_output()'.
 *       Can't be used in combination with 'dsp_pid_update()'.
 *
 *      // Create the PID controller
 *      dsp_pid_t* const pid = dsp_pid_create_and_configure(...);
 * 
 *       while (true) {
 * 
 *          // Get a new input from the ADC.
 *          u = adc_get_new_input();
 * 
 *          // Calculate the new PID output.
 *          y = dsp_pid_get_output(pid, u);
 * 
 *          // Do something with 'y'.
 *          modified_y = some_function(y, ...)
 * 
 *          // Send 'y to the DAC.
 *          dac_set_next_output(modified_y);
 * 
 *          // Finally update the state of the PID.
 *          // Feed back the modified 'y' to use the 'Tracking' feature.
 *          dsp_pid_update_state(pid, modified_y);
 * 
 *          // Pause the loop for 'Ts' seconds.
 *          sleep(Ts);
 *      }
 * 
 * 
 * @param pid Pointer to a pidController struct
 * 
 * @param y Modified PID output
 * 
 * @return 'true' if successfull and 'false' if parameters are invalid 
 */
DSP_FUNCTION bool dsp_pid_update_state(dsp_pid_t* const pid, const real_t y);  



/**
 * @brief Calculate the new PID output and update the internal state of the PID controller.
 * 
 * @note Must be called periodically with a period of 'Ts' seconds.
 *       Can't be used in combination with 'dsp_pid_get_output()' and dsp_pid_update_state()'.
 *      
 *      // Create the PID controller
 *      dsp_pid_t* const pid = dsp_pid_create_and_configure(...);
 * 
 *       while (true) {
 * 
 *          // Get a new input from the ADC.
 *          u = adc_get_new_input();
 * 
 *          // Calculate the new PID output.
 *          y = dsp_pid_update(pid, u);
 * 
 *          // Send 'y to the DAC.
 *          dac_set_next_output(y);
 * 
 *          // Pause the loop for 'Ts' seconds.
 *          sleep(Ts);
 *      }
 * 
 * 
 * @param pid Pointer to a pidController struct
 * 
 * @param New PID input
 * 
 * @return New PID output 
 */
DSP_FUNCTION real_t dsp_pid_update(dsp_pid_t* const pid, const real_t u);  




#ifdef __cplusplus
}
#endif


#endif // SJ_PID_CONTROLLER_H