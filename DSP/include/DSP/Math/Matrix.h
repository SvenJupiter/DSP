#ifndef SJ_MATRIX_H
#define SJ_MATRIX_H

#include "DSP/dsp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

// Matrix
typedef struct Matrix {
    size_t rows;
    size_t columns;
    real_t* elements;
} dsp_matrix_t;


// Create a matrix of size 'rows * columns' but don't initilize its elements
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create(const size_t rows, const size_t columns);

// Create a matrix from a givan array (if 'elements' is NULL, set all elements to 0)
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_from_array(const size_t rows, const size_t columns, const real_t* elements);

// Create
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_zeros(const size_t rows, const size_t columns);
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_eye(const size_t rows, const size_t columns);
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_diag(const real_t* const elements, const size_t rows, const size_t columns);
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_vandermonde(const size_t order, const real_t* const x_values, const size_t size);

// Create
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_product(const dsp_matrix_t* A, const dsp_matrix_t* B);
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_transpose(const dsp_matrix_t* const mat);
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_inv(const dsp_matrix_t* const mat);
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_pinv(const dsp_matrix_t* const M);

// Concat
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_horzcat(const dsp_matrix_t* const L, const dsp_matrix_t* const R);
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_vertcat(const dsp_matrix_t* const T, const dsp_matrix_t* const B);
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_blockcat(const dsp_matrix_t* const TL, const dsp_matrix_t* const TR, const dsp_matrix_t* const BL, const dsp_matrix_t* const BR);

// Create
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_submatrix(const dsp_matrix_t* const mat, const size_t row_index, const size_t column_index);

// Copy
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_copy(const dsp_matrix_t* const other);
DSP_FUNCTION bool dsp_matrix_copy_assign(dsp_matrix_t* const dest, const dsp_matrix_t* const src);
DSP_FUNCTION bool dsp_matrix_copy_assign_array(dsp_matrix_t* const dest, const real_t* const src);

// Move
DSP_FUNCTION dsp_matrix_t* dsp_matrix_create_move(dsp_matrix_t* const other);
DSP_FUNCTION bool dsp_matrix_move_assign(dsp_matrix_t* const dest, dsp_matrix_t* const src);

// Swap
DSP_FUNCTION bool dsp_matrix_swap(dsp_matrix_t* const a, dsp_matrix_t* const b);

// Destroy
DSP_FUNCTION bool dsp_matrix_allocate_internal_array(dsp_matrix_t* const mat, const size_t rows, const size_t columns);
DSP_FUNCTION bool dsp_matrix_release_internal_array(dsp_matrix_t* const mat);
DSP_FUNCTION bool dsp_matrix_destroy(dsp_matrix_t* const mat);

// Check same size
DSP_FUNCTION bool dsp_matrix_has_same_size(dsp_matrix_t* const a, dsp_matrix_t* const b);

// Reshape
DSP_FUNCTION bool dsp_matrix_reshape(dsp_matrix_t* const mat, const size_t new_rows, const size_t new_columns);

// Set
DSP_FUNCTION bool dsp_matrix_set_to_zero(dsp_matrix_t* const mat);
DSP_FUNCTION bool dsp_matrix_set_to_eye(dsp_matrix_t* const mat);

// Arithmetic
DSP_FUNCTION bool dsp_matrix_add(dsp_matrix_t* result, const dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);
DSP_FUNCTION bool dsp_matrix_add_and_assign(dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);
DSP_FUNCTION bool dsp_matrix_subtract(dsp_matrix_t* result, const dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);
DSP_FUNCTION bool dsp_matrix_subtract_and_assign(dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);
DSP_FUNCTION bool dsp_matrix_multiply(dsp_matrix_t* result, const dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);

