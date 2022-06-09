#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <math.h> // round, ceil, floor, pow

#include "DSP/Discrete/Discontinuous.h"

// #define SIGN(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))
// #define DEADZONE(x, UL, LL) ((x) > (UL) ? ((x) - (UL)) : ((x) < (LL) ? ((x) - (LL) : (x))))
// #define SIGNZONE(x, UL, LL) ((x) > (UL) ? 1 : ((x) < (LL)) ? -1 : 0)
// #define CLIP(x, UL, LL) ((x) >= (UL) ? (UL) : ((x) <= (LL) ? (LL) : (x)))
// #define CLIPIF(x, CL, UL, LL) ((CL) ? CLIP(x, UL, LL) : (x))

real_t dsp_discontinuous_saturate(const real_t x, const real_t upper_limit, const real_t lower_limit) {
    if (x > upper_limit) { return upper_limit; }
    else if (x < lower_limit) { return lower_limit; }
    else { return x; }
}

real_t dsp_discontinuous_saturate_if(const bool enabled, const real_t x, const real_t upper_limit, const real_t lower_limit) {
    if (enabled) { return dsp_discontinuous_saturate(x, upper_limit, lower_limit); }
    else {  return x; }
}

real_t dsp_discontinuous_clamp(const real_t x, const real_t y, const real_t upper_limit, const real_t lower_limit) {
    if (y >= upper_limit && x > 0) { return 0; }
    else if (y <= lower_limit && x < 0) { return 0; }
    else { return x;}
}

real_t dsp_discontinuous_clamp_if(const bool enabled, const real_t x, const real_t y, const real_t upper_limit, const real_t lower_limit) {
    if (enabled) { return dsp_discontinuous_clamp(x, y, upper_limit, lower_limit); }
    else { return x;}
}



dsp_saturation_t* dsp_saturation_create(const real_t upper_limit, const real_t lower_limit) {
    dsp_saturation_t* const saturation = malloc(sizeof(dsp_saturation_t));
    if (saturation == NULL) { return NULL; } 

    dsp_saturation_set_limits(saturation, upper_limit, lower_limit);
    return saturation;
}

bool dsp_saturation_set_limits(dsp_saturation_t* const saturation, const real_t upper_limit, const real_t lower_limit) {
    if (saturation == NULL) { return false; } 
    saturation->UpperLimit = upper_limit;
    saturation->LowerLimit = lower_limit;
    return true;
}

real_t dsp_saturation_update(dsp_saturation_t* const saturation, const real_t new_input) {
    if (saturation == NULL) { return 0; } 

    if (new_input >= saturation->UpperLimit) { saturation->output = saturation->UpperLimit; }
    else if (new_input <= saturation->LowerLimit) { saturation->output = saturation->LowerLimit; }
    else { saturation->output = new_input; }

    return saturation->output;
}

real_t dsp_saturation_get_output(dsp_saturation_t* const saturation) {
    if (saturation == NULL) { return 0; } 

    return saturation->output;
}

bool dsp_saturation_destroy(dsp_saturation_t* const saturation) {
    if (saturation == NULL) { return false; } 
    free(saturation);
    return true;
}






dsp_dead_zone_t* dsp_dead_zone_create(const real_t upper_limit, const real_t lower_limit) {
    dsp_dead_zone_t* const dead_zone = malloc(sizeof(dsp_dead_zone_t));
    if (dead_zone == NULL) { return NULL; } 

    dsp_dead_zone_set_limits(dead_zone, upper_limit, lower_limit);
    return dead_zone;
}

bool dsp_dead_zone_set_limits(dsp_dead_zone_t* const dead_zone, const real_t upper_limit, const real_t lower_limit) {
    if (dead_zone == NULL) { return false; } 
    dead_zone->UpperLimit = upper_limit;
    dead_zone->LowerLimit = lower_limit;
    return true;
}

real_t dsp_dead_zone_update(dsp_dead_zone_t* const dead_zone, const real_t new_input) {

    if (new_input > dead_zone->UpperLimit) { dead_zone->output = new_input - dead_zone->UpperLimit; }
    else if (new_input < dead_zone->LowerLimit) { dead_zone->output = new_input - dead_zone->LowerLimit; }
    else { dead_zone->output = 0; }

    return dead_zone->output;
}

real_t dsp_dead_zone_get_output(dsp_dead_zone_t* const dead_zone) {
    if (dead_zone == NULL) { return 0; } 

    return dead_zone->output;
}

bool dsp_dead_zone_destroy(dsp_dead_zone_t* const dead_zone) {
    if (dead_zone == NULL) { return false; } 
    free(dead_zone);
    return true;
}




