#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset, memmove
#include <math.h> // sqrtf, acosf
#include "DSP/Math/Vector.h"




#define VECTOR_SIZE sizeof(dsp_vector_t)
#define NEW_VECTOR() ((dsp_vector_t*) malloc(VECTOR_SIZE))

#define REAL_SIZE sizeof(real_t)
#define ARRAY_SIZE(size) ((size) * REAL_SIZE)
#define NEW_ARRAY(size) ((real_t*) malloc(ARRAY_SIZE(size)))
#define ELEMENT(vec, index) ((vec)->elements[(index)])


#define VECTOR_ELEMENT(vec, index) ELEMENT(vec, index)
#define MATRIX_ELEMENT(mat, row_index, column_index) ((mat)->elements[(row_index) * mat->columns + (column_index)])





// Create a vector of size 'size' but don't initilize its elements
dsp_vector_t* dsp_vector_create(const size_t size) {
    if (size == 0) { return NULL; }

    // Allocate space for a new vector
    dsp_vector_t* const vec = NEW_VECTOR();
    if (vec != NULL) {
        vec->elements = NULL;

        // Allocate space for the elements
        if (dsp_vector_allocate_internal_array(vec, size)) {
            return vec; 
        }
        else { 
            free(vec); 
            return NULL; 
        }
    }
    else {
        return NULL;
    }
}

// Create a vector from a givan array (if 'elements' is NULL, set all elements to 0)
dsp_vector_t* dsp_vector_create_from_array(const size_t size, const real_t* const elements) {

    // Create a new vector
    dsp_vector_t* const vec = dsp_vector_create(size);
    if (vec != NULL) {

        // Initilize its elements
        if (elements != NULL) { memcpy(vec->elements, elements, ARRAY_SIZE(size)); }
        else { memset(vec->elements, 0, ARRAY_SIZE(size)); }
    }
    return vec;
}

// Create
dsp_vector_t* dsp_vector_create_zerro_vector(const size_t size) {

    // Create a new vector
    return dsp_vector_create_from_array(size, NULL);
}
dsp_vector_t* dsp_vector_create_unit_vector(const size_t size, const size_t dim_index) {
    if (dim_index >= size) { return NULL; }

    // Create a new vector
    dsp_vector_t* const vec = dsp_vector_create_zerro_vector(size);
    if (vec == NULL) { return NULL; }

    // Set a single element to '1'
    vec->elements[dim_index] = 1;

    return vec;
}
dsp_vector_t* dsp_vector_create_normalized_vector(const dsp_vector_t* const vec) {
    if (vec == NULL) { return NULL; }

    // Create a new vector
    dsp_vector_t* const vec_n = dsp_vector_create(vec->size);
    if (vec_n == NULL) { return NULL; }

    if (dsp_vector_set_to_normalized(vec_n, vec)) {
        return vec_n;
    }
    else {
        dsp_vector_destroy(vec_n);
        return NULL;
    }
}
dsp_vector_t* dsp_vector_create_projection(const dsp_vector_t* const a, const dsp_vector_t* const b) {
    if (a == NULL || b == NULL) { return NULL; }
    if (a->size != b->size) { return NULL; }

    // Create a new vector
    dsp_vector_t* const vec = dsp_vector_create(a->size);
    if (vec == NULL) { return NULL; }

    if (dsp_vector_set_to_projection(vec, a, b)) {
        return vec;
    }
    else {
        dsp_vector_destroy(vec);
        return NULL;
    }
}
dsp_vector_t* dsp_vector_create_cross_product(const dsp_vector_t* const a, const dsp_vector_t* const b) { // Cross product (for R^3 vectors only)
    if (a == NULL || b == NULL) { return NULL; }
    if (a->size != 3) { return NULL; }
    if (b->size != 3) { return NULL; }

    // Create a new vector
    dsp_vector_t* const vec = dsp_vector_create(a->size);
    if (vec == NULL) { return NULL; }

    dsp_vector_set_to_cross_product(vec, a, b);
    return vec;
}
dsp_vector_t* dsp_vector_create_matrix_product(const dsp_matrix_t* const A, const dsp_vector_t* const x) {
    if (A == NULL || x == NULL) { return NULL; }
    if (A->columns != x->size) { return NULL; }

    // Create a new vector
    dsp_vector_t* const b = dsp_vector_create(A->rows);
    if (b == NULL) { return NULL; }

    // Calculate b = A * x
    if (dsp_matrix_vector_multiply(b, A, x)) {
        return b;
    }
    else {
        dsp_vector_destroy(b);
        return NULL;
    }
}
dsp_vector_t* dsp_vector_create_solution(const dsp_matrix_t* const A, const dsp_vector_t* const b) {
    if (A == NULL || b == NULL) { return NULL; }

    // Create a new vector
    dsp_vector_t* const x = dsp_vector_create(A->columns);
    if (x == NULL) { return NULL; }

    // Calculate solution to A * x = b
    if (dsp_vector_solve_lse(x, A, b)) {
        return x;
    }
    else {
        dsp_vector_destroy(x);
        return NULL;
    }
}

