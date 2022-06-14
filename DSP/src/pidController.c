#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset
#include "DSP/Discrete/pidController.h"


#define PID_SIZE sizeof(dsp_pid_t)
#define NEW_PID() ((dsp_pid_t*) malloc(PID_SIZE))


// Create
dsp_pid_t* dsp_pid_create() {

    // Allocate a new pidController struct
    dsp_pid_t* const pid = NEW_PID();
    if (pid == NULL) { return NULL; }

    // Set all of its content to zero
    memset(pid, 0, PID_SIZE);
    return pid;

}

// Create
dsp_pid_t* dsp_pid_create_controller(const real_t Ts,
    const real_t Kp, const real_t Ki, const real_t Kd, const real_t N) {
    return dsp_pid_create_and_configure(Ts, Kp, Ki, Kd, N, false, 1, 0, false, 1, -1, false, 1, false, 1);
}

// Create
dsp_pid_t* dsp_pid_create_and_configure(const real_t Ts,
    const real_t Kp, const real_t Ki, const real_t Kd, const real_t N,
    const bool limit_output, const real_t upper_limit, const real_t lower_limit,
    const bool limit_rate, const real_t rising_slew_rate, const real_t falling_slew_rate,
    const bool anti_windup_enabled, const real_t Kb,
    const bool tracking_enabled, const real_t Kt) {

    if (Ts <= 0.0f) { return false; }
    if (N <= 0.0f) { return false; }
    if (Ts * N >= 1.0f) { return false; }
    if (limit_output && upper_limit <= lower_limit) { return false; }
    if (limit_rate && rising_slew_rate <= 0.0f) { return false; }
    if (limit_rate && falling_slew_rate >= 0.0f) { return false; }

    // Allocate a new pidController struct
    dsp_pid_t* const pid = dsp_pid_create();
    if (pid == NULL) { return NULL; }


    if (dsp_pid_configure(pid, Ts, Kp, Ki, Kd, N, 
        limit_output, upper_limit, lower_limit, 
        limit_rate, rising_slew_rate, falling_slew_rate, 
        anti_windup_enabled, Kb, tracking_enabled, Kt)) {

        // Configuration successfull
        return pid;
    }
    else {
        // Configuration failed
        dsp_pid_destroy(pid);
        return NULL;
    }
}

// Copy
dsp_pid_t* dsp_pid_create_copy(const dsp_pid_t* const other) {
    if (other == NULL) { return NULL; }

    // Allocate a new pidController struct
    dsp_pid_t* const pid = NEW_PID();
    if (pid == NULL) { return NULL; }

    // Copy 'other' pidController struct into 'pid' pidController struct
    // *pid = *other;
    memcpy(pid, other, PID_SIZE);
    return pid;
}

bool dsp_pid_copy_assign(dsp_pid_t* const dest, const dsp_pid_t* const src) {
    if (dest == NULL) { return false; }
    if (src == NULL) { return false; }
    if (dest == src) { return false; }

    // Copy 'other' pidController struct into 'dest' pidController struct
    // *dest = *src;
    memcpy(dest, src, PID_SIZE);
    return true;
}

// Move
dsp_pid_t* dsp_pid_create_move(dsp_pid_t* const other) {
    if (other == NULL) { return NULL; }

    // Allocate a new pidController struct
    dsp_pid_t* const pid = NEW_PID();
    if (pid == NULL) { return NULL; }

    // Copy 'other' pidController struct into 'pid' pidController struct
    // *pid = *other;
    memcpy(pid, other, PID_SIZE);

    // other = {};
    memset(other , 0, PID_SIZE);
    return pid;
}
bool dsp_pid_move_assign(dsp_pid_t* const dest, dsp_pid_t* const src) {
    if (dest == NULL) { return false; }
    if (src == NULL) { return false; }
    if (dest == src) { return false; }

    // Copy 'src' pidController struct into 'dest' pidController struct
    // *dest = *src;
    memcpy(dest, src, PID_SIZE);

    // src = {};
    memset(dest , 0, PID_SIZE);
    return true;
}

