
/*
 * Include Files
 *
 */
#if defined(MATLAB_MEX_FILE)
#include "tmwtypes.h"
#include "simstruc_types.h"
#else
#include "rtwtypes.h"
#endif



/* %%%-SFUNWIZ_wrapper_includes_Changes_BEGIN --- EDIT HERE TO _END */
#include "DSP/Discrete/pidController.h"
/* %%%-SFUNWIZ_wrapper_includes_Changes_END --- EDIT HERE TO _BEGIN */
#define u_width 1
#define y_width 1

/*
 * Create external references here.  
 *
 */
/* %%%-SFUNWIZ_wrapper_externs_Changes_BEGIN --- EDIT HERE TO _END */
dsp_pid_t* pid = NULL;
/* %%%-SFUNWIZ_wrapper_externs_Changes_END --- EDIT HERE TO _BEGIN */

/*
 * Start function
 *
 */
void dspc_pid_Start_wrapper(const real32_T *Ts, const int_T p_width0,
			const real32_T *Kp, const int_T p_width1,
			const real32_T *Ki, const int_T p_width2,
			const real32_T *Kd, const int_T p_width3,
			const real32_T *N, const int_T p_width4,
			const boolean_T *limit_output, const int_T p_width5,
			const real32_T *upper_limit, const int_T p_width6,
			const real32_T *lower_limit, const int_T p_width7,
			const boolean_T *limit_rate, const int_T p_width8,
			const real32_T *rising_slew_rate, const int_T p_width9,
			const real32_T *falling_slew_rate, const int_T p_width10,
			const boolean_T *anti_windup_enabled, const int_T p_width11,
			const real32_T *Kb, const int_T p_width12,
			const boolean_T *tracking_enabled, const int_T p_width13,
			const real32_T *Kt, const int_T p_width14)
{
/* %%%-SFUNWIZ_wrapper_Start_Changes_BEGIN --- EDIT HERE TO _END */
pid = dsp_pid_create_and_configure(*Ts, 
    *Kp, *Ki, *Kd, *N, 
    *limit_output, *upper_limit, *lower_limit,
    *limit_rate, *rising_slew_rate, *falling_slew_rate,
    *anti_windup_enabled, *Kb, 
    *tracking_enabled, *Kt
);
/* %%%-SFUNWIZ_wrapper_Start_Changes_END --- EDIT HERE TO _BEGIN */
}
/*
 * Output function
 *
 */
void dspc_pid_Outputs_wrapper(const real32_T *u,
			real32_T *y,
			real32_T *pidsum,
			real32_T *Xi,
			const real32_T *Ts, const int_T p_width0,
			const real32_T *Kp, const int_T p_width1,
			const real32_T *Ki, const int_T p_width2,
			const real32_T *Kd, const int_T p_width3,
			const real32_T *N, const int_T p_width4,
			const boolean_T *limit_output, const int_T p_width5,
			const real32_T *upper_limit, const int_T p_width6,
			const real32_T *lower_limit, const int_T p_width7,
			const boolean_T *limit_rate, const int_T p_width8,
			const real32_T *rising_slew_rate, const int_T p_width9,
			const real32_T *falling_slew_rate, const int_T p_width10,
			const boolean_T *anti_windup_enabled, const int_T p_width11,
			const real32_T *Kb, const int_T p_width12,
			const boolean_T *tracking_enabled, const int_T p_width13,
			const real32_T *Kt, const int_T p_width14)
{
/* %%%-SFUNWIZ_wrapper_Outputs_Changes_BEGIN --- EDIT HERE TO _END */
*Xi = pid->Xi;
*y = dsp_pid_update(pid, *u);
*pidsum = pid->pidsum;
/* %%%-SFUNWIZ_wrapper_Outputs_Changes_END --- EDIT HERE TO _BEGIN */
}

/*
 * Terminate function
 *
 */
void dspc_pid_Terminate_wrapper(const real32_T *Ts, const int_T p_width0,
			const real32_T *Kp, const int_T p_width1,
			const real32_T *Ki, const int_T p_width2,
			const real32_T *Kd, const int_T p_width3,
			const real32_T *N, const int_T p_width4,
			const boolean_T *limit_output, const int_T p_width5,
			const real32_T *upper_limit, const int_T p_width6,
			const real32_T *lower_limit, const int_T p_width7,
			const boolean_T *limit_rate, const int_T p_width8,
			const real32_T *rising_slew_rate, const int_T p_width9,
			const real32_T *falling_slew_rate, const int_T p_width10,
			const boolean_T *anti_windup_enabled, const int_T p_width11,
			const real32_T *Kb, const int_T p_width12,
			const boolean_T *tracking_enabled, const int_T p_width13,
			const real32_T *Kt, const int_T p_width14)
{
/* %%%-SFUNWIZ_wrapper_Terminate_Changes_BEGIN --- EDIT HERE TO _END */
dsp_pid_destroy(pid);
/* %%%-SFUNWIZ_wrapper_Terminate_Changes_END --- EDIT HERE TO _BEGIN */
}