dsp_rate_limiter_t* dsp_rate_limiter_create(const real_t upper_rate, const real_t lower_rate, const real_t Ts, const real_t initial_output) {
    dsp_rate_limiter_t* const rate_limiter = malloc(sizeof(dsp_rate_limiter_t));
    if (rate_limiter == NULL) { return NULL; }

    dsp_rate_limiter_set_limits(rate_limiter, upper_rate, lower_rate, Ts);
    dsp_rate_limiter_set_initial_condition(rate_limiter, initial_output);
    return rate_limiter;
}

bool dsp_rate_limiter_set_limits(dsp_rate_limiter_t* const rate_limiter, const real_t upper_rate, const real_t lower_rate, const real_t Ts) {
    if (rate_limiter == NULL) { return false; }
    rate_limiter->UpperLimit = upper_rate * Ts;
    rate_limiter->LowerLimit = lower_rate * Ts;
    rate_limiter->Ts = Ts;
    return true;
}

bool dsp_rate_limiter_set_initial_condition(dsp_rate_limiter_t* const rate_limiter, const real_t initial_output) {
    if (rate_limiter == NULL) { return false; }
    rate_limiter->output = initial_output;
    return true;
}

bool dsp_rate_limiter_reset(dsp_rate_limiter_t* const rate_limiter) {
    return dsp_rate_limiter_set_initial_condition(rate_limiter, 0);
}

real_t dsp_rate_limiter_update(dsp_rate_limiter_t* const rate_limiter, const real_t new_input) {
    if (rate_limiter == NULL) { return 0; }

    rate_limiter->input = new_input - rate_limiter->output;
    if (rate_limiter->input > rate_limiter->UpperLimit) { rate_limiter->output += rate_limiter->UpperLimit; }
    else if (rate_limiter->input < rate_limiter->LowerLimit) { rate_limiter->output += rate_limiter->LowerLimit; }
    else { rate_limiter->output += rate_limiter->input; }

    return rate_limiter->output;
}

real_t dsp_rate_limiter_get_output(dsp_rate_limiter_t* const rate_limiter) {
    if (rate_limiter == NULL) { return 0; }

    return rate_limiter->output;
}

bool dsp_rate_limiter_destroy(dsp_rate_limiter_t* const rate_limiter) {
    if (rate_limiter == NULL) { return false; }
    free(rate_limiter);
    return true;
}





dsp_quantization_t* dsp_quantizer_create(const real_t offset, const real_t interval, const rounding_method_t method) {
    dsp_quantization_t* const quantizer = malloc(sizeof(dsp_quantization_t));
    if (quantizer == NULL) { return NULL; }

    dsp_quantizer_set_parameters(quantizer, offset, interval, method);
    return quantizer;
}

dsp_quantization_t* dsp_rounding_create(const size_t precision, const rounding_method_t method) {
    const real_t interval = powf(10, (-1) * ((real_t) precision));
    return dsp_quantizer_create(0, interval, method);
}

bool dsp_rounding_set_precision(dsp_quantization_t* const rounder, const size_t precision, const rounding_method_t method) {
    const real_t interval = powf(10, (-1) * ((real_t) precision));
    return dsp_quantizer_set_parameters(rounder, 0, interval, method);
}

bool dsp_quantizer_set_parameters(dsp_quantization_t* const quantizer, const real_t offset, const real_t interval, const rounding_method_t method) {
    if (quantizer == NULL) { return false; }

    quantizer->offset = offset;
    quantizer->interval = interval;
    switch (method) {
        case RoundDown:
            quantizer->round_func = floorf;
            break;
        case RoundUp:
            quantizer->round_func = ceilf;
            break;
        case RoundMath:
            quantizer->round_func = roundf;
            break;
        default:
            quantizer->round_func = roundf;
            break;
    }
    return true;
}

real_t dsp_quantizer_update(dsp_quantization_t* const quantizer, const real_t new_input) {
    if (quantizer == NULL) { return 0; }

    // y = c + q * round((u - c) / q)
    quantizer->output = quantizer->offset + quantizer->interval * quantizer->round_func((new_input - quantizer->offset) / quantizer->interval);

    return quantizer->output;
}

real_t dsp_quantizer_get_output(dsp_quantization_t* const quantizer) {
    if (quantizer == NULL) { return 0; }

    return quantizer->output;
}

bool dsp_quantizer_destroy(dsp_quantization_t* const quantizer) {
    if (quantizer == NULL) { return false; }
    free(quantizer);
    return true;
}








