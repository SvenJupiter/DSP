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

typedef enum sApproximation {
    ForwardEuler,  // s = (z - 1) / Ts
    BackwardEuler, // s = (z - 1) / (z * Ts)
    Trapezoidal // s = (2 / Ts) * (z - 1) / (Z + 1)
} s_approximation_t;

typedef enum AntiWindupMethod {
    None,
    Clamping,
    BackCalculation
} dsp_anti_windup_method_t;

typedef enum RoundingMethod {
    RoundMath = 0,
    RoundDown,
    RoundUp
} rounding_method_t;

// ----- Forward Declarations -----

// Math
typedef struct Polynomial dsp_poly_t;
typedef struct Matrix dsp_matrix_t;
typedef struct Vector dsp_vector_t;

// Discrete
typedef struct Signal dsp_signal_t;
typedef struct zTransferFunction dsp_ztf_t;
typedef struct zStateSpace dsp_zss_t;
typedef struct zStateObserver dsp_zso_t;
typedef struct pidController dsp_pid_t;
typedef struct Saturation dsp_saturation_t;
typedef struct DeadZone dsp_dead_zone_t;
typedef struct RateLimiter dsp_rate_limiter_t;
typedef struct Quantization dsp_quantization_t;



// ----- Function types -----

// Polynomial
typedef dsp_poly_t* (*dsp_polynomial_create_func_t) (const size_t order);
typedef dsp_poly_t* (*dsp_polynomial_create_from_array_func_t) (const size_t order, const real_t* const coeffs);
typedef dsp_poly_t* (*dsp_polynomial_create_fit_func_t) (const size_t order, const real_t* const x, const real_t* const y, const size_t size);
typedef dsp_poly_t* (*dsp_polynomial_create_copy_func_t) (const dsp_poly_t* const other);
typedef bool (*dsp_polynomial_copy_assign_func_t) (dsp_poly_t* const dest, const dsp_poly_t* const src);
typedef bool (*dsp_polynomial_copy_assign_array_func_t) (dsp_poly_t* const dest, const real_t* const src);
typedef dsp_poly_t* (*dsp_polynomial_create_move_func_t) (dsp_poly_t* const other);
typedef bool (*dsp_polynomial_move_assign_func_t) (dsp_poly_t* const dest, dsp_poly_t* const src);
typedef bool (*dsp_polynomial_swap_func_t) (dsp_poly_t* const a, dsp_poly_t* const b);
typedef bool (*dsp_polynomial_release_internal_array_func_t) (dsp_poly_t* const p);
typedef bool (*dsp_polynomial_destroy_func_t) (dsp_poly_t* const p);
typedef bool (*dsp_polynomial_has_same_size_func_t) (const dsp_poly_t* const a, const dsp_poly_t* const b);
typedef bool (*dsp_polynomial_add_func_t) (dsp_poly_t* const result, const dsp_poly_t* const p1, const dsp_poly_t* const p2);
typedef bool (*dsp_polynomial_add_and_assign_func_t) (dsp_poly_t* const p1, const dsp_poly_t* const p2);
typedef bool (*dsp_polynomial_subtract_func_t) (dsp_poly_t* const result, const dsp_poly_t* const p1, const dsp_poly_t* const p2);
typedef bool (*dsp_polynomial_subtract_and_assign_func_t) (dsp_poly_t* const p1, const dsp_poly_t* const p2);
typedef bool (*dsp_polynomial_multiply_func_t) (dsp_poly_t* const w, const dsp_poly_t* const u, const dsp_poly_t* const v);
typedef bool (*dsp_polynomial_divide_func_t) (dsp_poly_t* const q, dsp_poly_t* const r, const dsp_poly_t* const u, const dsp_poly_t* const v);
typedef bool (*dsp_polynomial_fit_func_t) (dsp_poly_t* const p, const real_t* const x, const real_t* const y, const size_t size);
typedef real_t (*dsp_polynomial_val_func_t) (const dsp_poly_t* const p, const real_t x);
typedef bool (*dsp_polynomial_shrink_to_fit_func_t) (dsp_poly_t* const p);
typedef bool (*dsp_polynomial_grow_to_func_t) (dsp_poly_t* const p, const size_t new_order);



