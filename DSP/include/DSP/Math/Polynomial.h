#ifndef SJ_POLYNOMIAL_H
#define SJ_POLYNOMIAL_H

#include "DSP/dsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Polynomial
typedef struct Polynomial {
    size_t order; // Order
    real_t* a; // Coeffs array
} dsp_poly_t;


// Create a polynomial of size 'order' but don't initilize its coeffs
DSP_FUNCTION dsp_poly_t* dsp_polynomial_create(const size_t order);

// Create a polynomial from a givan array (if 'coeffs' is NULL, set all coeffs to 0)
DSP_FUNCTION dsp_poly_t* dsp_polynomial_create_from_array(const size_t order, const real_t* const coeffs);

// Create a polynomial for a given set of points
DSP_FUNCTION dsp_poly_t* dsp_polynomial_create_fit(const size_t order, const real_t* const x, const real_t* const y, const size_t size);

// Copy
DSP_FUNCTION dsp_poly_t* dsp_polynomial_create_copy(const dsp_poly_t* const other);
DSP_FUNCTION bool dsp_polynomial_copy_assign(dsp_poly_t* const dest, const dsp_poly_t* const src);
DSP_FUNCTION bool dsp_polynomial_copy_assign_array(dsp_poly_t* const dest, const real_t* const src);

// Move
DSP_FUNCTION dsp_poly_t* dsp_polynomial_create_move(dsp_poly_t* const other);
DSP_FUNCTION bool dsp_polynomial_move_assign(dsp_poly_t* const dest, dsp_poly_t* const src);

// Swap
DSP_FUNCTION bool dsp_polynomial_swap(dsp_poly_t* const a, dsp_poly_t* const b);

// Destroy
DSP_FUNCTION bool dsp_polynomial_release_internal_array(dsp_poly_t* const p);
DSP_FUNCTION bool dsp_polynomial_destroy(dsp_poly_t* const p);

// Check same size
DSP_FUNCTION bool dsp_polynomial_has_same_size(const dsp_poly_t* const a, const dsp_poly_t* const b);

// Arithmetic
DSP_FUNCTION bool dsp_polynomial_add(dsp_poly_t* const result, const dsp_poly_t* const p1, const dsp_poly_t* const p2);
DSP_FUNCTION bool dsp_polynomial_add_and_assign(dsp_poly_t* const p1, const dsp_poly_t* const p2);
DSP_FUNCTION bool dsp_polynomial_subtract(dsp_poly_t* const result, const dsp_poly_t* const p1, const dsp_poly_t* const p2);
DSP_FUNCTION bool dsp_polynomial_subtract_and_assign(dsp_poly_t* const p1, const dsp_poly_t* const p2);


// Convolution and polynomial multiplication
// w = conv(u,v) returns the convolution of vectors u and v. 
// If u and v are vectors of polynomial coefficients, 
// convolving them is equivalent to multiplying the two polynomials.
DSP_FUNCTION bool dsp_polynomial_multiply(dsp_poly_t* const w, const dsp_poly_t* const u, const dsp_poly_t* const v);

// Deconvolution and polynomial division
// [q,r] = deconv(u,v) deconvolves a vector v out of a vector u using long division, 
// and returns the quotient q and remainder r such that u = conv(v,q) + r. 
// If u and v are vectors of polynomial coefficients, then deconvolving them is equivalent to 
// dividing the polynomial represented by u by the polynomial represented by v.
DSP_FUNCTION bool dsp_polynomial_divide(dsp_poly_t* const q, dsp_poly_t* const r, const dsp_poly_t* const u, const dsp_poly_t* const v);

// Fit
DSP_FUNCTION bool dsp_polynomial_fit(dsp_poly_t* const p, const real_t* const x, const real_t* const y, const size_t size);

// Evaluate
DSP_FUNCTION real_t dsp_polynomial_val(const dsp_poly_t* const p, const real_t x);

// Change size
DSP_FUNCTION bool dsp_polynomial_shrink_to_fit(dsp_poly_t* const p);
DSP_FUNCTION bool dsp_polynomial_grow_to(dsp_poly_t* const p, const size_t new_order);


#ifdef __cplusplus
};
#endif


#endif // SJ_POLYNOMIAL_H