// Concat
dsp_vector_t* dsp_vector_create_concat(const dsp_vector_t* const T, const dsp_vector_t* const B) {
    if (T == NULL || B == NULL) { return NULL; }

    // Create a new Vector
    dsp_vector_t* const vec = dsp_vector_create(T->size + B->size);
    if (vec == NULL) { return NULL; }

    // Concat
    if (dsp_vector_concat(vec, T, B)) {
        return vec;
    }
    else {
        dsp_vector_destroy(vec);
        return NULL;
    }
}

// Create a vector from a given matrix
dsp_vector_t* dsp_vector_create_from_matrix(const dsp_matrix_t* const mat) {
    if (mat == NULL) { return NULL; }
    return dsp_vector_create_from_array(mat->rows * mat->columns, mat->elements);
}

// Create a vector from a matrix row
dsp_vector_t* dsp_matrix_create_row_vector(const dsp_matrix_t* const mat, const size_t row_index) {
    if (mat == NULL) { return NULL; }
    if (row_index >= mat->rows) { return NULL; }

    // Rows are stored in a continous section of the 'mat->elements' array
    return dsp_vector_create_from_array(mat->columns, &(mat->elements[row_index * mat->columns]));
}

// Create a vector from a matrix column
dsp_vector_t* dsp_matrix_create_column_vector(const dsp_matrix_t* const mat, const size_t column_index) {
    if (mat == NULL) { return NULL; }
    if (column_index >= mat->columns) { return NULL; }

    // Create a new vector
    dsp_vector_t* const vec  = dsp_vector_create(mat->rows);
    if (vec == NULL) { return NULL; }

    // Initilize its elements
    for (size_t k = 0; k < mat->rows; ++k) {
        VECTOR_ELEMENT(vec, k) = MATRIX_ELEMENT(mat, k, column_index);
    }

    return vec;
}

// Copy
dsp_vector_t* dsp_vector_create_copy(const dsp_vector_t* const other) {
    if (other == NULL) { return NULL; }
    return dsp_vector_create_from_array(other->size, other->elements);
}
bool dsp_vector_copy_assign(dsp_vector_t* const dest, const dsp_vector_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    if (dest->elements == src->elements) { return false; }
    if (dest->size != src->size) { return false; }
    if (src->size == 0) { return false; }

    // Copy elements
    memcpy(dest->elements, src->elements, ARRAY_SIZE(src->size));
    return true;
}
bool dsp_vector_copy_assign_array(dsp_vector_t* const dest, const real_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest->elements == src) { return false; }

    // Copy array
    memcpy(dest->elements, src, ARRAY_SIZE(dest->size));
    return true;
}

// Move
dsp_vector_t* dsp_vector_create_move(dsp_vector_t* const other) {
    if (other == NULL) { return NULL; }

    // Allocate space for a new matrix
    dsp_vector_t* const vec = NEW_VECTOR();
    if (vec == NULL) { return NULL; }

    // Move elements array pointer
    vec->size = other->size;
    vec->elements = other->elements;

    // Invalidate other elements array pointer
    other->size = 0;
    other->elements = NULL;
    
    // Return
    return vec;
}
bool dsp_vector_move_assign(dsp_vector_t* const dest, dsp_vector_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    dsp_vector_release_internal_array(dest);

    // Move elements array pointer
    dest->size = src->size;
    dest->elements = src->elements;

    // Invalidate other elements array pointer
    src->size = 0;
    src->elements = NULL;
    
    // Return
    return true;
}

// Swap
bool dsp_vector_swap(dsp_vector_t* const a, dsp_vector_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    if (a == b) { return false; }
    dsp_vector_t temp = *a;
    *a = *b;
    *b = temp;
    return true;
}