// Matrix
typedef dsp_matrix_t* (*dsp_matrix_create_func_t)(const size_t rows, const size_t columns);
typedef dsp_matrix_t* (*dsp_matrix_create_from_array_func_t)(const size_t rows, const size_t columns, const real_t* elements);
typedef dsp_matrix_t* (*dsp_matrix_create_zeros_func_t)(const size_t rows, const size_t columns);
typedef dsp_matrix_t* (*dsp_matrix_create_eye_func_t)(const size_t rows, const size_t columns);
typedef dsp_matrix_t* (*dsp_matrix_create_diag_func_t)(const real_t* const elements, const size_t rows, const size_t columns);
typedef dsp_matrix_t* (*dsp_matrix_create_vandermonde_func_t)(const size_t order, const real_t* const x_values, const size_t size);
typedef dsp_matrix_t* (*dsp_matrix_create_product_func_t)(const dsp_matrix_t* A, const dsp_matrix_t* B);
typedef dsp_matrix_t* (*dsp_matrix_create_transpose_func_t)(const dsp_matrix_t* const mat);
typedef dsp_matrix_t* (*dsp_matrix_create_inv_func_t)(const dsp_matrix_t* const mat);
typedef dsp_matrix_t* (*dsp_matrix_create_pinv_func_t)(const dsp_matrix_t* const M);
typedef dsp_matrix_t* (*dsp_matrix_create_horzcat_func_t)(const dsp_matrix_t* const L, const dsp_matrix_t* const R);
typedef dsp_matrix_t* (*dsp_matrix_create_vertcat_func_t)(const dsp_matrix_t* const T, const dsp_matrix_t* const B);
typedef dsp_matrix_t* (*dsp_matrix_create_blockcat_func_t)(const dsp_matrix_t* const TL, const dsp_matrix_t* const TR, const dsp_matrix_t* const BL, const dsp_matrix_t* const BR);
typedef dsp_matrix_t* (*dsp_matrix_create_submatrix_func_t)(const dsp_matrix_t* const mat, const size_t row_index, const size_t column_index);
typedef dsp_matrix_t* (*dsp_matrix_create_copy_func_t)(const dsp_matrix_t* const other);
typedef bool (*dsp_matrix_copy_assign_func_t)(dsp_matrix_t* const dest, const dsp_matrix_t* const src);
typedef bool (*dsp_matrix_copy_assign_array_func_t)(dsp_matrix_t* const dest, const real_t* const src);
typedef dsp_matrix_t* (*dsp_matrix_create_move_func_t)(dsp_matrix_t* const other);
typedef bool (*dsp_matrix_move_assign_func_t)(dsp_matrix_t* const dest, dsp_matrix_t* const src);
typedef bool (*dsp_matrix_swap_func_t)(dsp_matrix_t* const a, dsp_matrix_t* const b);
typedef bool (*dsp_matrix_allocate_internal_array_func_t)(dsp_matrix_t* const mat, const size_t rows, const size_t columns);
typedef bool (*dsp_matrix_release_internal_array_func_t)(dsp_matrix_t* const mat);
typedef bool (*dsp_matrix_destroy_func_t)(dsp_matrix_t* const mat);
typedef bool (*dsp_matrix_has_same_size_func_t)(dsp_matrix_t* const a, dsp_matrix_t* const b);
typedef bool (*dsp_matrix_reshape_func_t)(dsp_matrix_t* const mat, const size_t new_rows, const size_t new_columns);
typedef bool (*dsp_matrix_set_to_zero_func_t)(dsp_matrix_t* const mat);
typedef bool (*dsp_matrix_set_to_eye_func_t)(dsp_matrix_t* const mat);
typedef bool (*dsp_matrix_add_func_t)(dsp_matrix_t* result, const dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);
typedef bool (*dsp_matrix_add_and_assign_func_t)(dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);
typedef bool (*dsp_matrix_subtract_func_t)(dsp_matrix_t* result, const dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);
typedef bool (*dsp_matrix_subtract_and_assign_func_t)(dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);
typedef bool (*dsp_matrix_multiply_func_t)(dsp_matrix_t* result, const dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2);
typedef bool (*dsp_matrix_multiply_by_scalar_func_t)(dsp_matrix_t* result, const dsp_matrix_t* const mat, const real_t scalar);
typedef bool (*dsp_matrix_multiply_by_scalar_and_assign_func_t)(dsp_matrix_t* mat, const real_t scalar);
typedef bool (*dsp_matrix_divide_by_scalar_func_t)(dsp_matrix_t* result, const dsp_matrix_t* const mat, const real_t scalar);
typedef bool (*dsp_matrix_divide_by_scalar_and_assign_func_t)(dsp_matrix_t* mat, const real_t scalar);
typedef real_t (*dsp_matrix_det_func_t)(const dsp_matrix_t* const mat);
typedef bool (*dsp_matrix_transpose_func_t)(dsp_matrix_t* const result, const dsp_matrix_t* const mat);
typedef bool (*dsp_matrix_inv_func_t)(dsp_matrix_t* const result, const dsp_matrix_t* const mat);
typedef bool (*dsp_matrix_pinv_func_t)(dsp_matrix_t* const result, const dsp_matrix_t* const mat);
typedef bool (*dsp_matrix_horzcat_func_t)(dsp_matrix_t* const result, const dsp_matrix_t* const L, const dsp_matrix_t* const R);
typedef bool (*dsp_matrix_vertcat_func_t)(dsp_matrix_t* const result, const dsp_matrix_t* const T, const dsp_matrix_t* const B);
typedef bool (*dsp_matrix_blockcat_func_t)(dsp_matrix_t* const result, const dsp_matrix_t* const TL, const dsp_matrix_t* const TR, const dsp_matrix_t* const BL, const dsp_matrix_t* const BR);
typedef bool (*dsp_matrix_horzsplit_func_t)(dsp_matrix_t* const L, dsp_matrix_t* const R, const dsp_matrix_t* const mat, const size_t split_after_column_num);
typedef bool (*dsp_matrix_vertsplit_func_t)(dsp_matrix_t* const T, dsp_matrix_t* const B, const dsp_matrix_t* const mat, const size_t split_after_row_num);
typedef bool (*dsp_matrix_blocksplit_func_t)(dsp_matrix_t* const TL, dsp_matrix_t* const TR, dsp_matrix_t* const BL, dsp_matrix_t* const BR, const dsp_matrix_t* const mat, const size_t split_after_row_num, const size_t split_after_column_num);
typedef real_t (*dsp_matrix_gauss_jordan_func_t)(dsp_matrix_t* const A, dsp_matrix_t* const B);
typedef bool (*dsp_matrix_swap_rows_func_t)(dsp_matrix_t* const mat, const size_t row_a_index, const size_t row_b_index);
typedef bool (*dsp_matrix_swap_columns_func_t)(dsp_matrix_t* const mat, const size_t column_a_index, const size_t column_b_index);
typedef bool (*dsp_matrix_scale_up_row_func_t)(dsp_matrix_t* const mat, const size_t row_index, const size_t column_a_index, const size_t column_b_index, const real_t s);
typedef bool (*dsp_matrix_scale_up_column_func_t)(dsp_matrix_t* const mat, const size_t column_index, const size_t row_a_index, const size_t row_b_index, const real_t s);
typedef bool (*dsp_matrix_scale_down_row_func_t)(dsp_matrix_t* const mat, const size_t row_index, const size_t column_a_index, const size_t column_b_index, const real_t s);
typedef bool (*dsp_matrix_scale_down_column_func_t)(dsp_matrix_t* const mat, const size_t column_index, const size_t row_a_index, const size_t row_b_index, const real_t s);
typedef bool (*dsp_matrix_modify_row_func_t)(dsp_matrix_t* const mat, const size_t row_a_index, const size_t row_b_index, const size_t column_a_index, const size_t column_b_index, const real_t s);
typedef bool (*dsp_matrix_modify_column_func_t)(dsp_matrix_t* const mat, const size_t column_a_index, const size_t column_b_index, const size_t row_a_index, const size_t row_b_index, const real_t s);