// Arithmetic
DSP_FUNCTION bool dsp_matrix_multiply_by_scalar(dsp_matrix_t* result, const dsp_matrix_t* const mat, const real_t scalar);
DSP_FUNCTION bool dsp_matrix_multiply_by_scalar_and_assign(dsp_matrix_t* mat, const real_t scalar);
DSP_FUNCTION bool dsp_matrix_divide_by_scalar(dsp_matrix_t* result, const dsp_matrix_t* const mat, const real_t scalar);
DSP_FUNCTION bool dsp_matrix_divide_by_scalar_and_assign(dsp_matrix_t* mat, const real_t scalar);

// Determinante
DSP_FUNCTION real_t dsp_matrix_det(const dsp_matrix_t* const mat);

// Transpose
DSP_FUNCTION bool dsp_matrix_transpose(dsp_matrix_t* const result, const dsp_matrix_t* const mat);

// Inverse
DSP_FUNCTION bool dsp_matrix_inv(dsp_matrix_t* const result, const dsp_matrix_t* const mat);
DSP_FUNCTION bool dsp_matrix_pinv(dsp_matrix_t* const result, const dsp_matrix_t* const mat);

// Concat
DSP_FUNCTION bool dsp_matrix_horzcat(dsp_matrix_t* const result, const dsp_matrix_t* const L, const dsp_matrix_t* const R);
DSP_FUNCTION bool dsp_matrix_vertcat(dsp_matrix_t* const result, const dsp_matrix_t* const T, const dsp_matrix_t* const B);
DSP_FUNCTION bool dsp_matrix_blockcat(dsp_matrix_t* const result, const dsp_matrix_t* const TL, const dsp_matrix_t* const TR, const dsp_matrix_t* const BL, const dsp_matrix_t* const BR);

// Split 
DSP_FUNCTION bool dsp_matrix_horzsplit(dsp_matrix_t* const L, dsp_matrix_t* const R, const dsp_matrix_t* const mat, const size_t split_after_column_num);
DSP_FUNCTION bool dsp_matrix_vertsplit(dsp_matrix_t* const T, dsp_matrix_t* const B, const dsp_matrix_t* const mat, const size_t split_after_row_num);
DSP_FUNCTION bool dsp_matrix_blocksplit(dsp_matrix_t* const TL, dsp_matrix_t* const TR, dsp_matrix_t* const BL, dsp_matrix_t* const BR, const dsp_matrix_t* const mat, const size_t split_after_row_num, const size_t split_after_column_num);


// Gauß-Jordan
DSP_FUNCTION real_t dsp_matrix_gauss_jordan(dsp_matrix_t* const A, dsp_matrix_t* const B);
DSP_FUNCTION bool dsp_matrix_swap_rows(dsp_matrix_t* const mat, const size_t row_a_index, const size_t row_b_index);
DSP_FUNCTION bool dsp_matrix_swap_columns(dsp_matrix_t* const mat, const size_t column_a_index, const size_t column_b_index);
DSP_FUNCTION bool dsp_matrix_scale_up_row(dsp_matrix_t* const mat, const size_t row_index, const size_t column_a_index, const size_t column_b_index, const real_t s);
DSP_FUNCTION bool dsp_matrix_scale_up_column(dsp_matrix_t* const mat, const size_t column_index, const size_t row_a_index, const size_t row_b_index, const real_t s);
DSP_FUNCTION bool dsp_matrix_scale_down_row(dsp_matrix_t* const mat, const size_t row_index, const size_t column_a_index, const size_t column_b_index, const real_t s);
DSP_FUNCTION bool dsp_matrix_scale_down_column(dsp_matrix_t* const mat, const size_t column_index, const size_t row_a_index, const size_t row_b_index, const real_t s);
DSP_FUNCTION bool dsp_matrix_modify_row(dsp_matrix_t* const mat, const size_t row_a_index, const size_t row_b_index, const size_t column_a_index, const size_t column_b_index, const real_t s);
DSP_FUNCTION bool dsp_matrix_modify_column(dsp_matrix_t* const mat, const size_t column_a_index, const size_t column_b_index, const size_t row_a_index, const size_t row_b_index, const real_t s);


#ifdef __cplusplus
}
#endif


#endif // SJ_MATRIX_H