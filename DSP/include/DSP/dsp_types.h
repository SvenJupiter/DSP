#ifndef SJ_DSP_TYPES_H
#define SJ_DSP_TYPES_H

#include <stdint.h> // size_t
#include <stdbool.h> // bool

#ifdef BUILD_SHARDED_DSP_LIB
// #pragma message("Building shared libDSP")
#define DSP_FUNCTION __declspec(dllexport)
#endif
#ifdef USE_SHARDED_DSP_LIB
// #pragma message("Using shared libDSP")
#define DSP_FUNCTION __declspec(dllimport)
#endif

#ifndef DSP_FUNCTION
#define DSP_FUNCTION
#endif


// ----- Types -----


// 32-bit floating point number
typedef float real_t;


// ----- Enums -----

// Discrete approximation for continuous 's'
typedef enum sApproximation {
    ForwardEuler,  // s = (z - 1) / Ts
    BackwardEuler, // s = (z - 1) / (z * Ts)
    Trapezoidal // s = (2 / Ts) * (z - 1) / (Z + 1)
} s_approximation_t;

// PID Anti-Windup Method
typedef enum AntiWindupMethod {
    None = 0,
    Clamping = 1,
    BackCalculation = 2
} dsp_anti_windup_method_t;

// PID Tracking Mode
typedef enum TrackingMode {
    Disabled = 0,       // fromTR = 0
    Direct = 1,         // fromTR = Kt * (tr - postSat) (Matlab)
    Delayed = 2,        // fromTR = Kt * (tr - Output)
    Feedthrough = 3,    // fromTR = Kt * tr
} dsp_tracking_mode_t;

typedef enum RoundingMethod {
    RoundMath = 0,
    RoundDown,
    RoundUp
} rounding_method_t;


#endif // SJ_DSP_TYPES_H