// Swap
bool dsp_pid_swap(dsp_pid_t* const a, dsp_pid_t* const b) {
    if (a == NULL) { return false; }
    if (b == NULL) { return false; }
    if (a == b) { return false; }

    // dsp_pid_t temp = *a;
    // *a = *b;
    // * b = temp;

    dsp_pid_t temp;
    memcpy(&temp, a, PID_SIZE);
    memcpy(a, b, PID_SIZE);
    memcpy(b, &temp, PID_SIZE);
    return true;
}

// Destroy
bool dsp_pid_destroy(dsp_pid_t* const pid) {
    if (pid == NULL) { return false; }

    free(pid);
    return true;
}



// Configure
bool dsp_pid_configure(dsp_pid_t* const pid, const real_t Ts,
    const real_t Kp, const real_t Ki, const real_t Kd, const real_t N,
    const bool limit_output, const real_t upper_limit, const real_t lower_limit,
    const bool limit_rate, const real_t rising_slew_rate, const real_t falling_slew_rate,
    const bool anti_windup_enabled, const real_t Kb,
    const bool tracking_enabled, const real_t Kt) {

    if (pid == NULL) { return false; }
    if (Ts <= 0.0f) { return false; }
    if (N <= 0.0f) { return false; }
    if (Ts * N >= 1.0f) { return false; }
    if (limit_output && upper_limit <= lower_limit) { return false; }
    if (limit_rate && rising_slew_rate <= 0.0f) { return false; }
    if (limit_rate && falling_slew_rate >= 0.0f) { return false; }

    dsp_pid_set_sample_time(pid, Ts);
    dsp_pid_set_gains(pid, Kp, Ki, Kd, N);
    dsp_pid_set_output_saturation(pid, limit_output, upper_limit, lower_limit);
    dsp_pid_set_output_rate_limitation(pid, limit_rate, rising_slew_rate, falling_slew_rate);
    dsp_pid_set_anti_windup(pid, anti_windup_enabled, Kb);
    dsp_pid_set_tracking(pid, tracking_enabled, Kt);
    dsp_pid_set_initial_state(pid, 0.0f, 0.0f, 0.0f);
    return true;
}




// Set Parameter
bool dsp_pid_set_sample_time(dsp_pid_t* const pid, const real_t Ts) {
    if (pid == NULL) { return false; }
    if (Ts <= 0.0f) { return false; }

    // Set new sample time
    pid->Ts = Ts;

    // Update internal constants
    pid->Ad = 1.0f - pid->N * pid->Ts; // 1 - N * Ts
    pid->Bd = pid->Kd * pid->N * pid->Ts; // Kd * N * Ts
    pid->rising_step_size = pid->rising_slew_rate * pid->Ts; // R+ * Ts
    pid->falling_step_size = (-1.0f) * pid->falling_slew_rate * pid->Ts; // R- * Ts
    return true;
}

bool dsp_pid_set_gains(dsp_pid_t* const pid, const real_t Kp, const real_t Ki, const real_t Kd, const real_t N) {
    if (pid == NULL) { return false; }
    if (pid->Ts <= 0.0f) { return false; }
    if (pid->Ts * N >= 1.0f) { return false; }

    // Set new gains
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->N = N;

    // Update internal constants
    pid->Ad = 1.0f - pid->N * pid->Ts; // 1 - N * Ts
    pid->Bd = pid->Kd * pid->N * pid->Ts; // Kd * N * Ts
    return true;
}

bool dsp_pid_set_output_saturation(dsp_pid_t* const pid, const bool limit_output, const real_t upper_limit, const real_t lower_limit) {
    if (pid == NULL) { return false; }
    if (pid->Ts <= 0.0f) { return false; }
    if (limit_output && upper_limit <= lower_limit) { return false; }

    // Set new limits
    pid->limit_output = limit_output;
    pid->upper_limit = upper_limit;
    pid->lower_limit = lower_limit;
    return true;
}

bool dsp_pid_set_output_rate_limitation(dsp_pid_t* const pid, const bool limit_rate, const real_t rising_slew_rate, const real_t falling_slew_rate) {
    if (pid == NULL) { return false; }
    if (pid->Ts <= 0.0f) { return false; }
    if (limit_rate && rising_slew_rate <= 0.0f) { return false; }
    if (limit_rate && falling_slew_rate >= 0.0f) { return false; }

    // Set new slew rates
    pid->limit_rate = limit_rate;
    pid->rising_slew_rate = rising_slew_rate;
    pid->falling_slew_rate = falling_slew_rate;

    // Update internal constants
    pid->rising_step_size = pid->rising_slew_rate * pid->Ts; // R+ * Ts
    pid->falling_step_size = pid->falling_slew_rate * pid->Ts; // R- * Ts
    return true;
}

