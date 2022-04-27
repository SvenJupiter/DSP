#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset
#include "DSP/Discrete/pidController.h"

#define PID_SIZE sizeof(dsp_pid_t)
#define NEW_PID() malloc(PID_SIZE)

// Create
dsp_pid_t* dsp_pid_create(const real_t Kp, const real_t Ki, const real_t Kd, const real_t Ts, const s_approximation_t IF, const real_t Tf, const s_approximation_t DF) {
    if (Ts == 0) { return NULL; }
    if (Tf == 0) {return NULL; }

    // Create a new PID-Controller
    dsp_pid_t* const pid = NEW_PID();
    if (pid == NULL) { return NULL; }

    // Initilize
    memset(pid, 0, PID_SIZE);

    // Configure
    if (dsp_pid_set_gains(pid, Kp, Ki, Kd)) {
        if (dsp_pid_set_zss(pid, Ts, IF, Tf, DF)) {

            // Return PID-Controller
            return pid;
        }
    }

    // Destroy
    dsp_pid_destroy(pid);
    return NULL;
}

// Copy
dsp_pid_t* dsp_pid_create_copy(const dsp_pid_t* const other) {
    if (other == NULL) { return NULL; }

    // Create a new PID-Controller
    dsp_pid_t* const pid = NEW_PID();
    if (pid == NULL) { return NULL; }

    // Copy parameters
    memcpy(pid, other, PID_SIZE);

    // Copy Integrator
    pid->integrator = dsp_zss_create_copy(other->integrator);

    // Copy Derivative
    pid->derivative = dsp_zss_create_copy(other->derivative);

    // check
    if (pid->integrator == NULL || pid->derivative == NULL) {
        dsp_pid_destroy(pid);
        return NULL;
    }
    else {
        return pid;
    }
}
bool dsp_pid_copy_assign(dsp_pid_t* const dest, const dsp_pid_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }

    // Release internal arrays
    dsp_pid_release_internal_arrays(dest);

    // Copy parameters
    memcpy(dest, src, PID_SIZE);

    // Copy Integrator
    dest->integrator = dsp_zss_create_copy(src->integrator);

    // Copy Derivative
    dest->derivative = dsp_zss_create_copy(src->derivative);

    // check
    if (dest->integrator == NULL || dest->derivative == NULL) {
        return false;
    }
    else {
        return true;
    }
}

// Move
dsp_pid_t* dsp_pid_create_move(dsp_pid_t* const other) {
    if (other == NULL) { return NULL; }

    // Create a new PID-Controller
    dsp_pid_t* const pid = NEW_PID();
    if (pid == NULL) { return NULL; }

    // Copy parameters
    memcpy(pid, other, PID_SIZE);

    // Invalidate other parameters 
    memset(other, 0, PID_SIZE);

    return pid;
}
bool dsp_pid_move_assign(dsp_pid_t* const dest, dsp_pid_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }

    // Release internal arrays
    dsp_pid_release_internal_arrays(dest);

    // Copy parameters
    memcpy(dest, src, PID_SIZE);

    // Invalidate other parameters 
    memset(src, 0, PID_SIZE);

    return true;
}

// Swap
bool dsp_pid_swap(dsp_pid_t* const a, dsp_pid_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    if (a == b) { return false; }
    dsp_pid_t temp = *a;
    *a = *b;
    *b = temp;
    return true;
}

// Destroy
bool dsp_pid_release_internal_arrays(dsp_pid_t* const pid) {
    if (pid == NULL) { return false; }
    dsp_zss_destroy(pid->integrator);
    dsp_zss_destroy(pid->derivative);
    pid->integrator = NULL;
    pid->derivative = NULL;
    return true;
}
bool dsp_pid_destroy(dsp_pid_t* const pid) {
    if (pid == NULL) { return false; }
    dsp_pid_release_internal_arrays(pid);
    free(pid);
    return true;
}