dsp_schmitt_trigger_t* dsp_schmitt_trigger_create (
    const real_t low_level_input, const real_t high_level_input,
    const real_t low_level_output, const real_t high_level_output,
    const bool inverted, const bool initially_high) {

    // Check
    if (low_level_input >= high_level_input) { return NULL; }
    if (low_level_output >= high_level_output) { return NULL; }

    // Allocate a new Schmitt Trigger
    dsp_schmitt_trigger_t* trigger = malloc(sizeof(dsp_schmitt_trigger_t));
    if (trigger == NULL) { return NULL; }

    // Configure Schmitt Trigger
    if (dsp_schmitt_trigger_set_input_level(trigger, low_level_input, high_level_input)) {
        if (dsp_schmitt_trigger_set_output_level(trigger, low_level_output, high_level_output)) {
            if (dsp_schmitt_trigger_set_inverted(trigger, inverted)) {
                if (dsp_schmitt_trigger_set_output(trigger, initially_high)) {

                    // Return new Schmitt Trigger
                    return trigger;
                }
            }
        }
    }

    // Configuration failed
    dsp_schmitt_trigger_destroy(trigger);
    return NULL;
}

bool dsp_schmitt_trigger_set_input_level(dsp_schmitt_trigger_t* const trigger, const real_t low_level_input, const real_t high_level_input) {
    if (trigger == NULL) { return false; }
    if (low_level_input >= high_level_input) { return false; }

    trigger->low_level_input = low_level_input;
    trigger->high_level_input = high_level_input;
    return true;
}

bool dsp_schmitt_trigger_set_output_level(dsp_schmitt_trigger_t* const trigger, const real_t low_level_output, const real_t high_level_output) {
    if (trigger == NULL) { return false; }
    if (low_level_output >= high_level_output) { return false; }

    trigger->low_level_output = low_level_output;
    trigger->high_level_output = high_level_output;
    return true; 
}

bool dsp_schmitt_trigger_set_inverted(dsp_schmitt_trigger_t* const trigger, const bool inverted) {
    if (trigger == NULL) { return false; }
    trigger->inverted = inverted;
    return true;
}

bool dsp_schmitt_trigger_set_output(dsp_schmitt_trigger_t* const trigger, const bool high) {
    if (trigger == NULL) { return false; }

    if (trigger->inverted) {
        if (high) {
            trigger->inverted_output = trigger->high_level_output;
            trigger->normal_output = trigger->low_level_output;
        }
        else {
            trigger->inverted_output = trigger->low_level_output;
            trigger->normal_output = trigger->high_level_output;         
        }
    }
    else {
        if (high) {
            trigger->normal_output = trigger->high_level_output;  
            trigger->inverted_output = trigger->low_level_output;
        }
        else {
            trigger->normal_output = trigger->low_level_output;
            trigger->inverted_output = trigger->high_level_output;
        }
    }

    return true;
}

bool dsp_schmitt_trigger_get_output(dsp_schmitt_trigger_t* const trigger) {
    if (trigger == NULL) { return 0; }
    return (trigger->inverted ? trigger->inverted_output : trigger->normal_output);
}

real_t dsp_schmitt_trigger_update(dsp_schmitt_trigger_t* const trigger, const real_t input_level) {
    if (trigger == NULL) { return 0; }

    if (input_level < trigger->low_level_input) {
        trigger->normal_output = trigger->low_level_output;
        trigger->inverted_output = trigger->high_level_output;
    }
    else if (input_level > trigger->high_level_input) {
        trigger->normal_output = trigger->high_level_output;
        trigger->inverted_output = trigger->low_level_output;
    }

    return dsp_schmitt_trigger_get_output(trigger);
}

bool dsp_schmitt_trigger_destroy(dsp_schmitt_trigger_t* const trigger) {
    if (trigger == NULL) { return false; }
    free(trigger);
    return true;
}






dsp_schmitt_quantization_t* dsp_schmitt_quantizer_create(const real_t offset, const real_t interval, const real_t high_level_hysteresis, const real_t low_level_hysteresis, const real_t initial_output) {

    // Allocate a new Schmitt Quantizer
    dsp_schmitt_quantization_t* const quantizer = malloc(sizeof(dsp_schmitt_quantization_t));
    if (quantizer == NULL) { return NULL; }

    // Configure Schmitt Quantizer
    if (dsp_schmitt_quantizer_set_parameters(quantizer, offset, interval, high_level_hysteresis, low_level_hysteresis)) {
        if (dsp_schmitt_quantizer_set_output(quantizer, initial_output)) {

            // Return new Schmitt Quantizer
            return quantizer;
        }
    }
    // Configuration failed
    dsp_schmitt_quantizer_destroy(quantizer);
    return NULL;
}

