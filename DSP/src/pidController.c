#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset
#include "DSP/Discrete/pidController.h"
#include "DSP/Discrete/Discontinuous.h"


#define PID_SIZE sizeof(dsp_pid_t)
#define NEW_PID() malloc(PID_SIZE)



dsp_pid_t* dsp_pid_create(const real_t Kp, const real_t Ki, const real_t Kd, const real_t N, const real_t Ts, const s_approximation_t IF, const s_approximation_t DF) {
    if (N == 0 || Ts == 0) { return NULL; }

    // Create new PID
    dsp_pid_t* const pid = NEW_PID();
    if (pid == NULL) { return NULL; }
    memset(pid, 0, PID_SIZE);

    // configure PID
    if (dsp_pid_configure(pid, Kp, Ki, Kd, N, Ts, IF, DF)) {
        return pid;
    }
    else {
        // Destroy PID
        free(pid);
        return NULL;
    }
}

dsp_pid_t* dsp_pid_create_copy(const dsp_pid_t* const other) {
    if (other == NULL) { return NULL; }

    // Create new PID
    dsp_pid_t* const pid = NEW_PID();
    if (pid == NULL) { return NULL; }
    // memset(pid, 0, PID_SIZE);

    // Copy
    memcpy(pid, other, PID_SIZE);
    return pid;
}

bool dsp_pid_copy_assign(dsp_pid_t* const dest, const dsp_pid_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }

    // Copy
    memcpy(dest, src, PID_SIZE);
    return true;
}

dsp_pid_t* dsp_pid_create_move(dsp_pid_t* const other) {
    if (other == NULL) { return NULL; }

    // Create new PID
    dsp_pid_t* const pid = NEW_PID();
    if (pid == NULL) { return NULL; }
    // memset(pid, 0, PID_SIZE);

    // Move
    memcpy(pid, other, PID_SIZE);
    memset(other, 0, PID_SIZE);
    return pid;
}

bool dsp_pid_move_assign(dsp_pid_t* const dest, dsp_pid_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }

    // Copy
    memcpy(dest, src, PID_SIZE);
    memset(src, 0, PID_SIZE);
    return true;
}

bool dsp_pid_swap(dsp_pid_t* const a, dsp_pid_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    if (a == b) { return false; }

    dsp_pid_t temp = *a;
    *a = *b;
    *b = temp;
    return true;
}

bool dsp_pid_destroy(dsp_pid_t* const pid) {
    if (pid == NULL) { return false; }

    free(pid);
    return true;
}




bool dsp_pid_configure(dsp_pid_t* const pid, const real_t Kp, const real_t Ki, const real_t Kd, const real_t N, const real_t Ts, const s_approximation_t IF, const s_approximation_t DF) {
    if (pid == NULL) { return false; }
    if (N == 0 || Ts == 0) { return NULL; }
    
    dsp_pid_set_gains(pid, Kp, Ki, Kd);
    dsp_pid_set_zss(pid, N, Ts, IF, DF);
    return true;
}

bool dsp_pid_set_gains(dsp_pid_t* const pid, const real_t Kp, const real_t Ki, const real_t Kd) {
    if (pid == NULL) { return false; }
    
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    return true;
}

bool dsp_pid_set_zss(dsp_pid_t* const pid, const real_t N, const real_t Ts, const s_approximation_t IF, const s_approximation_t DF) {
    if (pid == NULL) { return false; }
    if (N == 0 || Ts == 0) { return NULL; }

    dsp_integrator_configure(&(pid->integrator), 1, Ts, IF, false, 1, -1);
    dsp_derivative_configure(&(pid->derivative), 1, N, Ts, DF, false, 1, -1);
    return true;
}

bool dsp_pid_set_initial_state(dsp_pid_t* const pid, const real_t xi0, const real_t xd0) {
    if (pid == NULL) { return false; }

    dsp_integrator_set_state(&(pid->integrator), xi0);
    dsp_derivative_set_state(&(pid->derivative), xd0);
    return true;
}

bool dsp_pid_reset(dsp_pid_t* const pid) {
    return dsp_pid_set_initial_state(pid, 0, 0);
}

bool dsp_pid_set_output_saturation(dsp_pid_t* const pid, const bool limit_output, const real_t upper_limit, const real_t lower_limit) {
    if (pid == NULL) { return false; }

    pid->limit_output = limit_output;
    pid->upper_limit = upper_limit;
    pid->lower_limit = lower_limit;

    // Update flags
    switch (pid->anti_windup_method) {

        case None:
            pid->back_calculation_enabled = false; 
            pid->clamping_enabled = false;
            break;

        case BackCalculation:
            if (pid->limit_output) {
                pid->back_calculation_enabled = true; 
                pid->clamping_enabled = false;
            }
            else {
                pid->back_calculation_enabled = false; 
                pid->clamping_enabled = false;
            }
            break;

        case Clamping:
            if (pid->limit_output) {
                pid->back_calculation_enabled = false; 
                pid->clamping_enabled = true;
            }
            else {
                pid->back_calculation_enabled = false; 
                pid->clamping_enabled = false;
            }
            break;

        default:
            pid->back_calculation_enabled = false; 
            pid->clamping_enabled = false;
            break;
    }

    return true;
}

