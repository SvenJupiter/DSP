#ifndef SJ_PID_CONTROLLER_H
#define SJ_PID_CONTROLLER_H

#include "DSP/dsp_types.h"
#include "DSP/Discrete/Integrator.h"
#include "DSP/Discrete/Derivative.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pidController {

    // PID Gains
    real_t Kp;
    real_t Ki;
    real_t Kd;

    // Output Saturation
    bool limit_output;
    real_t upper_limit;
    real_t lower_limit;

    // Anti-Windup
    dsp_anti_windup_method_t anti_windup_method;
    bool back_calculation_enabled;
    bool clamping_enabled;
    real_t Kb;

    // Tracking
    dsp_tracking_mode_t tracking_mode;
    real_t Kt;

    // Internal
    dsp_integrator_t integrator;
    dsp_derivative_t derivative;

    // Internal
    real_t fromP;
    real_t fromI;
    real_t fromD;
    real_t fromTR;
    real_t fromAW;
    real_t fromState;
    real_t toInt;
    real_t preInt;
    real_t PreSat;
    real_t PostSat;
    real_t Output;

} dsp_pid_t;


// Create
DSP_FUNCTION dsp_pid_t* dsp_pid_create(const real_t Kp, const real_t Ki, const real_t Kd, const real_t N, const real_t Ts, const s_approximation_t IF, const s_approximation_t DF);

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

// Set
DSP_FUNCTION bool dsp_pid_configure(dsp_pid_t* const pid, const real_t Kp, const real_t Ki, const real_t Kd, const real_t N, const real_t Ts, const s_approximation_t IF, const s_approximation_t DF);
DSP_FUNCTION bool dsp_pid_set_gains(dsp_pid_t* const pid, const real_t Kp, const real_t Ki, const real_t Kd);
DSP_FUNCTION bool dsp_pid_set_zss(dsp_pid_t* const pid, const real_t N, const real_t Ts, const s_approximation_t IF, const s_approximation_t DF);
DSP_FUNCTION bool dsp_pid_set_initial_state(dsp_pid_t* const pid, const real_t xi0, const real_t xd0);
DSP_FUNCTION bool dsp_pid_reset(dsp_pid_t* const pid);
DSP_FUNCTION bool dsp_pid_set_output_saturation(dsp_pid_t* const pid, const bool limit_output, const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION bool dsp_pid_set_anti_windup_method(dsp_pid_t* const pid, const dsp_anti_windup_method_t anti_windup_method, const real_t Kb);
DSP_FUNCTION bool dsp_pid_set_tracking_mode(dsp_pid_t* const pid, const dsp_tracking_mode_t tracking_mode, const real_t Kt);


// Doesn't update internal states
DSP_FUNCTION real_t dsp_pid_get_output(dsp_pid_t* const pid, const real_t e, const real_t tr);

// Update internal state
DSP_FUNCTION bool dsp_pid_update_state(dsp_pid_t* const pid, const real_t e, const real_t tr);  

// Output with Anti-Windup and Tracking (Will Update internal states)
DSP_FUNCTION real_t dsp_pid_update(dsp_pid_t* const pid, const real_t e, const real_t tr);  




#ifdef __cplusplus
};
#endif


#endif // SJ_PID_CONTROLLER_HFfP