#ifndef SJ_VECTOR_H
#define SJ_VECTOR_H

#include "DSP/dsp_types.h"
#include "DSP/Math/Matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

// Vector
typedef struct Vector {
    size_t size;
    real_t* elements;
} dsp_vector_t;


// Create a vector of size 'size' but don't initilize its elements
DSP_FUNCTION dsp_vector_t* dsp_vector_create(const size_t size);

// Create a vector from a givan array (if 'elements' is NULL, set all elements to 0)
DSP_FUNCTION dsp_vector_t* dsp_vector_create_from_array(const size_t size, const real_t* const elements);

// Create
DSP_FUNCTION dsp_vector_t* dsp_vector_create_zerro_vector(const size_t size);
DSP_FUNCTION dsp_vector_t* dsp_vector_create_unit_vector(const size_t size, const size_t dim_index);
DSP_FUNCTION dsp_vector_t* dsp_vector_create_normalized_vector(const dsp_vector_t* const vec);
DSP_FUNCTION dsp_vector_t* dsp_vector_create_projection(const dsp_vector_t* const a, const dsp_vector_t* const b);
DSP_FUNCTION dsp_vector_t* dsp_vector_create_cross_product(const dsp_vector_t* const a, const dsp_vector_t* const b); // Cross product (for R^3 vectors only)
DSP_FUNCTION dsp_vector_t* dsp_vector_create_matrix_product(const dsp_matrix_t* const A, const dsp_vector_t* const x);
DSP_FUNCTION dsp_vector_t* dsp_vector_create_solution(const dsp_matrix_t* const A, const dsp_vector_t* const b);

// Concat
DSP_FUNCTION dsp_vector_t* dsp_vector_create_concat(const dsp_vector_t* const T, const dsp_vector_t* const B);

// Create a vector from a given matrix
DSP_FUNCTION dsp_vector_t* dsp_vector_create_from_matrix(const dsp_matrix_t* const mat);

// Create a vector from a matrix row
DSP_FUNCTION dsp_vector_t* dsp_matrix_create_row_vector(const dsp_matrix_t* const mat, const size_t row_index);

// Create a vector from a matrix column
DSP_FUNCTION dsp_vector_t* dsp_matrix_create_column_vector(const dsp_matrix_t* const mat, const size_t column_index);

// Copy
DSP_FUNCTION dsp_vector_t* dsp_vector_create_copy(const dsp_vector_t* const other);
DSP_FUNCTION bool dsp_vector_copy_assign(dsp_vector_t* const dest, const dsp_vector_t* const src);
DSP_FUNCTION bool dsp_vector_copy_assign_array(dsp_vector_t* const dest, const real_t* const src);

// Move
DSP_FUNCTION dsp_vector_t* dsp_vector_create_move(dsp_vector_t* const other);
DSP_FUNCTION bool dsp_vector_move_assign(dsp_vector_t* const dest, dsp_vector_t* const src);

// Swap
DSP_FUNCTION bool dsp_vector_swap(dsp_vector_t* const a, dsp_vector_t* const b);

// Destroy
DSP_FUNCTION bool dsp_vector_allocate_internal_array(dsp_vector_t* const vec, const size_t size);
DSP_FUNCTION bool dsp_vector_release_internal_array(dsp_vector_t* const vec);
DSP_FUNCTION bool dsp_vector_destroy(dsp_vector_t* const vec);

// Check same size
DSP_FUNCTION bool dsp_vector_has_same_size(dsp_vector_t* const a, dsp_vector_t* const b);

// Set
DSP_FUNCTION bool dsp_vector_set_to_zero(dsp_vector_t* const vec);
DSP_FUNCTION bool dsp_vector_set_to_unit(dsp_vector_t* const vec, const size_t dim_index);
DSP_FUNCTION bool dsp_vector_normalize(dsp_vector_t* const vec);
DSP_FUNCTION bool dsp_vector_set_to_normalized(dsp_vector_t* const vec_n, const dsp_vector_t* const vec);
DSP_FUNCTION bool dsp_vector_set_to_projection(dsp_vector_t* const vec, const dsp_vector_t* const a, const dsp_vector_t* const b);
DSP_FUNCTION bool dsp_vector_set_to_cross_product(dsp_vector_t* const vec, const dsp_vector_t* const a, const dsp_vector_t* const b); // Cross product (for R^3 vectors only)


// to real_t
DSP_FUNCTION size_t dsp_vector_size(const dsp_vector_t* const vec);
DSP_FUNCTION real_t dsp_vector_length_squared(const dsp_vector_t* const vec);
DSP_FUNCTION real_t dsp_vector_length(const dsp_vector_t* const vec);
DSP_FUNCTION real_t dsp_vector_dot_product(const dsp_vector_t* const a, const dsp_vector_t* const b);
DSP_FUNCTION real_t dsp_vector_cosphi(const dsp_vector_t* const a, const dsp_vector_t* const b);
DSP_FUNCTION real_t dsp_vector_angle(const dsp_vector_t* const a, const dsp_vector_t* const b);
DSP_FUNCTION real_t dsp_vector_project(const dsp_vector_t* const a, const dsp_vector_t* const b);