// Vector
typedef dsp_vector_t* (*dsp_vector_create_func_t)(const size_t size);
typedef dsp_vector_t* (*dsp_vector_create_from_array_func_t)(const size_t size, const real_t* const elements);
typedef dsp_vector_t* (*dsp_vector_create_zerro_vector_func_t)(const size_t size);
typedef dsp_vector_t* (*dsp_vector_create_unit_vector_func_t)(const size_t size, const size_t dim_index);
typedef dsp_vector_t* (*dsp_vector_create_normalized_vector_func_t)(const dsp_vector_t* const vec);
typedef dsp_vector_t* (*dsp_vector_create_projection_func_t)(const dsp_vector_t* const a, const dsp_vector_t* const b);
typedef dsp_vector_t* (*dsp_vector_create_cross_product_func_t)(const dsp_vector_t* const a, const dsp_vector_t* const b); // Cross product (for R^3 vectors only)
typedef dsp_vector_t* (*dsp_vector_create_matrix_product_func_t)(const dsp_matrix_t* const A, const dsp_vector_t* const x);
typedef dsp_vector_t* (*dsp_vector_create_solution_func_t)(const dsp_matrix_t* const A, const dsp_vector_t* const b);
typedef dsp_vector_t* (*dsp_vector_create_concat_func_t)(const dsp_vector_t* const T, const dsp_vector_t* const B);
typedef dsp_vector_t* (*dsp_vector_create_from_matrix_func_t)(const dsp_matrix_t* const mat);
typedef dsp_vector_t* (*dsp_matrix_create_row_vector_func_t)(const dsp_matrix_t* const mat, const size_t row_index);
typedef dsp_vector_t* (*dsp_matrix_create_column_vector_func_t)(const dsp_matrix_t* const mat, const size_t column_index);
typedef dsp_vector_t* (*dsp_vector_create_copy_func_t)(const dsp_vector_t* const other);
typedef bool (*dsp_vector_copy_assign_func_t)(dsp_vector_t* const dest, const dsp_vector_t* const src);
typedef bool (*dsp_vector_copy_assign_array_func_t)(dsp_vector_t* const dest, const real_t* const src);
typedef dsp_vector_t* (*dsp_vector_create_move_func_t)(dsp_vector_t* const other);
typedef bool (*dsp_vector_move_assign_func_t)(dsp_vector_t* const dest, dsp_vector_t* const src);
typedef bool (*dsp_vector_swap_func_t)(dsp_vector_t* const a, dsp_vector_t* const b);
typedef bool (*dsp_vector_allocate_internal_array_func_t)(dsp_vector_t* const vec, const size_t size);
typedef bool (*dsp_vector_release_internal_array_func_t)(dsp_vector_t* const vec);
typedef bool (*dsp_vector_destroy_func_t)(dsp_vector_t* const vec);
typedef bool (*dsp_vector_has_same_size_func_t)(dsp_vector_t* const a, dsp_vector_t* const b);
typedef bool (*dsp_vector_set_to_zero_func_t)(dsp_vector_t* const vec);
typedef bool (*dsp_vector_set_to_unit_func_t)(dsp_vector_t* const vec, const size_t dim_index);
typedef bool (*dsp_vector_normalize_func_t)(dsp_vector_t* const vec);
typedef bool (*dsp_vector_set_to_normalized_func_t)(dsp_vector_t* const vec_n, const dsp_vector_t* const vec);
typedef bool (*dsp_vector_set_to_projection_func_t)(dsp_vector_t* const vec, const dsp_vector_t* const a, const dsp_vector_t* const b);
typedef bool (*dsp_vector_set_to_cross_product_func_t)(dsp_vector_t* const vec, const dsp_vector_t* const a, const dsp_vector_t* const b); // Cross product (for R^3 vectors only)
typedef size_t (*dsp_vector_size_func_t)(const dsp_vector_t* const vec);
typedef real_t (*dsp_vector_length_squared_func_t)(const dsp_vector_t* const vec);
typedef real_t (*dsp_vector_length_func_t)(const dsp_vector_t* const vec);
typedef real_t (*dsp_vector_dot_product_func_t)(const dsp_vector_t* const a, const dsp_vector_t* const b);
typedef real_t (*dsp_vector_cosphi_func_t)(const dsp_vector_t* const a, const dsp_vector_t* const b);
typedef real_t (*dsp_vector_angle_func_t)(const dsp_vector_t* const a, const dsp_vector_t* const b);
typedef real_t (*dsp_vector_project_func_t)(const dsp_vector_t* const a, const dsp_vector_t* const b);
typedef bool (*dsp_vector_add_func_t)(dsp_vector_t* const result, const dsp_vector_t* const a, const dsp_vector_t* const b);
typedef bool (*dsp_vector_add_and_assign_func_t)(dsp_vector_t* const a, const dsp_vector_t* const b);
typedef bool (*dsp_vector_subtract_func_t)(dsp_vector_t* const result, const dsp_vector_t* const a, const dsp_vector_t* const b);
typedef bool (*dsp_vector_subtract_and_assign_func_t)(dsp_vector_t* const a, const dsp_vector_t* const b);
typedef bool (*dsp_vector_multiply_by_scalar_func_t)(dsp_vector_t* const result, const dsp_vector_t* const vec, const real_t scalar);
typedef bool (*dsp_vector_multiply_by_scalar_and_assign_func_t)(dsp_vector_t* const vec, const real_t scalar);
typedef bool (*dsp_vector_divide_by_scalar_func_t)(dsp_vector_t* const result, const dsp_vector_t* const vec, const real_t scalar);
typedef bool (*dsp_vector_divide_by_scalar_and_assign_func_t)(dsp_vector_t* const vec, const real_t scalar);
typedef bool (*dsp_vector_concat_func_t)(dsp_vector_t* const result, const dsp_vector_t* const T, const dsp_vector_t* const B);
typedef bool (*dsp_vector_split_func_t)(dsp_vector_t* const T, dsp_vector_t* const B, const dsp_vector_t* const vec, const size_t split_after_element_num);
typedef bool (*dsp_vector_conv_func_t)(dsp_vector_t* const w, const dsp_vector_t* const u, const dsp_vector_t* const v);
typedef bool (*dsp_vector_deconv_func_t)(dsp_vector_t* const q, dsp_vector_t* const r, const dsp_vector_t* const u, const dsp_vector_t* const v);
typedef dsp_matrix_t* (*dsp_matrix_create_from_vector_func_t)(const size_t rows, const size_t columns, const dsp_vector_t* vec);
typedef bool (*dsp_matrix_vector_multiply_func_t)(dsp_vector_t* const result, const dsp_matrix_t* const mat, const dsp_vector_t* const vec);
typedef bool (*dsp_matrix_vector_multiply_and_add_to_vector_func_t)(dsp_vector_t* const acc, const dsp_matrix_t* const mat, const dsp_vector_t* const vec);
typedef bool (*dsp_matrix_vector_multiply_and_subtract_from_vector_func_t)(dsp_vector_t* const acc, const dsp_matrix_t* const mat, const dsp_vector_t* const vec);
typedef bool (*dsp_matrix_assign_row_vector_func_t)(dsp_matrix_t* const mat, const size_t row_index, const dsp_vector_t* const row_vec);
typedef bool (*dsp_matrix_assign_column_vector_func_t)(dsp_matrix_t* const mat, const size_t column_index, const dsp_vector_t* const colum_vec);
typedef bool (*dsp_matrix_add_on_row_vector_func_t)(dsp_matrix_t* const mat, const size_t row_index, const dsp_vector_t* const row_vec);
typedef bool (*dsp_matrix_add_on_column_vector_func_t)(dsp_matrix_t* const mat, const size_t column_index, const dsp_vector_t* const colum_vec);
typedef bool (*dsp_matrix_subract_of_row_vector_func_t)(dsp_matrix_t* const mat, const size_t row_index, const dsp_vector_t* const row_vec);
typedef bool (*dsp_matrix_subract_of_column_vector_func_t)(dsp_matrix_t* const mat, const size_t column_index, const dsp_vector_t* const colum_vec);
typedef bool (*dsp_vector_solve_lse_func_t)(dsp_vector_t* const x, const dsp_matrix_t* const A, const dsp_vector_t* const b);
typedef bool (*dsp_solve_func_t)(real_t* const x, const size_t x_size, const real_t* const A, const real_t* const b, const size_t b_size);



