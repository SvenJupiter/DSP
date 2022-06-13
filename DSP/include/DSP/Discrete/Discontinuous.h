#ifndef SJ_DISCONTINUOUS_H
#define SJ_DISCONTINUOUS_H

#include "DSP/dsp_types.h" // real_t

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Saturation {
    real_t UpperLimit;
    real_t LowerLimit;
    real_t output;
} dsp_saturation_t;

DSP_FUNCTION dsp_saturation_t* dsp_saturation_create(const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION bool dsp_saturation_set_limits(dsp_saturation_t* const saturation, const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION real_t dsp_saturation_update(dsp_saturation_t* const saturation, const real_t new_input);
DSP_FUNCTION real_t dsp_saturation_output(dsp_saturation_t* const saturation);
DSP_FUNCTION bool dsp_saturation_destroy(dsp_saturation_t* const saturation);




typedef struct DeadZone {
    real_t UpperLimit;
    real_t LowerLimit;
    real_t output;
} dsp_dead_zone_t;

DSP_FUNCTION dsp_dead_zone_t* dsp_dead_zone_create(const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION bool dsp_dead_zone_set_limits(dsp_dead_zone_t* const dead_zone, const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION real_t dsp_dead_zone_update(dsp_dead_zone_t* const dead_zone, const real_t new_input);
DSP_FUNCTION real_t dsp_dead_zone_output(dsp_dead_zone_t* const dead_zone);
DSP_FUNCTION bool dsp_dead_zone_destroy(dsp_dead_zone_t* const dead_zone);




typedef struct RateLimiter {
    real_t UpperLimit;
    real_t LowerLimit;
    real_t Ts;
    real_t input;
    real_t output;
} dsp_rate_limiter_t;

DSP_FUNCTION dsp_rate_limiter_t* dsp_rate_limiter_create(const real_t upper_rate, const real_t lower_rate, const real_t Ts, const real_t initial_output);
DSP_FUNCTION bool dsp_rate_limiter_set_limits(dsp_rate_limiter_t* const rate_limiter, const real_t upper_rate, const real_t lower_rate, const real_t Ts);
DSP_FUNCTION bool dsp_rate_limiter_set_initial_condition(dsp_rate_limiter_t* const rate_limiter, const real_t initial_output);
DSP_FUNCTION bool dsp_rate_limiter_reset(dsp_rate_limiter_t* const rate_limiter);
DSP_FUNCTION real_t dsp_rate_limiter_update(dsp_rate_limiter_t* const rate_limiter, const real_t new_input);
DSP_FUNCTION real_t dsp_rate_limiter_get_output(dsp_rate_limiter_t* const rate_limiter);
DSP_FUNCTION bool dsp_rate_limiter_destroy(dsp_rate_limiter_t* const rate_limiter);





// y = c + q * round((u - c) / q)
typedef real_t (*round_func_t)(real_t x);
typedef struct Quantization {
    real_t offset;
    real_t interval;
    round_func_t round_func;
    real_t output;
} dsp_quantization_t;

DSP_FUNCTION dsp_quantization_t* dsp_quantizer_create(const real_t offset, const real_t interval, const rounding_method_t method);
DSP_FUNCTION dsp_quantization_t* dsp_rounding_create(const size_t precision, const rounding_method_t method);
DSP_FUNCTION bool dsp_rounding_set_precision(dsp_quantization_t* const rounder, const size_t precision, const rounding_method_t method);
DSP_FUNCTION bool dsp_quantizer_set_parameters(dsp_quantization_t* const quantizer, const real_t offset, const real_t interval, const rounding_method_t method);
DSP_FUNCTION real_t dsp_quantizer_update(dsp_quantization_t* const quantizer, const real_t new_input);
DSP_FUNCTION real_t dsp_quantizer_get_output(dsp_quantization_t* const quantizer);
DSP_FUNCTION bool dsp_quantizer_destroy(dsp_quantization_t* const quantizer);




typedef struct SchmittTrigger {

    real_t low_level_input;
    real_t high_level_input;
    real_t low_level_output;
    real_t high_level_output;
    bool inverted;
    real_t normal_output;
    real_t inverted_output;

} dsp_schmitt_trigger_t;

DSP_FUNCTION dsp_schmitt_trigger_t* dsp_schmitt_trigger_create(
    const real_t low_level_input, const real_t high_level_input,
    const real_t low_level_output, const real_t high_level_output,
    const bool inverted, const bool initially_high
);
DSP_FUNCTION bool dsp_schmitt_trigger_set_input_level(dsp_schmitt_trigger_t* const trigger, const real_t low_level_input, const real_t high_level_input);
DSP_FUNCTION bool dsp_schmitt_trigger_set_output_level(dsp_schmitt_trigger_t* const trigger, const real_t low_level_output, const real_t high_level_output);
DSP_FUNCTION bool dsp_schmitt_trigger_set_inverted(dsp_schmitt_trigger_t* const trigger, const bool inverted);
DSP_FUNCTION bool dsp_schmitt_trigger_set_output(dsp_schmitt_trigger_t* const trigger, const bool high);
DSP_FUNCTION real_t dsp_schmitt_trigger_get_output(dsp_schmitt_trigger_t* const trigger);
DSP_FUNCTION real_t dsp_schmitt_trigger_update(dsp_schmitt_trigger_t* const trigger, const real_t input_level);
DSP_FUNCTION bool dsp_schmitt_trigger_destroy(dsp_schmitt_trigger_t* const trigger);




// y = c + q * round((u - c) / q)
typedef struct SchmittQuantization {

    real_t offset;
    real_t interval;
    real_t high_level_hysteresis;
    real_t low_level_hysteresis;
    real_t output;

} dsp_schmitt_quantization_t;

DSP_FUNCTION dsp_schmitt_quantization_t* dsp_schmitt_quantizer_create(const real_t offset, const real_t interval, const real_t high_level_hysteresis, const real_t low_level_hysteresis, const real_t initial_output);
DSP_FUNCTION dsp_schmitt_quantization_t* dsp_schmitt_quantizer_create_relative(const real_t offset, const real_t interval, const real_t relative_hysteresis_level, const real_t relative_hysteresis_width, const real_t initial_output);
DSP_FUNCTION bool dsp_schmitt_quantizer_set_parameters(dsp_schmitt_quantization_t* const quantizer, const real_t offset, const real_t interval, const real_t high_level_hysteresis, const real_t low_level_hysteresis);
DSP_FUNCTION bool dsp_schmitt_quantizer_set_offset_interval(dsp_schmitt_quantization_t* const quantizer, const real_t offset, const real_t interval);
DSP_FUNCTION bool dsp_schmitt_quantizer_set_hysteresis(dsp_schmitt_quantization_t* const quantizer, const real_t high_level_hysteresis, const real_t low_level_hysteresis);
DSP_FUNCTION bool dsp_schmitt_quantizer_set_output(dsp_schmitt_quantization_t* const quantizer, const real_t initial_output);
DSP_FUNCTION real_t dsp_schmitt_quantizer_get_output(dsp_schmitt_quantization_t* const quantizer);
DSP_FUNCTION real_t dsp_schmitt_quantizer_update(dsp_schmitt_quantization_t* const quantizer, const real_t new_input);
DSP_FUNCTION bool dsp_schmitt_quantizer_destroy(dsp_schmitt_quantization_t* const quantizer);


// Helper Functions
DSP_FUNCTION real_t dsp_discontinuous_saturate(const real_t x, const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION real_t dsp_discontinuous_saturate_if(const bool enabled, const real_t x, const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION real_t dsp_discontinuous_clamp(const real_t x, const real_t y, const real_t upper_limit, const real_t lower_limit);
DSP_FUNCTION real_t dsp_discontinuous_clamp_if(const bool enabled, const real_t x, const real_t y, const real_t upper_limit, const real_t lower_limit);


#ifdef __cplusplus
}
#endif


#endif // SJ_DISCONTINUOUS_H