// Destroy
bool dsp_vector_allocate_internal_array(dsp_vector_t* const vec, const size_t size) {
    if (vec == NULL) { return false; }
    if (vec->elements != NULL) { return false; }
    if (size == 0) { return false; }

    // Allocate space for the elements
    vec->elements = NEW_ARRAY(size);
    if (vec->elements != NULL) { 
        vec->size = size;
        return true; 
    }
    else { 
        return false; 
    }
}
bool dsp_vector_release_internal_array(dsp_vector_t* const vec) {
    if (vec == NULL) { return false; }
    if (vec->elements != NULL) {
        free(vec->elements);
        vec->elements = NULL;
        vec->size = 0;
    }
    return true;
}
bool dsp_vector_destroy(dsp_vector_t* const vec) {
    if (vec == NULL) { return false; }
    if (vec->elements != NULL) { free(vec->elements); }
    free(vec);
    return true;
}

// Check same size
bool dsp_vector_has_same_size(dsp_vector_t* const a, dsp_vector_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    return (a->size == b->size);
}

// Set
bool dsp_vector_set_to_zero(dsp_vector_t* const vec) {
    if (vec == NULL) { return false; }
    if (vec->elements == NULL) { return false; }
    if (vec->size == 0) { return false; }

    memset(vec->elements, 0, ARRAY_SIZE(vec->size));
    return true;
}
bool dsp_vector_set_to_unit(dsp_vector_t* const vec, const size_t dim_index) {
    if (vec == NULL) { return false; }
    if (vec->elements == NULL) { return false; }
    if (vec->size == 0) { return false; }
    if (dim_index >= vec->size) { return false; }

    memset(vec->elements, 0, ARRAY_SIZE(vec->size));
    vec->elements[dim_index] = 1;
    return true;
}
bool dsp_vector_normalize(dsp_vector_t* const vec) {
    if (vec == NULL) { return false; }
    if (vec->size == 0) { return false; }

    // Calculate length of the vector
    const real_t vec_length = dsp_vector_length(vec);
    if (vec_length == 0) { return false; }

    // Normalize elements
    for (size_t k = 0; k < vec->size; ++k) {
        vec->elements[k] = vec->elements[k] / vec_length;
    }

    return true;
}
bool dsp_vector_set_to_normalized(dsp_vector_t* const vec_n, const dsp_vector_t* const vec) {
    if (vec_n == NULL || vec == NULL) { return false; }
    if (vec->size == 0) { return false; }
    if (vec->size != vec_n->size) { return false; }

    // Calculate length of the vector
    const real_t vec_length = dsp_vector_length(vec);
    if (vec_length == 0) { return false; }

    // Normalize elements
    for (size_t k = 0; k < vec->size; ++k) {
        vec_n->elements[k] = vec->elements[k] / vec_length;
    }

    return true;
}
bool dsp_vector_set_to_projection(dsp_vector_t* const vec, const dsp_vector_t* const a, const dsp_vector_t* const b) {
    if (vec == NULL || a == NULL || b == NULL) { return false; }
    if (vec->size != a->size) { return false; }
    if (a->size != b->size) { return false; }
    if (b->size == 0) { return false; }

    // Calculate dot product of 'a' and 'b'
    const real_t dot_ab = dsp_vector_dot_product(a, b);

    // Calculate length of 'b'
    const real_t b_length_squared = dsp_vector_length_squared(b);
    if (b_length_squared == 0) { return false; }

    // Calculate scaling factor
    const real_t alpha = dot_ab / b_length_squared;

    // Project elements
    // v = ((a * b) / |b|^2) * b
    for (size_t k = 0; k < vec->size; ++k) {
        vec->elements[k] = alpha * b->elements[k];
    }
    return true;
}
bool dsp_vector_set_to_cross_product(dsp_vector_t* const vec, const dsp_vector_t* const a, const dsp_vector_t* const b) { // Cross product (for R^3 vectors only)
    if (vec == NULL || a == NULL || b == NULL) { return false; }
    if (vec->size != 3) { return false; }
    if (a->size != 3) { return false; }
    if (b->size != 3) { return false; }

    // Set the elements
    vec->elements[0] = a->elements[1] * b->elements[2] - a->elements[2] * b->elements[1];
    vec->elements[1] = a->elements[2] * b->elements[0] - a->elements[0] * b->elements[2];
    vec->elements[2] = a->elements[0] * b->elements[1] - a->elements[1] * b->elements[0];
    return true;
}