// Signal
typedef real_t (*dsp_dot_product_func_t)(const real_t* const u, const real_t* const v, const size_t size);
typedef size_t (*dsp_conv_func_t)(const real_t* const u, const size_t u_size, const real_t* const v, const size_t v_size, real_t* const w, const size_t w_size);
typedef size_t (*dsp_deconv_func_t)(const real_t* const u, const size_t u_size, const real_t* const v, const size_t v_size, real_t* const q, size_t q_size, real_t* const r, size_t r_size);
typedef dsp_signal_t* (*dsp_signal_create_func_t)(const size_t initial_capacity);
typedef dsp_signal_t (*dsp_signal_construct_func_t)(const size_t initial_capacity);
typedef dsp_signal_t* (*dsp_signal_create_copy_func_t)(const dsp_signal_t* const other);
typedef bool (*dsp_signal_copy_assign_func_t)(dsp_signal_t* const  dest, const dsp_signal_t* const src);
typedef dsp_signal_t* (*dsp_signal_create_move_func_t)(dsp_signal_t* const other);
typedef bool (*dsp_signal_move_assign_func_t)(dsp_signal_t* const  dest, dsp_signal_t* const src);
typedef bool (*dsp_signal_destruct_func_t)(dsp_signal_t* const signal);
typedef bool (*dsp_signal_destroy_func_t)(dsp_signal_t* const signal);
typedef real_t (*dsp_signal_dot_product_func_t)(const dsp_signal_t* const u, const dsp_signal_t* const v);
typedef size_t (*dsp_signal_conv_func_t)(dsp_signal_t* const w, const dsp_signal_t* const u, const dsp_signal_t* const v);
typedef size_t (*dsp_signal_deconv_func_t)(dsp_signal_t* const q, dsp_signal_t* const r, const dsp_signal_t* const u, const dsp_signal_t* const v);
typedef void (*dsp_signal_reserve_func_t)(dsp_signal_t* const signal, const size_t new_capacity);
typedef void (*dsp_signal_resize_func_t)(dsp_signal_t* const signal, const size_t new_size, const real_t* const value_ptr);
typedef void (*dsp_signal_shrink_to_fit_func_t)(dsp_signal_t* const signal);
typedef real_t* (*dsp_signal_at_func_t)(dsp_signal_t* const signal, const size_t position);
typedef real_t* (*dsp_signal_front_func_t)(dsp_signal_t* const signal);
typedef real_t* (*dsp_signal_back_func_t)(dsp_signal_t* const signal);
typedef real_t* (*dsp_signal_data_func_t)(dsp_signal_t* const signal);
typedef size_t (*dsp_signal_size_func_t)(dsp_signal_t* const signal);
typedef size_t (*dsp_signal_capacity_func_t)(dsp_signal_t* const signal);
typedef bool (*dsp_signal_empty_func_t)(dsp_signal_t* const signal);
typedef void (*dsp_signal_assign_func_t)(dsp_signal_t* const signal, const real_t* const new_elements, const size_t new_size);
typedef void (*dsp_signal_push_back_func_t)(dsp_signal_t* const signal, const real_t* const new_element);
typedef void (*dsp_signal_pop_back_func_t)(dsp_signal_t* const signal);
typedef real_t* (*dsp_signal_insert_func_t)(dsp_signal_t* const signal, const size_t position, const real_t* const new_element);
typedef void (*dsp_signal_erase_func_t)(dsp_signal_t* const signal, const size_t position);
typedef void (*dsp_signal_swap_func_t)(dsp_signal_t* const vec_a_ptr, dsp_signal_t* const vec_b_ptr);
typedef void (*dsp_signal_clear_func_t)(dsp_signal_t* const signal);
typedef real_t* (*dsp_signal_emplace_func_t)(dsp_signal_t* const signal, const size_t position, const bool fill_zeros);
typedef real_t* (*dsp_signal_emplace_back_func_t)(dsp_signal_t* const signal, const bool fill_zeros);



