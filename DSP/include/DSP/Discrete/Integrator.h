#ifndef SJ_DISCRETE_TIME_INTEGRATOR
#define SJ_DISCRETE_TIME_INTEGRATOR

#include "DSP/dsp_types.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct DiscreteTimeIntegrator
{
    real_t K; // Gain
    real_t Ts; // Sample Time
    s_approximation_t IF; // Integrator Method

    // Output saturation
    bool limit_output;
    real_t upper_limit;
    real_t lower_limit;

    // Internal
    real_t x; // Integrator state

} dsp_integrator_t;


DSP_FUNCTION dsp_integrator_t* dsp_integrator_create(const real_t K, const real_t Ts, const s_approximation_t IF, const bool limit_output, const real_t upper_limit, const real_t lower_limit);

DSP_FUNCTION dsp_integrator_t* dsp_integrator_create_copy(const dsp_integrator_t* const other);
DSP_FUNCTION bool dsp_integrator_copy_assign(dsp_integrator_t* const dest, const dsp_integrator_t* const src);

DSP_FUNCTION dsp_integrator_t* dsp_integrator_create_move(dsp_integrator_t* const other);
DSP_FUNCTION bool dsp_integrator_move_assign(dsp_integrator_t* const dest, dsp_integrator_t* const src);

DSP_FUNCTION bool dsp_integrator_swap(dsp_integrator_t* const a, dsp_integrator_t* const b);
DSP_FUNCTION bool dsp_integrator_destroy(dsp_integrator_t* const integrator);

DSP_FUNCTION bool dsp_integrator_configure(dsp_integrator_t* const integrator, const real_t K, const real_t Ts, const s_approximation_t IF, const bool limit_output, const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION bool dsp_integrator_set_parameters(dsp_integrator_t* const integrator, const real_t K, const real_t Ts, const s_approximation_t IF);
DSP_FUNCTION bool dsp_integrator_set_output_saturation(dsp_integrator_t* const integrator, const bool limit_output, const real_t upper_limit, const real_t lower_limit);

DSP_FUNCTION real_t dsp_integrator_get_state(dsp_integrator_t* const integrator);
DSP_FUNCTION bool dsp_integrator_set_state(dsp_integrator_t* const integrator, const real_t x0);
DSP_FUNCTION bool dsp_integrator_reset(dsp_integrator_t* const integrator);

// Get Output and update state
DSP_FUNCTION real_t dsp_integrator_get_output(dsp_integrator_t* const integrator, const real_t u);
DSP_FUNCTION real_t dsp_integrator_update_state(dsp_integrator_t* const integrator, const real_t u);
DSP_FUNCTION real_t dsp_integrator_update(dsp_integrator_t* const integrator, const real_t u);


#ifdef __cplusplus
}
#endif


#endif // SJ_DISCRETE_TIME_INTEGRATOR