// to real_t
size_t dsp_vector_size(const dsp_vector_t* const vec) {
    if (vec == NULL) { return 0; }
    return vec->size;
}
real_t dsp_vector_length_squared(const dsp_vector_t* const vec) {
    return dsp_vector_dot_product(vec, vec);
}
real_t dsp_vector_length(const dsp_vector_t* const vec) {
    return sqrtf(dsp_vector_length_squared(vec));
}
real_t dsp_vector_dot_product(const dsp_vector_t* const a, const dsp_vector_t* const b) {
    if (a == NULL || b == NULL) { return 0; }
    if (a->size != b->size) { return 0; }
    if (a->size == 0) { return 0; }

    real_t acc = a->elements[0] * b->elements[0];  
    for (size_t k = 1; k < a->size; ++k) {
        acc += a->elements[k] * b->elements[k];
    }
    return acc;
}
real_t dsp_vector_cosphi(const dsp_vector_t* const a, const dsp_vector_t* const b) {
    if (a == NULL || b == NULL) { return 0; }
    if (a == b) { return 1; }
    if (a->size != b->size) { return 0; }
    if (a->size == 0) { return 0; }

    const real_t dot_ab = dsp_vector_dot_product(a, b);
    const real_t len_a = dsp_vector_length(a);
    const real_t len_b = dsp_vector_length(b);

    if (len_a == 0 || len_b == 0) { return 1; }
    else { return dot_ab / (len_a * len_b); }
}
real_t dsp_vector_angle(const dsp_vector_t* const a, const dsp_vector_t* const b) {
    return acosf(dsp_vector_cosphi(a, b));
}
real_t dsp_vector_project(const dsp_vector_t* const a, const dsp_vector_t* const b) {
    if (a == NULL || b == NULL) { return 0; }
    if (a->size != b->size) { return 0; }
    if (a->size == 0) { return 0; }

    // Calculate dot product of 'a' and 'b'
    const real_t dot_ab = dsp_vector_dot_product(a, b);

    // Calculate length of 'b'
    const real_t b_length = dsp_vector_length(b);
    if (b_length == 0) { return 0; }

    // Calculate length of 'a' in direction of 'b'
    return dot_ab / b_length;
}


// Arithmetic
bool dsp_vector_add(dsp_vector_t* const result, const dsp_vector_t* const a, const dsp_vector_t* const b) {
    if (result == NULL || a == NULL || b == NULL) { return false; }
    if (result->size == 0) { return false; }
    if (result->size != a->size) { return false; }
    if (a->size != b->size) { return false; }

    for (size_t k = 0; k < a->size; ++k) {
        result->elements[k] = a->elements[k] + b->elements[k];
    }

    return true;
}
bool dsp_vector_add_and_assign(dsp_vector_t* const a, const dsp_vector_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    if (a->size == 0) { return false; }
    if (a->size != b->size) { return false; }

    for (size_t k = 0; k < a->size; ++k) {
        a->elements[k] += b->elements[k];
    }

    return true;
}
bool dsp_vector_subtract(dsp_vector_t* const result, const dsp_vector_t* const a, const dsp_vector_t* const b) {
    if (result == NULL || a == NULL || b == NULL) { return false; }
    if (result->size == 0) { return false; }
    if (result->size != a->size) { return false; }
    if (a->size != b->size) { return false; }

    for (size_t k = 0; k < a->size; ++k) {
        result->elements[k] = a->elements[k] - b->elements[k];
    }

    return true;
}
bool dsp_vector_subtract_and_assign(dsp_vector_t* const a, const dsp_vector_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    if (a->size == 0) { return false; }
    if (a->size != b->size) { return false; }

    for (size_t k = 0; k < a->size; ++k) {
        a->elements[k] -= b->elements[k];
    }

    return true;
}