// Z-Transfer-Function
typedef dsp_ztf_t* (*dsp_ztf_create_func_t)(const size_t order);
typedef dsp_ztf_t* (*dsp_ztf_create_from_arrays_func_t)(const size_t order, const real_t* const num, const real_t* const den, const real_t* initial_u, const real_t* initial_y);
typedef dsp_ztf_t* (*dsp_ztf_create_PT1_func_t)(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y);
typedef dsp_ztf_t* (*dsp_ztf_create_lead_lag_compensator_func_t)(const real_t T1, const real_t T2, const real_t Ts, const real_t initial_u, const real_t initial_y);
typedef dsp_ztf_t* (*dsp_ztf_create_lowpass_filter_func_t)(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y);
typedef dsp_ztf_t* (*dsp_ztf_create_highpass_filter_func_t)(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y);
typedef dsp_ztf_t* (*dsp_ztf_create_integrator_func_t)(const real_t K, const real_t Ts, const real_t initial_u, const real_t initial_y, const s_approximation_t IF);
typedef dsp_ztf_t* (*dsp_ztf_create_derivative_func_t)(const real_t K, const real_t T, const real_t Ts, const real_t initial_u, const real_t initial_y, const s_approximation_t DF);
typedef bool (*dsp_ztf_destroy_func_t)(dsp_ztf_t* const ztf);
typedef bool (*dsp_ztf_set_coefficients_func_t)(dsp_ztf_t* const ztf, const real_t* const num, const real_t* const den);
typedef bool (*dsp_ztf_set_initial_condition_func_t)(dsp_ztf_t* const ztf, const real_t* initial_u, const real_t* initial_y);
typedef bool (*dsp_ztf_reset_func_t)(dsp_ztf_t* const ztf);
typedef real_t (*dsp_ztf_update_func_t)(dsp_ztf_t* const ztf, const real_t new_u);
typedef real_t (*dsp_ztf_output_func_t)(dsp_ztf_t* const ztf);