// Arithmetic
DSP_FUNCTION bool dsp_vector_add(dsp_vector_t* const result, const dsp_vector_t* const a, const dsp_vector_t* const b);
DSP_FUNCTION bool dsp_vector_add_and_assign(dsp_vector_t* const a, const dsp_vector_t* const b);
DSP_FUNCTION bool dsp_vector_subtract(dsp_vector_t* const result, const dsp_vector_t* const a, const dsp_vector_t* const b);
DSP_FUNCTION bool dsp_vector_subtract_and_assign(dsp_vector_t* const a, const dsp_vector_t* const b);


// Arithmetic
DSP_FUNCTION bool dsp_vector_multiply_by_scalar(dsp_vector_t* const result, const dsp_vector_t* const vec, const real_t scalar);
DSP_FUNCTION bool dsp_vector_multiply_by_scalar_and_assign(dsp_vector_t* const vec, const real_t scalar);
DSP_FUNCTION bool dsp_vector_divide_by_scalar(dsp_vector_t* const result, const dsp_vector_t* const vec, const real_t scalar);
DSP_FUNCTION bool dsp_vector_divide_by_scalar_and_assign(dsp_vector_t* const vec, const real_t scalar);


// Concat
DSP_FUNCTION bool dsp_vector_concat(dsp_vector_t* const result, const dsp_vector_t* const T, const dsp_vector_t* const B);

// Split
DSP_FUNCTION bool dsp_vector_split(dsp_vector_t* const T, dsp_vector_t* const B, const dsp_vector_t* const vec, const size_t split_after_element_num);


// Convolution and polynomial multiplication
// w = conv(u,v) returns the convolution of vectors u and v. 
// If u and v are vectors of polynomial coefficients, 
// convolving them is equivalent to multiplying the two polynomials.
// w->size == u->size + v->size - 1;
DSP_FUNCTION bool dsp_vector_conv(dsp_vector_t* const w, const dsp_vector_t* const u, const dsp_vector_t* const v);

// Deconvolution and polynomial division
// [q,r] = deconv(u,v) deconvolves a vector v out of a vector u using long division, 
// and returns the quotient q and remainder r such that u = conv(v,q) + r. 
// If u and v are vectors of polynomial coefficients, then deconvolving them is equivalent to 
// dividing the polynomial represented by u by the polynomial represented by v.
// q->size == (u->size - v->size + 1)
// r->size == u->size
DSP_FUNCTION bool dsp_vector_deconv(dsp_vector_t* const q, dsp_vector_t* const r, const dsp_vector_t* const u, const dsp_vector_t* const v);




// Create a matrix from a givan vector
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_from_vector(const size_t rows, const size_t columns, const dsp_vector_t* vec);

// Arithmetic
DSP_FUNCTION bool dsp_matrix_vector_multiply(dsp_vector_t* const result, const dsp_matrix_t* const mat, const dsp_vector_t* const vec);

// Arithmetic
DSP_FUNCTION bool dsp_matrix_vector_multiply_and_add_to_vector(dsp_vector_t* const acc, const dsp_matrix_t* const mat, const dsp_vector_t* const vec);
DSP_FUNCTION bool dsp_matrix_vector_multiply_and_subtract_from_vector(dsp_vector_t* const acc, const dsp_matrix_t* const mat, const dsp_vector_t* const vec);

// Arithmetic
DSP_FUNCTION bool dsp_matrix_assign_row_vector(dsp_matrix_t* const mat, const size_t row_index, const dsp_vector_t* const row_vec);
DSP_FUNCTION bool dsp_matrix_assign_column_vector(dsp_matrix_t* const mat, const size_t column_index, const dsp_vector_t* const colum_vec);
DSP_FUNCTION bool dsp_matrix_add_on_row_vector(dsp_matrix_t* const mat, const size_t row_index, const dsp_vector_t* const row_vec);
DSP_FUNCTION bool dsp_matrix_add_on_column_vector(dsp_matrix_t* const mat, const size_t column_index, const dsp_vector_t* const colum_vec);
DSP_FUNCTION bool dsp_matrix_subract_of_row_vector(dsp_matrix_t* const mat, const size_t row_index, const dsp_vector_t* const row_vec);
DSP_FUNCTION bool dsp_matrix_subract_of_column_vector(dsp_matrix_t* const mat, const size_t column_index, const dsp_vector_t* const colum_vec);


// Solve system of linear equations
// A * x = b
// x == pinv(A) * b
DSP_FUNCTION bool dsp_vector_solve_lse(dsp_vector_t* const x, const dsp_matrix_t* const A, const dsp_vector_t* const b);
DSP_FUNCTION bool dsp_solve(real_t* const x, const size_t x_size, const real_t* const A, const real_t* const b, const size_t b_size);



#ifdef __cplusplus
};
#endif


#endif // SJ_VECTOR_H