#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <math.h> // round, ceil, floor, pow

#include "DSP/Discrete/Discontinuous.h"

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
    const real_t interval = powf(10, -precision);
    return dsp_quantizer_create(0, interval, method);
}

bool dsp_rounding_set_precision(dsp_quantization_t* const rounder, const size_t precision, const rounding_method_t method) {
    const real_t interval = powf(10, -precision);
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