// Set
bool dsp_pid_set_gains(dsp_pid_t* const pid, const real_t Kp, const real_t Ki, const real_t Kd) {
    if (pid == NULL) { return false; }
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    return true;
}
bool dsp_pid_set_zss(dsp_pid_t* const pid, const real_t Ts, const s_approximation_t IF, const real_t Tf, const s_approximation_t DF) {
    if (pid == NULL) { return false; }
    if (Ts == 0) { return false; }
    if (Tf == 0) {return false; }
    dsp_pid_release_internal_arrays(pid);

    pid->integrator = dsp_zss_create_integrator(1, Ts, IF, 0);
    pid->derivative = dsp_zss_create_derivative(1, Tf, Ts, DF, 0);
    return (pid->integrator != NULL && pid->derivative != NULL);
}
bool dsp_pid_set_initial_state(dsp_pid_t* const pid, const real_t* const xi0, const real_t* const xd0) {
    if (pid == NULL) { return false; }
    dsp_zss_set_state(pid->integrator, xi0);
    dsp_zss_set_state(pid->derivative, xd0);
    return true;
}
bool dsp_reset(dsp_pid_t* const pid) {
    if (pid == NULL) { return false; }
    dsp_zss_reset(pid->integrator);
    dsp_zss_reset(pid->derivative);
    return true;
}
bool dsp_pid_set_output_saturation(dsp_pid_t* const pid, const bool limit_output, const real_t upper_limit, const real_t lower_limit) {
    if (pid == NULL) { return false; }
    pid->limit_output = limit_output;
    pid->upper_limit = upper_limit;
    pid->lower_limit = lower_limit;
    return true;
}
bool dsp_pid_set_anti_windup(dsp_pid_t* const pid, const dsp_anti_windup_method_t anti_windup_method, real_t Kb) {
    if (pid == NULL) { return false; }
    pid->anti_windup_method = anti_windup_method;
    pid->Kb = Kb;
    return true;
}
bool dsp_pid_set_tracking(dsp_pid_t* const pid, const bool tracking_enabled, real_t Kt) {
    if (pid == NULL) { return false; }
    pid->tracking_enabled = tracking_enabled;
    pid->Kt = Kt;
    return true;
}


// Update
real_t dsp_pid_update(dsp_pid_t* const pid, const real_t e, const real_t tr) {
    if (pid == NULL) { return 0; }

    // P
    pid->fromP = pid->Kp * e;

    // I
    pid->fromTR = (pid->tracking_enabled ? pid->Kt * (tr - pid->postSat) : 0);
    pid->fromAW = (pid->limit_output && pid->anti_windup_method == BackCalculation ? pid->Kb * (pid->postSat - pid->preSat) : 0);
    pid->preInt = pid->Ki * e + pid->fromTR + pid->fromAW;
    if (pid->limit_output && pid->anti_windup_method == Clamping) {
        if (pid->preSat > pid->upper_limit && pid->preInt > 0) {
            pid->toInt = 0;
        }
        else if (pid->preSat < pid->lower_limit && pid->preInt < 0) {
            pid->toInt = 0;
        }
        else {
            pid->toInt = pid->preInt;
        }
    }
    else {
        pid->toInt = pid->preInt;
    }
    dsp_zss_update(pid->integrator, &(pid->toInt), &(pid->fromI));

    // D 
    pid->toDer = pid->Kd * e;
    dsp_zss_update(pid->derivative, &(pid->toDer), &(pid->fromD));

    // Saturation
    pid->preSat = pid->fromP + pid->fromI + pid->fromD;
    if (pid->limit_output) {
        if (pid->preSat > pid->upper_limit) {
            pid->postSat = pid->upper_limit;
        }
        else if (pid->preSat < pid->lower_limit) {
            pid->postSat = pid->lower_limit;
        }
        else {
            pid->postSat = pid->preSat;
        }
    }
    else {
        pid->postSat = pid->preSat;
    }

    return pid->postSat;
}