dsp_schmitt_quantization_t* dsp_schmitt_quantizer_create_relative(const real_t offset, const real_t interval, const real_t relative_hysteresis_level, const real_t relative_hysteresis_width, const real_t initial_output) {

    const real_t high_level_hysteresis = interval * (relative_hysteresis_level + relative_hysteresis_width);
    const real_t low_level_hysteresis = interval * (1 - relative_hysteresis_level + relative_hysteresis_width);
    return dsp_schmitt_quantizer_create(offset, interval, high_level_hysteresis, low_level_hysteresis, initial_output);
}

bool dsp_schmitt_quantizer_set_parameters(dsp_schmitt_quantization_t* const quantizer, const real_t offset, const real_t interval, const real_t high_level_hysteresis, const real_t low_level_hysteresis) {
    if (quantizer == NULL) { return false; }
    // if (offset <= 0.0f) { return false; }
    if (interval <= 0.0f) { return false; }
    if (high_level_hysteresis <= 0.0f) { return false; }
    if (low_level_hysteresis <= 0.0f) { return false; }

    // if (interval >= high_level_hysteresis + low_level_hysteresis) { return false; }
    // if (high_level_hysteresis + low_level_hysteresis >= 2 * interval) { return false; }

    quantizer->offset = offset;
    quantizer->interval = interval;
    quantizer->high_level_hysteresis = high_level_hysteresis;
    quantizer->low_level_hysteresis = low_level_hysteresis;
    return true;
}

bool dsp_schmitt_quantizer_set_offset_interval(dsp_schmitt_quantization_t* const quantizer, const real_t offset, const real_t interval) {
    if (quantizer == NULL) { return false; }
    // if (offset <= 0.0f) { return false; }
    if (interval <= 0.0f) { return false; }

    // if (interval >= quantizer->high_level_hysteresis + quantizer->low_level_hysteresis) { return false; }
    // if (quantizer->high_level_hysteresis + quantizer->low_level_hysteresis >= 2 * interval) { return false; }
    
    quantizer->offset = offset;
    quantizer->interval = interval;
    return true;
}

bool dsp_schmitt_quantizer_set_hysteresis(dsp_schmitt_quantization_t* const quantizer, const real_t high_level_hysteresis, const real_t low_level_hysteresis) {
    if (quantizer == NULL) { return false; }
    if (high_level_hysteresis <= 0.0f) { return false; }
    if (low_level_hysteresis <= 0.0f) { return false; }

    if (quantizer->interval >= high_level_hysteresis + low_level_hysteresis) { return false; }
    if (high_level_hysteresis + low_level_hysteresis >= 2 * quantizer->interval) { return false; }

    quantizer->high_level_hysteresis = high_level_hysteresis;
    quantizer->low_level_hysteresis = low_level_hysteresis;
    return true;
}

bool dsp_schmitt_quantizer_set_output(dsp_schmitt_quantization_t* const quantizer, const real_t initial_output) {
    if (quantizer == NULL) { return false; }
    // y = c + q * round((u - c) / q)
    quantizer->output = quantizer->offset + quantizer->interval * roundf((initial_output - quantizer->offset) / quantizer->interval);
    return true;
}

real_t dsp_schmitt_quantizer_get_output(dsp_schmitt_quantization_t* const quantizer) {
    if (quantizer == NULL) { return 0; }
    return quantizer->output;
}

real_t dsp_schmitt_quantizer_update(dsp_schmitt_quantization_t* const quantizer, const real_t new_input) {
    if (quantizer == NULL) { return 0; }

    if(new_input - quantizer->output > quantizer->high_level_hysteresis) {
        do { 
            quantizer->output += quantizer->interval;
        }
        while (new_input - quantizer->output > quantizer->high_level_hysteresis);
    }
    else if(quantizer->output - new_input > quantizer->low_level_hysteresis) {
        do { 
            quantizer->output -= quantizer->interval;
        }
        while (quantizer->output - new_input > quantizer->low_level_hysteresis);
    }

    // Avoid floation point drift error after multiple additions/subtractions
    dsp_schmitt_quantizer_set_output(quantizer, quantizer->output);

    return quantizer->output;
}

bool dsp_schmitt_quantizer_destroy(dsp_schmitt_quantization_t* const quantizer) {
    if (quantizer == NULL) { return false; }
    free(quantizer);
    return true;
}