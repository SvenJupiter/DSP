#ifndef SJ_SIGNAL_H
#define SJ_SIGNAL_H

#include "DSP/dsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif



// Returns the scalar dot product of u and v
DSP_FUNCTION real_t dsp_dot_product(
    const real_t* const u, 
    const real_t* const v, 
    const size_t size
);


// Convolution and polynomial multiplication
// w = conv(u,v) returns the convolution of vectors u and v. 
// If u and v are vectors of polynomial coefficients, 
// convolving them is equivalent to multiplying the two polynomials.
DSP_FUNCTION size_t dsp_conv(
    const real_t* const u, const size_t u_size,
    const real_t* const v, const size_t v_size,
    real_t* const w, const size_t w_size
);



// Deconvolution and polynomial division
// [q,r] = deconv(u,v) deconvolves a vector v out of a vector u using long division, 
// and returns the quotient q and remainder r such that u = conv(v,q) + r. 
// If u and v are vectors of polynomial coefficients, then deconvolving them is equivalent to 
// dividing the polynomial represented by u by the polynomial represented by v.
DSP_FUNCTION size_t dsp_deconv(
    const real_t* const u, const size_t u_size,
    const real_t* const v, const size_t v_size,
    real_t* const q, size_t q_size,
    real_t* const r, size_t r_size
);




// Wow, it's like C++ std::vector<real_t>
typedef struct Signal {
    real_t* elements;
    size_t size;
    size_t capacity;
} dsp_signal_t;


// Create
DSP_FUNCTION dsp_signal_t* dsp_signal_create(const size_t initial_capacity);
DSP_FUNCTION dsp_signal_t dsp_signal_construct(const size_t initial_capacity);

// Copy
DSP_FUNCTION dsp_signal_t* dsp_signal_create_copy(const dsp_signal_t* const other);
DSP_FUNCTION bool dsp_signal_copy_assign(dsp_signal_t* const  dest, const dsp_signal_t* const src);

// Move
DSP_FUNCTION dsp_signal_t* dsp_signal_create_move(dsp_signal_t* const other);
DSP_FUNCTION bool dsp_signal_move_assign(dsp_signal_t* const  dest, dsp_signal_t* const src);

// Destroy
DSP_FUNCTION bool dsp_signal_destruct(dsp_signal_t* const signal);
DSP_FUNCTION bool dsp_signal_destroy(dsp_signal_t* const signal);

// dot, conv, deconv
DSP_FUNCTION real_t dsp_signal_dot_product(const dsp_signal_t* const u, const dsp_signal_t* const v);
DSP_FUNCTION size_t dsp_signal_conv(dsp_signal_t* const w, const dsp_signal_t* const u, const dsp_signal_t* const v);
DSP_FUNCTION size_t dsp_signal_deconv(dsp_signal_t* const q, dsp_signal_t* const r, const dsp_signal_t* const u, const dsp_signal_t* const v);

// ----- std::vector<real_t> functions -----

DSP_FUNCTION void dsp_signal_reserve(dsp_signal_t* const signal, const size_t new_capacity);

DSP_FUNCTION void dsp_signal_resize(dsp_signal_t* const signal, const size_t new_size, const real_t* const value_ptr);

DSP_FUNCTION void dsp_signal_shrink_to_fit(dsp_signal_t* const signal);

DSP_FUNCTION real_t* dsp_signal_at(dsp_signal_t* const signal, const size_t position);

DSP_FUNCTION real_t* dsp_signal_front(dsp_signal_t* const signal);

DSP_FUNCTION real_t* dsp_signal_back(dsp_signal_t* const signal);

DSP_FUNCTION real_t* dsp_signal_data(dsp_signal_t* const signal);

DSP_FUNCTION size_t dsp_signal_size(dsp_signal_t* const signal);

DSP_FUNCTION size_t dsp_signal_capacity(dsp_signal_t* const signal);

DSP_FUNCTION bool dsp_signal_empty(dsp_signal_t* const signal);

DSP_FUNCTION void dsp_signal_assign(dsp_signal_t* const signal, const real_t* const new_elements, const size_t new_size);

DSP_FUNCTION void dsp_signal_push_back(dsp_signal_t* const signal, const real_t* const new_element);

DSP_FUNCTION void dsp_signal_pop_back(dsp_signal_t* const signal);

DSP_FUNCTION real_t* dsp_signal_insert(dsp_signal_t* const signal, const size_t position, const real_t* const new_element);

DSP_FUNCTION void dsp_signal_erase(dsp_signal_t* const signal, const size_t position);

DSP_FUNCTION void dsp_signal_swap(dsp_signal_t* const vec_a_ptr, dsp_signal_t* const vec_b_ptr);

DSP_FUNCTION void dsp_signal_clear(dsp_signal_t* const signal);

DSP_FUNCTION real_t* dsp_signal_emplace(dsp_signal_t* const signal, const size_t position, const bool fill_zeros);

DSP_FUNCTION real_t* dsp_signal_emplace_back(dsp_signal_t* const signal, const bool fill_zeros);







#ifdef __cplusplus
}
#endif


#endif // SJ_SIGNAL_H