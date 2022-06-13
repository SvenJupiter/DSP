#ifndef SJ_DISCRETE_TIME_DERIVATIVE
#define SJ_DISCRETE_TIME_DERIVATIVE

#include "DSP/dsp_types.h"


#ifdef __cplusplus
extern "C" {
#endif


typedef struct DiscreteTimeDerivative
{
    // real_t K; // Gain
    // real_t Ts; // Sample Time
    // real_t N; // Filter Coefficient
    // s_approximation_t DF; // Filter Method

    // Output saturation
    bool limit_output;
    real_t upper_limit;
    real_t lower_limit;

    // Internal
    real_t A;
    real_t B;
    // real_t C; == 1
    real_t D;
    real_t x;

} dsp_derivative_t;


DSP_FUNCTION dsp_derivative_t* dsp_derivative_create(const real_t K, const real_t N, const real_t Ts, const s_approximation_t DF, const bool limit_output, const real_t upper_limit, const real_t lower_limit);

DSP_FUNCTION dsp_derivative_t* dsp_derivative_create_copy(const dsp_derivative_t* const other);
DSP_FUNCTION bool dsp_derivative_copy_assign(dsp_derivative_t* const dest, const dsp_derivative_t* const src);

DSP_FUNCTION dsp_derivative_t* dsp_derivative_create_move(dsp_derivative_t* const other);
DSP_FUNCTION bool dsp_derivative_move_assign(dsp_derivative_t* const dest, dsp_derivative_t* const src);

DSP_FUNCTION bool dsp_derivative_swap(dsp_derivative_t* const a, dsp_derivative_t* const b);
DSP_FUNCTION bool dsp_derivative_destroy(dsp_derivative_t* const derivative);

DSP_FUNCTION bool dsp_derivative_configure(dsp_derivative_t* const derivative, const real_t K, const real_t N, const real_t Ts, const s_approximation_t DF, const bool limit_output, const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION bool dsp_derivative_set_parameters(dsp_derivative_t* const derivative, const real_t K, const real_t N, const real_t Ts, const s_approximation_t DF);
DSP_FUNCTION bool dsp_derivative_set_output_saturation(dsp_derivative_t* const derivative, const bool limit_output, const real_t upper_limit, const real_t lower_limit);


DSP_FUNCTION real_t dsp_derivative_get_state(dsp_derivative_t* const derivative);
DSP_FUNCTION bool dsp_derivative_set_state(dsp_derivative_t* const derivative, const real_t x0);
DSP_FUNCTION bool dsp_derivative_reset(dsp_derivative_t* const derivative);

DSP_FUNCTION real_t dsp_derivative_get_output(dsp_derivative_t* const derivative, const real_t u);
DSP_FUNCTION real_t dsp_derivative_update_state(dsp_derivative_t* const derivative, const real_t u);
DSP_FUNCTION real_t dsp_derivative_update(dsp_derivative_t* const derivative, const real_t u);


#ifdef __cplusplus
}
#endif


#endif // SJ_DISCRETE_TIME_DERIVATIVE