// Arithmetic
bool dsp_vector_multiply_by_scalar(dsp_vector_t* const result, const dsp_vector_t* const vec, const real_t scalar) {
    if (result == NULL || vec == NULL) { return false; }
    if (result->size != vec->size) { return false; }
    if (vec->size == 0) { return false; }

    for (size_t k = 0; k < vec->size; ++k) {
        result->elements[k] = vec->elements[k] * scalar;
    }

    return true;
}
bool dsp_vector_multiply_by_scalar_and_assign(dsp_vector_t* const vec, const real_t scalar) {
    if (vec == NULL) { return false; }
    if (vec->size == 0) { return false; }

    for (size_t k = 0; k < vec->size; ++k) {
        vec->elements[k] *= scalar;
    }

    return true;
}
bool dsp_vector_divide_by_scalar(dsp_vector_t* const result, const dsp_vector_t* const vec, const real_t scalar) {
    if (result == NULL || vec == NULL) { return false; }
    if (result->size != vec->size) { return false; }
    if (vec->size == 0) { return false; }
    if (scalar == 0) { return false; }

    for (size_t k = 0; k < vec->size; ++k) {
        result->elements[k] = vec->elements[k] / scalar;
    }

    return true;
}
bool dsp_vector_divide_by_scalar_and_assign(dsp_vector_t* const vec, const real_t scalar) {
    if (vec == NULL) { return false; }
    if (vec->size == 0) { return false; }
    if (scalar == 0) { return false; }

    for (size_t k = 0; k < vec->size; ++k) {
        vec->elements[k] /= scalar;
    }

    return true;
}



// Concat
bool dsp_vector_concat(dsp_vector_t* const result, const dsp_vector_t* const T, const dsp_vector_t* const B) {
    if (result == NULL || T == NULL || B == NULL) { return false; }
    if (result->size != T->size + B->size) { return false; }

    memcpy(result->elements, T->elements, ARRAY_SIZE(T->size));
    memcpy(&(result->elements[T->size]), B->elements, ARRAY_SIZE(B->size));
    return true;
}

// Split
bool dsp_vector_split(dsp_vector_t* const T, dsp_vector_t* const B, const dsp_vector_t* const vec, const size_t split_after_element_num) {
    if (T == NULL || B == NULL || vec == NULL) { return false; }
    if (split_after_element_num == 0 || split_after_element_num >= vec->size) { return false; }
    if (T->size != split_after_element_num) {return false; }
    if (B->size != (vec->size - split_after_element_num)) { return false; }

    memcpy(T->elements, vec->elements, ARRAY_SIZE(T->size));
    memcpy(B->elements, &(vec->elements[T->size]), ARRAY_SIZE(B->size));
    return true;
}




// Convolution and polynomial multiplication
// w = conv(u,v) returns the convolution of vectors u and v. 
// If u and v are vectors of polynomial coefficients, 
// convolving them is equivalent to multiplying the two polynomials.
// w->size == u->size + v->size - 1;
bool dsp_vector_conv(dsp_vector_t* const w, const dsp_vector_t* const u, const dsp_vector_t* const v) {
    if (u == NULL || v == NULL || w == NULL) { return false; }
    if (u->size == 0 || v->size == 0) { return false; }
    const size_t conv_size = u->size + v->size - 1;
    if (w->size != conv_size) { return false; }

    size_t start, end;
    for (size_t	k = 0; k < conv_size; k++) {

        // Determine start and end
        start = (k < u->size ? 0 : k - (u->size - 1));
        end = (k < v->size ? k : (v->size - 1));

        // Init
        w->elements[k] = 0;

        // Calculate k-th element
        for (size_t j = start; j <= end; ++j) {
            w->elements[k] += v->elements[j] * u->elements[k-j];
        }
    }

    return true;
}

// Deconvolution and polynomial division
// [q,r] = deconv(u,v) deconvolves a vector v out of a vector u using long division, 
// and returns the quotient q and remainder r such that u = conv(v,q) + r. 
// If u and v are vectors of polynomial coefficients, then deconvolving them is equivalent to 
// dividing the polynomial represented by u by the polynomial represented by v.
// q->size == (u->size - v->size + 1)
// r->size == u->size
bool dsp_vector_deconv(dsp_vector_t* const q, dsp_vector_t* const r, const dsp_vector_t* const u, const dsp_vector_t* const v) {
    if (u == NULL || v == NULL || q == NULL || r == NULL) { return false; }
    if (u->size == 0 || v->size == 0) { return false; }
    if (r->size != u->size) { return false; } 
    if (v->elements[0] == 0) { return false; }
    if (u->size < v->size) {
        dsp_vector_set_to_zero(q);
        dsp_vector_copy_assign(r, u);
        return true;
    }
    if (q->size != (u->size - v->size + 1)) { return false;}

    // We copy 'u' into 'r' at the start
    dsp_vector_copy_assign(r, u);

    // Deconvolution 
    for (size_t k = 0; k < q->size; ++k) {

        // Divide
        q->elements[k] = r->elements[k] / v->elements[0];

        // Subtract
        r->elements[k] = 0;
        for (size_t j = 1; j < v->size; ++j) {
            r->elements[k+j] -= q->elements[k] * v->elements[j]; 
        }
    }
    return true;
}