bool dsp_pid_set_anti_windup_method(dsp_pid_t* const pid, const dsp_anti_windup_method_t anti_windup_method, const real_t Kb) {
    if (pid == NULL) { return false; }

    pid->anti_windup_method = anti_windup_method;
    switch (pid->anti_windup_method) {

        case None:
            pid->back_calculation_enabled = false; 
            pid->clamping_enabled = false;
            break;

        case BackCalculation:
            pid->Kb = Kb;
            if (pid->limit_output) {
                pid->back_calculation_enabled = true; 
                pid->clamping_enabled = false;
            }
            else {
                pid->back_calculation_enabled = false; 
                pid->clamping_enabled = false;
            }
            break;

        case Clamping:
            if (pid->limit_output) {
                pid->back_calculation_enabled = false; 
                pid->clamping_enabled = true;
            }
            else {
                pid->back_calculation_enabled = false; 
                pid->clamping_enabled = false;
            }
            break;

        default:
            pid->back_calculation_enabled = false; 
            pid->clamping_enabled = false;
            break;
    }
    return true;
}

bool dsp_pid_set_tracking_mode(dsp_pid_t* const pid, const dsp_tracking_mode_t tracking_mode, const real_t Kt) {
    if (pid == NULL) { return false; }

    pid->tracking_mode = tracking_mode;
    pid->Kt = (tracking_mode != Disabled ? Kt : 0);
    return true;
}



// Output without Anti-Windup or Tracking
real_t dsp_pid_get_output(dsp_pid_t* const pid, const real_t e, const real_t tr) { 
    if (pid == NULL) { return 0; }

    // Calculate P-Path
    const real_t fromP = pid->Kp * e;

    // Calculate D-Path
    const real_t fromD = dsp_derivative_get_output(&(pid->derivative), pid->Kd * e);

    // Predicted Output for Tracking and Anti-Windup
    const real_t PreSat = fromP + pid->fromState + fromD;
    const real_t PostSat = dsp_discontinuous_saturate_if(pid->limit_output, PreSat, pid->upper_limit, pid->lower_limit);

    // Tracking
    real_t fromTR;
    switch (pid->tracking_mode) {
        case Direct:
            fromTR = pid->Kt * (tr - PostSat);
            break;
        case Delayed:
            fromTR = pid->Kt * (tr - pid->Output);
            break;
        case Feedthrough:
            fromTR = pid->Kt * tr;
            break;
        case Disabled: // Same as default case
        default:
            fromTR = 0;
            break;
    }

    // Back Calculation
    const real_t fromAW = (pid->back_calculation_enabled ? (pid->Kb * (PostSat - PreSat)) : 0);

    // PreInt
    const real_t preInt = pid->Ki * e + fromAW + fromTR;

    // Clamping
    const real_t toInt = dsp_discontinuous_clamp_if(
        pid->clamping_enabled, preInt, 
        pid->PreSat, pid->upper_limit, pid->lower_limit
    );

    // Update I-Path
    const real_t fromI = dsp_integrator_get_output(&(pid->integrator), toInt);

    // Calculate new Output
    real_t Output = fromP + fromI + fromD;

    // Saturate new Output
    Output = dsp_discontinuous_saturate_if(pid->limit_output, Output, pid->upper_limit, pid->lower_limit);

    // Return new Output
    return Output;
}



bool dsp_pid_update_state(dsp_pid_t* const pid, const real_t e, const real_t tr) {
    if (pid == NULL) { return 0; }

    // Update P-Path
    pid->fromP = pid->Kp * e;

    // Update D-Path
    pid->fromD = dsp_derivative_update(&(pid->derivative), pid->Kd * e);

    // Predicted Output for Tracking and Anti-Windup
    pid->PreSat = pid->fromP + pid->fromState + pid->fromD;
    pid->PostSat = dsp_discontinuous_saturate_if(pid->limit_output, pid->PreSat, pid->upper_limit, pid->lower_limit);

    // Tracking
    switch (pid->tracking_mode) {
        case Direct:
            pid->fromTR = pid->Kt * (tr - pid->PostSat);
            break;
        case Delayed:
            pid->fromTR = pid->Kt * (tr - pid->Output);
            break;
        case Feedthrough:
            pid->fromTR = pid->Kt * tr;
            break;
        case Disabled: // Same as default case
        default:
            pid->fromTR = 0;
            break;
    }

    // Back Calculation
    pid->fromAW = (pid->back_calculation_enabled ? (pid->Kb * (pid->PostSat - pid->PreSat)) : 0);

    // PreInt
    pid->preInt = pid->Ki * e + pid->fromAW + pid->fromTR;

    // Clamping
    pid->toInt = dsp_discontinuous_clamp_if(
        pid->clamping_enabled, pid->preInt, 
        pid->PreSat, pid->upper_limit, pid->lower_limit
    );

    // Update I-Path
    pid->fromI = dsp_integrator_update(&(pid->integrator), pid->toInt);
    pid->fromState = dsp_integrator_get_state(&(pid->integrator));

    // Calculate new Output
    pid->Output = pid->fromP + pid->fromI + pid->fromD;

    // Saturate new Output
    pid->Output = dsp_discontinuous_saturate_if(pid->limit_output, pid->Output, pid->upper_limit, pid->lower_limit);

    return true;
}


real_t dsp_pid_update(dsp_pid_t* const pid, const real_t e, const real_t tr) {
    if (pid == NULL) { return 0; }

    // Update internal state
    dsp_pid_update_state(pid, e, tr);

    // Return new Output
    return pid->Output;
}