bool dsp_pid_set_anti_windup(dsp_pid_t* const pid, const bool anti_windup_enabled, const real_t Kb) {
    if (pid == NULL) { return false; }
    if (pid->Ts <= 0.0f) { return false; }

    // Set new gain
    pid->anti_windup_enabled = anti_windup_enabled;
    pid->Kb = Kb;
    return true;
}

bool dsp_pid_set_tracking(dsp_pid_t* const pid, const bool tracking_enabled, const real_t Kt) {
    if (pid == NULL) { return false; }
    if (pid->Ts <= 0.0f) { return false; }

    // Set new gain
    pid->tracking_enabled = tracking_enabled;
    pid->Kt = Kt;
    return true;
}



// Set State
bool dsp_pid_set_initial_state(dsp_pid_t* const pid, const real_t Xi, const real_t Xd, const real_t Xr) {
    if (pid == NULL) { return false; }

    // Set new states
    pid->Xi = Xi;
    pid->Xd = Xd;
    pid->Xr = Xr;

    pid->input = 0.0f;
    pid->pidsum = Xi;
    pid->output = Xr;

    return true;
}

bool dsp_pid_set_initial_output(dsp_pid_t* const pid, const real_t Y0) {
    return dsp_pid_set_initial_state(pid, Y0, 0.0f, Y0);
}

bool dsp_pid_reset(dsp_pid_t* const pid) {
    return dsp_pid_set_initial_state(pid, 0.0f, 0.0f, 0.0f);
}

// Doesn't update internal states
real_t dsp_pid_get_output(dsp_pid_t* const pid, const real_t u) {
    if (pid == NULL) { return 0.0f; }

    // Calculate PID output (P + I + D)
    pid->input = u;
    pid->pidsum = (pid->Kp * pid->input) + pid->Xi + (pid->N * (pid->Kd * pid->input - pid->Xd));
    pid->output = pid->pidsum;

    // Saturation
    if (pid->limit_output) {
        if (pid->pidsum > pid->upper_limit)      { pid->output = pid->upper_limit; }
        else if (pid->pidsum < pid->lower_limit) { pid->output = pid->lower_limit; }
    }

    // Rate Limitation
    if (pid->limit_rate) {
        const real_t change = pid->output - pid->Xr;
        if (change > pid->rising_step_size) { pid->output = pid->Xr + pid->rising_step_size; }
        if (change < pid->falling_step_size) { pid->output = pid->Xr + pid->falling_step_size; }
    }

    // Return
    return pid->output;
}

// Update internal state
bool dsp_pid_update_state(dsp_pid_t* const pid, const real_t y) {
    if (pid == NULL) { return false; }

    // Integrator input
    real_t toInt = pid->Ki * pid->input;

    // Tracking
    if (pid->tracking_enabled) {

        // Feedback to Integrator input
        toInt += pid->Kt * (y - pid->output); 
    }

    // Anti-Windup
    if (pid->anti_windup_enabled) {

        // Back-Calculation
        toInt += pid->Kb * (pid->output - pid->pidsum); 

        // Clamping
        if (pid->limit_output) {
            if (pid->pidsum >= pid->upper_limit && toInt > 0.0f) { toInt = 0.0f; }
            else if (pid->pidsum <= pid->lower_limit && toInt < 0.0f) { toInt = 0.0f; }
        }

    }

    // Update Integrator state
    pid->Xi += pid->Ts * toInt;

    // Update Derivative Filter state
    pid->Xd = pid->Ad * pid->Xd + pid->Bd * pid->input;

    // Update Rate Limiter state
    pid->Xr = pid->output;

    return true;
}



// Calculate output and update internal state
real_t dsp_pid_update(dsp_pid_t* const pid, const real_t u) {
    if (pid == NULL) { return 0.0f; }

    // calculate new output
    dsp_pid_get_output(pid, u);

    // Update internal states
    dsp_pid_update_state(pid, 0.0f);

    // Return new output
    return pid->output;
}