// Create a matrix from a givan vector
dsp_matrix_t* dsp_matrix_create_from_vector(const size_t rows, const size_t columns, const dsp_vector_t* vec) {
    if (vec == NULL) { return NULL; }
    if (rows * columns != vec->size) { return NULL; }
    return dsp_matrix_create_from_array(rows, columns, vec->elements);
}

// Arithmetic
bool dsp_matrix_vector_multiply(dsp_vector_t* const result, const dsp_matrix_t* const mat, const dsp_vector_t* const vec) {
    if (result == NULL || mat == NULL || vec == NULL) { return false; }
    if (result->size != mat->rows) { return false; }
    if (mat->columns != vec->size) { return false; }

    for (size_t k = 0; k < result->size; ++k) {

        VECTOR_ELEMENT(result, k) = MATRIX_ELEMENT(mat, k, 0) * VECTOR_ELEMENT(vec, 0);
        for (size_t j = 1; j < vec->size; ++j) {
            VECTOR_ELEMENT(result, k) += MATRIX_ELEMENT(mat, k, j) * VECTOR_ELEMENT(vec, j);
        }
    }

    return true;
}


// Arithmetic
bool dsp_matrix_vector_multiply_and_add_to_vector(dsp_vector_t* const acc, const dsp_matrix_t* const mat, const dsp_vector_t* const vec) {
    if (acc == NULL || mat == NULL || vec == NULL) { return false; }
    if (acc->size != mat->rows) { return false; }
    if (mat->columns != vec->size) { return false; }

    for (size_t k = 0; k < acc->size; ++k) {
        for (size_t j = 0; j < vec->size; ++j) {
            VECTOR_ELEMENT(acc, k) += MATRIX_ELEMENT(mat, k, j) * VECTOR_ELEMENT(vec, j);
        }
    }

    return true;
}
bool dsp_matrix_vector_multiply_and_subtract_from_vector(dsp_vector_t* const acc, const dsp_matrix_t* const mat, const dsp_vector_t* const vec) {
    if (acc == NULL || mat == NULL || vec == NULL) { return false; }
    if (acc->size != mat->rows) { return false; }
    if (mat->columns != vec->size) { return false; }

    for (size_t k = 0; k < acc->size; ++k) {
        for (size_t j = 0; j < vec->size; ++j) {
            VECTOR_ELEMENT(acc, k) -= MATRIX_ELEMENT(mat, k, j) * VECTOR_ELEMENT(vec, j);
        }
    }

    return true;
}