// Z-State-Space
typedef dsp_zss_t* (*dsp_zss_create_func_t)(const size_t nx,const size_t nu, const size_t ny);
typedef dsp_zss_t* (*dsp_zss_create_from_arrays_func_t)(const size_t nx, const size_t nu, const size_t ny, const real_t* const a, const real_t* const b, const real_t* const c, const real_t* const d, const real_t* const x0);
typedef dsp_zss_t* (*dsp_zss_create_from_matrices_func_t)(const dsp_matrix_t* const A, const dsp_matrix_t* const B, const dsp_matrix_t* const C, const dsp_matrix_t* const D, const real_t* const x0);
typedef dsp_zss_t* (*dsp_zss_create_from_tf_func_t)(const size_t order, const real_t* const num, const real_t* const den, const real_t* const x0);
typedef dsp_zss_t* (*dsp_zss_create_from_poly_func_t)(const dsp_poly_t* const num, const dsp_poly_t* const den, const real_t* const x0);
typedef dsp_zss_t* (*dsp_zss_create_from_ztf_func_t)(const dsp_ztf_t* const ztf, const real_t* const x0);
typedef dsp_zss_t* (*dsp_zss_create_pt1_func_t)(const real_t K, const real_t T, const real_t Ts, const real_t x0);
typedef dsp_zss_t* (*dsp_zss_create_lead_lag_compensator_func_t)(const real_t T1, const real_t T2, const real_t Ts, const real_t x0);
typedef dsp_zss_t* (*dsp_zss_create_lowpass_filter_func_t)(const real_t K, const real_t T, const real_t Ts, const real_t x0);
typedef dsp_zss_t* (*dsp_zss_create_highpass_filter_func_t)(const real_t K, const real_t T, const real_t Ts, const real_t x0);
typedef dsp_zss_t* (*dsp_zss_create_integrator_func_t)(const real_t K, const real_t Ts, const s_approximation_t s_approx , const real_t x0);
typedef dsp_zss_t* (*dsp_zss_create_derivative_func_t)(const real_t K, const real_t T, const real_t Ts, const s_approximation_t s_approx, const real_t x0);
typedef dsp_zss_t* (*dsp_zss_create_copy_func_t)(const dsp_zss_t* const other);
typedef bool (*dsp_zss_copy_assign_func_t)(dsp_zss_t* const dest, const dsp_zss_t* const src);
typedef dsp_zss_t* (*dsp_zss_create_move_func_t)(dsp_zss_t* const other);
typedef bool (*dsp_zss_move_assign_func_t)(dsp_zss_t* const dest, dsp_zss_t* const src);
typedef bool (*dsp_zss_swap_func_t)(dsp_zss_t* const a, dsp_zss_t* const b);
typedef bool (*dsp_zss_allocate_internal_arrays_func_t)(dsp_zss_t* const zss, const size_t nx, const size_t nu, const size_t ny);
typedef bool (*dsp_zss_release_internal_arrays_func_t)(dsp_zss_t* const zss);
typedef bool (*dsp_zss_destroy_func_t)(dsp_zss_t* const zss);
typedef bool (*dsp_zss_set_state_to_func_t)(dsp_zss_t* const zss, const dsp_vector_t* const x0);
typedef bool (*dsp_zss_set_state_func_t)(dsp_zss_t* const zss, const real_t* const x0);
typedef bool (*dsp_zss_reset_func_t)(dsp_zss_t* const zss);
typedef bool (*dsp_zss_vector_update_state_func_t)(dsp_zss_t* const zss, const dsp_vector_t* const u);
typedef bool (*dsp_zss_vector_output_func_t)(dsp_zss_t* const zss, const dsp_vector_t* const u, dsp_vector_t* const y);
typedef bool (*dsp_zss_vector_update_func_t)(dsp_zss_t* const zss, const dsp_vector_t* const u, dsp_vector_t* const y);
typedef bool (*dsp_zss_update_state_func_t)(dsp_zss_t* const zss, const real_t* const u);
typedef bool (*dsp_zss_output_func_t)(dsp_zss_t* const zss, const real_t* const u, real_t* const y);
typedef bool (*dsp_zss_update_func_t)(dsp_zss_t* const zss, const real_t* const u, real_t* const y);



