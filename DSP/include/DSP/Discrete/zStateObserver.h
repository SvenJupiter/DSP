#ifndef SJ_Z_STATE_OBSERVER_H
#define SJ_Z_STATE_OBSERVER_H

#include "DSP/dsp_types.h"
#include "DSP/Discrete/zStateSpace.h"

#ifdef __cplusplus
extern "C" {
#endif


// Discrete-time Luenberger observer
// y^[n] = C * x^[n] + D * u[n]
// e[k] = y[k] - y^[k]
// x^[n+1] = A * x^[n] + B * u[n] + L * e[k]
typedef struct zStateObserver {

    dsp_matrix_t* A; // System matrix
    dsp_matrix_t* B; // Input matrix
    dsp_matrix_t* C; // Output matrix
    dsp_matrix_t* D; // Feedthrough matrix
    dsp_matrix_t* L; // Observer gain matrix

    dsp_vector_t* xh; // Estimated state vector

    // Internal
    dsp_vector_t* xn; // Estimated state vector 
    dsp_vector_t* yh; // Estimated output
    dsp_vector_t* e; // Estimation error

} dsp_zso_t;

// Create state observer but don't initilize its internal arrrays
DSP_FUNCTION dsp_zso_t* dsp_zso_create(const size_t nx,const size_t nu, const size_t ny);

// Create from arrays
DSP_FUNCTION dsp_zso_t* dsp_zso_create_from_arrays(const size_t nx, const size_t nu, const size_t ny,
    const real_t* const a, const real_t* const b,
    const real_t* const c, const real_t* const d,
    const real_t* const l, const real_t* const x0
);

// Create from matrices
DSP_FUNCTION dsp_zso_t* dsp_zso_create_from_matrices(
    const dsp_matrix_t* const A, const dsp_matrix_t* const B,
    const dsp_matrix_t* const C, const dsp_matrix_t* const D,
    const dsp_matrix_t* const L, const real_t* const x0
);

// Create from states space object
DSP_FUNCTION dsp_zso_t* dsp_zso_create_from_zss(const dsp_zss_t* const zss, const real_t* const l, const real_t* const x0);


// Copy
DSP_FUNCTION dsp_zso_t* dsp_zso_create_copy(const dsp_zso_t* const other);
DSP_FUNCTION bool dsp_zso_copy_assign(dsp_zso_t* const dest, const dsp_zso_t* const src);

// Move
DSP_FUNCTION dsp_zso_t* dsp_zso_create_move(dsp_zso_t* const other);
DSP_FUNCTION bool dsp_zso_move_assign(dsp_zso_t* const dest, dsp_zso_t* const src);

// Swap
DSP_FUNCTION bool dsp_zso_swap(dsp_zso_t* const a, dsp_zso_t* const b);

// Destroy
DSP_FUNCTION bool dsp_zso_allocate_internal_arrays(dsp_zso_t* const zso, const size_t nx, const size_t nu, const size_t ny);
DSP_FUNCTION bool dsp_zso_release_internal_arrays(dsp_zso_t* const zso);
DSP_FUNCTION bool dsp_zso_destroy(dsp_zso_t* const zso);

// Set state
DSP_FUNCTION bool dsp_zso_set_state_to(dsp_zso_t* const zso, const dsp_vector_t* const x0);
DSP_FUNCTION bool dsp_zso_set_state(dsp_zso_t* const zso, const real_t* const x0);

// Reset state
DSP_FUNCTION bool dsp_zso_reset(dsp_zso_t* const zso);

// Update
DSP_FUNCTION bool dsp_zso_vector_get_estimated_state(dsp_zso_t* const zso, dsp_vector_t* const xh);
DSP_FUNCTION bool dsp_zso_vector_update_estimated_state(dsp_zso_t* const zso, const dsp_vector_t* const u, const dsp_vector_t* const y);
DSP_FUNCTION bool dsp_zso_vector_update(dsp_zso_t* const zso, const dsp_vector_t* const u, const dsp_vector_t* const y, dsp_vector_t* const xh);

// Update
DSP_FUNCTION bool dsp_zso_get_estimated_state(dsp_zso_t* const zso, real_t* const xh);
DSP_FUNCTION bool dsp_zso_update_estimated_state(dsp_zso_t* const zso, const real_t* const u, const real_t* const y);
DSP_FUNCTION bool dsp_zso_update(dsp_zso_t* const zso, const real_t* const u, const real_t* const y, real_t* const xh);


#ifdef __cplusplus
}
#endif


#endif // SJ_Z_STATE_OBSERVER_H