// Arithmetic
bool dsp_matrix_assign_row_vector(dsp_matrix_t* const mat, const size_t row_index, const dsp_vector_t* const row_vec) {
    if (mat == NULL) { return false; }
    if (row_index >= mat->rows) { return false; }
    if (row_vec != NULL && row_vec->size != mat->columns) { return false; }

    for (size_t k = 0; k < mat->columns; ++k) {
        MATRIX_ELEMENT(mat, row_index, k) = (row_vec != NULL ? VECTOR_ELEMENT(row_vec, k) : 0);
    }
    return true;
}
bool dsp_matrix_assign_column_vector(dsp_matrix_t* const mat, const size_t column_index, const dsp_vector_t* const colum_vec) {
    if (mat == NULL) { return false; }
    if (column_index >= mat->columns) { return false; }
    if (colum_vec != NULL && colum_vec->size != mat->rows) { return false; }

    for (size_t k = 0; k < mat->rows; ++k) {
        MATRIX_ELEMENT(mat, k, column_index) = (colum_vec != NULL ? VECTOR_ELEMENT(colum_vec, k) : 0);
    }
    return true;
}
bool dsp_matrix_add_on_row_vector(dsp_matrix_t* const mat, const size_t row_index, const dsp_vector_t* const row_vec) {
    if (mat == NULL) { return false; }
    if (row_index >= mat->rows) { return false; }
    if (row_vec == NULL) { return false; }
    if (row_vec->size != mat->columns) { return false; }

    for (size_t k = 0; k < mat->columns; ++k) {
        MATRIX_ELEMENT(mat, row_index, k) += VECTOR_ELEMENT(row_vec, k);
    }
    return true;
}
bool dsp_matrix_add_on_column_vector(dsp_matrix_t* const mat, const size_t column_index, const dsp_vector_t* const colum_vec) {
    if (mat == NULL) { return false; }
    if (column_index >= mat->columns) { return false; }
    if (colum_vec == NULL) { return false; }
    if (colum_vec->size != mat->rows) { return false; }

    for (size_t k = 0; k < mat->rows; ++k) {
        MATRIX_ELEMENT(mat, k, column_index) += VECTOR_ELEMENT(colum_vec, k);
    }
    return true;
}
bool dsp_matrix_subract_of_row_vector(dsp_matrix_t* const mat, const size_t row_index, const dsp_vector_t* const row_vec) {
    if (mat == NULL) { return false; }
    if (row_index >= mat->rows) { return false; }
    if (row_vec == NULL) { return false; }
    if (row_vec->size != mat->columns) { return false; }

    for (size_t k = 0; k < mat->columns; ++k) {
        MATRIX_ELEMENT(mat, row_index, k) -= VECTOR_ELEMENT(row_vec, k);
    }
    return true;
}
bool dsp_matrix_subract_of_column_vector(dsp_matrix_t* const mat, const size_t column_index, const dsp_vector_t* const colum_vec) {
    if (mat == NULL) { return false; }
    if (column_index >= mat->columns) { return false; }
    if (colum_vec == NULL) { return false; }
    if (colum_vec->size != mat->rows) { return false; }

    for (size_t k = 0; k < mat->rows; ++k) {
        MATRIX_ELEMENT(mat, k, column_index) -= VECTOR_ELEMENT(colum_vec, k);
    }
    return true;
}


// Solve system of linear equations
// A * x = b
// x == pinv(A) * b
bool dsp_vector_solve_lse(dsp_vector_t* const x, const dsp_matrix_t* const A, const dsp_vector_t* const b) {
    if (x == NULL || A == NULL || b == NULL) { return false; }
    if (A->columns != x->size) { return false; }
    if (b->size != A->rows) { return false; }

    if (A->rows == A->columns) {

        // Copy b into x
        dsp_vector_copy_assign(x, b);

        // Let B borrow the elements from x
        //dsp_matrix_t B = {
        //    .rows = x->size,
        //    .columns = 1,
        //    .elements = x->elements
        //};

        // Let B borrow the elements from x
        dsp_matrix_t B = { x->size, 1, x->elements};

        // create a copy of A
        dsp_matrix_t* const A_copy =  dsp_matrix_create_copy(A);
        if (A_copy == NULL) { return false; }
        
        if (dsp_matrix_gauss_jordan(A_copy, &B)) {

            // Now B contains the solution to A+x = b
            // And since B borrowed its elements from x,
            // x also contains the solution
            dsp_matrix_destroy(A_copy);
            // B wasn't created by a call to 'dsp_vector_create_xxx()'
            // so we don't need to destroy it.
            return true;
        }
        else {
            // No sulution exists.
            dsp_matrix_destroy(A_copy);
            // B wasn't created by a call to 'dsp_vector_create_xxx()'
            // so we don't need to destroy it.
            return false;
        }
    }
    else {

        // Calculate the pseudo inverse matrix
        dsp_matrix_t* const pinv_A = dsp_matrix_create_pinv(A);
        if (pinv_A == NULL) { return false; }

        // calculate the solution
        dsp_matrix_vector_multiply(x, pinv_A, b);
        dsp_matrix_destroy(pinv_A);
        return true;
    }
}

bool dsp_solve(real_t* const x, const size_t x_size, const real_t* const A, const real_t* const b, const size_t b_size) {
    if (x == NULL || A == NULL || b == NULL) { return false; }
    if (x_size == 0 || b_size == 0) { return false; }

    //dsp_vector_t X = {.size = x_size, .elements = x};
    //const dsp_matrix_t M = {.rows = b_size, .columns = x_size, .elements = (real_t* const) A};
    //const dsp_vector_t B = {.size = b_size, .elements = (real_t* const) b};

    dsp_vector_t X = {x_size, x};
    const dsp_matrix_t M = {b_size, x_size, (real_t* const) A};
    const dsp_vector_t B = {b_size, (real_t* const) b};
    return dsp_vector_solve_lse(&X, &M, &B);
}