// Z-State-Observer
typedef dsp_zso_t* (*dsp_zso_create_func_t)(const size_t nx,const size_t nu, const size_t ny);
typedef dsp_zso_t* (*dsp_zso_create_from_arrays_func_t)(const size_t nx, const size_t nu, const size_t ny, const real_t* const a, const real_t* const b, const real_t* const c, const real_t* const d, const real_t* const l, const real_t* const x0);
typedef dsp_zso_t* (*dsp_zso_create_from_matrices_func_t)(const dsp_matrix_t* const A, const dsp_matrix_t* const B, const dsp_matrix_t* const C, const dsp_matrix_t* const D, const dsp_matrix_t* const L, const real_t* const x0);
typedef dsp_zso_t* (*dsp_zso_create_from_zss_func_t)(const dsp_zss_t* const zss, const real_t* const l, const real_t* const x0);
typedef dsp_zso_t* (*dsp_zso_create_copy_func_t)(const dsp_zso_t* const other);
typedef bool (*dsp_zso_copy_assign_func_t)(dsp_zso_t* const dest, const dsp_zso_t* const src);
typedef dsp_zso_t* (*dsp_zso_create_move_func_t)(dsp_zso_t* const other);
typedef bool (*dsp_zso_move_assign_func_t)(dsp_zso_t* const dest, dsp_zso_t* const src);
typedef bool (*dsp_zso_swap_func_t)(dsp_zso_t* const a, dsp_zso_t* const b);
typedef bool (*dsp_zso_allocate_internal_arrays_func_t)(dsp_zso_t* const zso, const size_t nx, const size_t nu, const size_t ny);
typedef bool (*dsp_zso_release_internal_arrays_func_t)(dsp_zso_t* const zso);
typedef bool (*dsp_zso_destroy_func_t)(dsp_zso_t* const zso);
typedef bool (*dsp_zso_set_state_to_func_t)(dsp_zso_t* const zso, const dsp_vector_t* const x0);
typedef bool (*dsp_zso_set_state_func_t)(dsp_zso_t* const zso, const real_t* const x0);
typedef bool (*dsp_zso_reset_func_t)(dsp_zso_t* const zso);
typedef bool (*dsp_zso_vector_update_state_func_t)(dsp_zso_t* const zso, const dsp_vector_t* const u, const dsp_vector_t* const y);
typedef bool (*dsp_zso_vector_output_func_t)(dsp_zso_t* const zso, dsp_vector_t* const xh);
typedef bool (*dsp_zso_vector_update_func_t)(dsp_zso_t* const zso, const dsp_vector_t* const u, const dsp_vector_t* const y, dsp_vector_t* const xh);
typedef bool (*dsp_zso_update_state_func_t)(dsp_zso_t* const zso, const real_t* const u, const real_t* const y);
typedef bool (*dsp_zso_output_func_t)(dsp_zso_t* const zso, real_t* const xh);
typedef bool (*dsp_zso_update_func_t)(dsp_zso_t* const zso, const real_t* const u, const real_t* const y, real_t* const xh);



// PID-Controller
typedef dsp_pid_t* (*dsp_pid_create_func_t)(const real_t Kp, const real_t Ki, const real_t Kd, const real_t Ts, const s_approximation_t IF, const real_t Tf, const s_approximation_t DF);
typedef dsp_pid_t* (*dsp_pid_create_copy_func_t)(const dsp_pid_t* const other);
typedef bool (*dsp_pid_copy_assign_func_t)(dsp_pid_t* const dest, const dsp_pid_t* const src);
typedef dsp_pid_t* (*dsp_pid_create_move_func_t)(dsp_pid_t* const other);
typedef bool (*dsp_pid_move_assign_func_t)(dsp_pid_t* const dest, dsp_pid_t* const src);
typedef bool (*dsp_pid_swap_func_t)(dsp_pid_t* const a, dsp_pid_t* const b);
typedef bool (*dsp_pid_release_internal_arrays_func_t)(dsp_pid_t* const pid);
typedef bool (*dsp_pid_destroy_func_t)(dsp_pid_t* const pid);
typedef bool (*dsp_pid_set_gains_func_t)(dsp_pid_t* const pid, const real_t Kp, const real_t Ki, const real_t Kd);
typedef bool (*dsp_pid_set_zss_func_t)(dsp_pid_t* const pid, const real_t Ts, const s_approximation_t IF, const real_t Tf, const s_approximation_t DF);
typedef bool (*dsp_pid_set_initial_state_func_t)(dsp_pid_t* const pid, const real_t* const xi0, const real_t* const xd0);
typedef bool (*dsp_reset_func_t)(dsp_pid_t* const pid);
typedef bool (*dsp_pid_set_output_saturation_func_t)(dsp_pid_t* const pid, const bool limit_output, const real_t upper_limit, const real_t lower_limit);
typedef bool (*dsp_pid_set_anti_windup_func_t)(dsp_pid_t* const pid, const dsp_anti_windup_method_t anti_windup_method, real_t Kb);
typedef bool (*dsp_pid_set_tracking_func_t)(dsp_pid_t* const pid, const bool tracking_enabled, real_t Kt);
typedef real_t (*dsp_pid_update_func_t)(dsp_pid_t* const pid, const real_t e, const real_t tr);


// Discontinuous
typedef dsp_saturation_t* (*dsp_saturation_create_func_t)(const real_t upper_limit, const real_t lower_limit);
typedef bool (*dsp_saturation_set_limits_func_t)(dsp_saturation_t* const saturation, const real_t upper_limit, const real_t lower_limit);
typedef real_t (*dsp_saturation_update_func_t)(dsp_saturation_t* const saturation, const real_t new_input);
typedef real_t (*dsp_saturation_output_func_t)(dsp_saturation_t* const saturation);
typedef bool (*dsp_saturation_destroy_func_t)(dsp_saturation_t* const saturation);
typedef dsp_dead_zone_t* (*dsp_dead_zone_create_func_t)(const real_t upper_limit, const real_t lower_limit);
typedef bool (*dsp_dead_zone_set_limits_func_t)(dsp_dead_zone_t* const dead_zone, const real_t upper_limit, const real_t lower_limit);
typedef real_t (*dsp_dead_zone_update_func_t)(dsp_dead_zone_t* const dead_zone, const real_t new_input);
typedef real_t (*dsp_dead_zone_output_func_t)(dsp_dead_zone_t* const dead_zone);
typedef bool (*dsp_dead_zone_destroy_func_t)(dsp_dead_zone_t* const dead_zone);
typedef dsp_rate_limiter_t* (*dsp_rate_limiter_create_func_t)(const real_t upper_rate, const real_t lower_rate, const real_t Ts, const real_t initial_output);
typedef bool (*dsp_rate_limiter_set_limits_func_t)(dsp_rate_limiter_t* const rate_limiter, const real_t upper_rate, const real_t lower_rate, const real_t Ts);
typedef bool (*dsp_rate_limiter_set_initial_condition_func_t)(dsp_rate_limiter_t* const rate_limiter, const real_t initial_output);
typedef bool (*dsp_rate_limiter_reset_func_t)(dsp_rate_limiter_t* const rate_limiter);
typedef real_t (*dsp_rate_limiter_update_func_t)(dsp_rate_limiter_t* const rate_limiter, const real_t new_input);
typedef real_t (*dsp_rate_limiter_get_output_func_t)(dsp_rate_limiter_t* const rate_limiter);
typedef bool (*dsp_rate_limiter_destroy_func_t)(dsp_rate_limiter_t* const rate_limiter);
typedef dsp_quantization_t* (*dsp_quantizer_create_func_t)(const real_t offset, const real_t interval, const rounding_method_t method);
typedef dsp_quantization_t* (*dsp_rounding_create_func_t)(const size_t precision, const rounding_method_t method);
typedef bool (*dsp_rounding_set_precision_func_t)(dsp_quantization_t* const rounder, const size_t precision, const rounding_method_t method);
typedef bool (*dsp_quantizer_set_parameters_func_t)(dsp_quantization_t* const quantizer, const real_t offset, const real_t interval, const rounding_method_t method);
typedef real_t (*dsp_quantizer_update_func_t)(dsp_quantization_t* const quantizer, const real_t new_input);
typedef real_t (*dsp_quantizer_get_output_func_t)(dsp_quantization_t* const quantizer);
typedef bool (*dsp_quantizer_destroy_func_t)(dsp_quantization_t* const quantizer);

#endif // SJ_DSP_TYPES_H