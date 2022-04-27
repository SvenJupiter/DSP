#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset, memmove
#include "DSP/Math/Matrix.h"

#define MATRIX_SIZE sizeof(dsp_matrix_t)
#define NEW_MATRIX() malloc(MATRIX_SIZE)

#define REAL_SIZE sizeof(real_t)
#define ARRAY_SIZE(rows, columns) ((rows) * (columns) * REAL_SIZE)
#define NEW_ARRAY(rows, columns) malloc(ARRAY_SIZE(rows, columns))
#define ELEMENT(mat, row_index, column_index) ((mat)->elements[(row_index) * mat->columns + (column_index)])


// Create a matrix of size 'rows * columns' but don't initilize its elements
dsp_matrix_t* dsp_matrix_create(const size_t rows, const size_t columns) {
    if (rows * columns == 0) { return NULL; }

    // Allocate space for a new matrix
    dsp_matrix_t* const mat = NEW_MATRIX();
    if (mat != NULL) {
        mat->elements = NULL;

        // Allocate space for the elements
        if (dsp_matrix_allocate_internal_array(mat, rows, columns)) {
            return mat; 
        }
        else { 
            free(mat); 
            return NULL; 
        }
    }
    else {
        return NULL;
    }
}

// Create a matrix from a givan array (if 'elements' is NULL, set all elements to 0)
dsp_matrix_t* dsp_matrix_create_from_array(const size_t rows, const size_t columns, const real_t* elements) {

    // Create a new matrix
    dsp_matrix_t* const mat = dsp_matrix_create(rows, columns);
    if (mat != NULL) {

        // Initilize its elements
        if (elements != NULL) { memcpy(mat->elements, elements, ARRAY_SIZE(rows, columns)); }
        else { memset(mat->elements, 0, ARRAY_SIZE(rows, columns)); }
    }
    return mat;
}

// Create
dsp_matrix_t* dsp_matrix_create_zeros(const size_t rows, const size_t columns) {
    return dsp_matrix_create_from_array(rows, columns, NULL);
}
dsp_matrix_t* dsp_matrix_create_eye(const size_t rows, const size_t columns) {
    dsp_matrix_t* mat = dsp_matrix_create(rows, columns);
    if (mat == NULL) { return NULL; }

    dsp_matrix_set_to_eye(mat);
    return mat;
}
dsp_matrix_t* dsp_matrix_create_diag(const real_t* const elements, const size_t rows, const size_t columns) {
    dsp_matrix_t* mat = dsp_matrix_create(rows, columns);
    if (mat == NULL) { return NULL; }

    for (size_t i = 0; i < mat->rows; ++i) {
        for (size_t j = 0; j < mat->columns; ++j) {
            ELEMENT(mat, j, i) = (i == j ? (elements != NULL ? elements[i] : 1) : 0);
        }
    }

    return mat;
}
dsp_matrix_t* dsp_matrix_create_vandermonde(const size_t order, const real_t* const x_values, const size_t size) {
    dsp_matrix_t* mat = dsp_matrix_create(size, order+1);
    if (mat == NULL) { return NULL; }

    for (size_t i = 0; i < size; ++i) {

        ELEMENT(mat, i, 0) = 1;
        ELEMENT(mat, i, 1) = x_values[i];
        for (size_t j = 2; j <= order; ++j) {
            ELEMENT(mat, i, j) = ELEMENT(mat, i, j-1) * x_values[i];
        }
    }

    return mat;
}

// Create
dsp_matrix_t* dsp_matrix_create_product(const dsp_matrix_t* A, const dsp_matrix_t* B) {
    if (A == NULL || B == NULL) { return NULL; }
    if (A->columns != B->rows) { return NULL; }


    // Create a new Matrix
    dsp_matrix_t* const C = dsp_matrix_create(A->rows, B->columns);
    if (C == NULL) { return NULL; }

    // Calculate the inverse of the matrix
    if (dsp_matrix_multiply(C, A, B)) {
        return C;
    }
    else {
        dsp_matrix_destroy(C);
        return NULL;
    }  
}
dsp_matrix_t* dsp_matrix_create_transpose(const dsp_matrix_t* const mat) {
    if (mat == NULL) { return NULL; }

    // Create a new Matrix
    dsp_matrix_t* const mat_t = dsp_matrix_create(mat->columns, mat->rows);
    if (mat_t == NULL) { return NULL; }

    // Calculate the inverse of the matrix
    if (dsp_matrix_transpose(mat_t, mat)) {
        return mat_t;
    }
    else {
        dsp_matrix_destroy(mat_t);
        return NULL;
    }  
}
dsp_matrix_t* dsp_matrix_create_inv(const dsp_matrix_t* const mat) {
    if (mat == NULL) { return NULL; }
    if (mat->columns !=  mat->rows) { return NULL; }

    // Create a new Matrix
    dsp_matrix_t* const inv_mat = dsp_matrix_create(mat->columns, mat->rows);
    if (inv_mat == NULL) { return NULL; }

    // Calculate the inverse of the matrix
    if (dsp_matrix_inv(inv_mat, mat)) {
        return inv_mat;
    }
    else {
        dsp_matrix_destroy(inv_mat);
        return NULL;
    }   
}
dsp_matrix_t* dsp_matrix_create_pinv(const dsp_matrix_t* const mat) {
    if (mat == NULL) { return NULL; }

    // Create a new Matrix
    dsp_matrix_t* const inv_mat = dsp_matrix_create(mat->columns, mat->rows);
    if (inv_mat == NULL) { return NULL; }

    // Calculate the inverse of the matrix
    if (dsp_matrix_pinv(inv_mat, mat)) {
        return inv_mat;
    }
    else {
        dsp_matrix_destroy(inv_mat);
        return NULL;
    }
}


// Concat
dsp_matrix_t* dsp_matrix_create_horzcat(const dsp_matrix_t* const L, const dsp_matrix_t* const R) {
    if (L == NULL || R == NULL) { return NULL; }
    if (L->rows != R->rows) { return NULL; }

    // Create a new Matrix
    dsp_matrix_t* const mat = dsp_matrix_create(L->rows, L->rows + L->columns);
    if (mat == NULL) { return NULL; }

    if (dsp_matrix_horzcat(mat, L, R)) {
        return mat;
    }
    else {
        dsp_matrix_destroy(mat);
        return NULL;
    }
}
dsp_matrix_t* dsp_matrix_create_vertcat(const dsp_matrix_t* const T, const dsp_matrix_t* const B) {
    if (T == NULL || B == NULL) { return NULL; }
    if (T->columns != B->columns) { return NULL; }

    // Create a new Matrix
    dsp_matrix_t* const mat = dsp_matrix_create(T->rows + B->rows, T->columns);
    if (mat == NULL) { return NULL; }

    if (dsp_matrix_vertcat(mat, T, B)) {
        return mat;
    }
    else {
        dsp_matrix_destroy(mat);
        return NULL;
    }
}
dsp_matrix_t* dsp_matrix_create_blockcat(const dsp_matrix_t* const TL, const dsp_matrix_t* const TR, const dsp_matrix_t* const BL, const dsp_matrix_t* const BR) {

    if (TL == NULL && TR == NULL && BL == NULL && BR == NULL) { return NULL; }
    else if (BL == NULL && BR == NULL) { return dsp_matrix_create_horzcat(TL, TR); }
    else if (TR == NULL && BR == NULL) { return dsp_matrix_create_vertcat(TL, BL); }
    else if (TL == NULL && TR == NULL) { return dsp_matrix_create_horzcat(BL, BR); }
    else if (TL == NULL && BL == NULL) { return dsp_matrix_create_vertcat(TR, BR); }
    else {
        // Size of the new Matrix
        const size_t rows = (TL != NULL ? TL->rows : TR->rows) + (BL != NULL ? BL->rows : BR->rows);
        const size_t columns = (TL != NULL ? TL->columns : BL->columns) + (TR != NULL ? TR->columns : BR->columns);

        // Create a new Matrix
        dsp_matrix_t* const mat = dsp_matrix_create(rows, columns);
        if (mat == NULL) { return NULL; }

        if (dsp_matrix_blockcat(mat, TL, TR, BL, BR)) {
            return mat;
        }
        else {
            dsp_matrix_destroy(mat);
            return NULL;
        }
    }
}


// Create
dsp_matrix_t* dsp_matrix_create_submatrix(const dsp_matrix_t* const mat, const size_t row_index, const size_t column_index) {
    if (mat == NULL) { return NULL; }

    dsp_matrix_t* const sub = dsp_matrix_create(mat->rows-1, mat->columns-1);
    if (sub == NULL) { return NULL; }

    size_t k = 0, l;
    for (size_t i = 0; i < mat->rows; ++i) {
        if (i == row_index) { continue; }

        l = 0;
        for (size_t j = 0; j < mat->columns; ++j) {
            if (j == column_index) { continue; }
                ELEMENT(sub, k, l) = ELEMENT(mat, i, j);
            ++l;
        }
        ++k;
    }
    return sub;
}

// Copy
dsp_matrix_t* dsp_matrix_create_copy(const dsp_matrix_t* const other) {
    if (other == NULL) { return NULL; }
    return dsp_matrix_create_from_array(other->rows, other->columns, other->elements);
}
bool dsp_matrix_copy_assign(dsp_matrix_t* const dest, const dsp_matrix_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    if (dest->rows != src->rows) { return false; }
    if (dest->elements == src->elements) { return false; }
    if (dest->columns != src->columns) { return false; }
    if (src->rows * src->columns == 0) { return false; }

    // Copy array
    memcpy(dest->elements, src->elements, ARRAY_SIZE(src->rows, src->columns));
    return true;
}
bool dsp_matrix_copy_assign_array(dsp_matrix_t* const dest, const real_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest->elements == src) { return false; }

    // Copy array
    memcpy(dest->elements, src, ARRAY_SIZE(dest->rows, dest->columns));
    return true;
}

// Move
dsp_matrix_t* dsp_matrix_create_move(dsp_matrix_t* const other) {
    if (other == NULL) { return NULL; }

    // Allocate space for a new matrix
    dsp_matrix_t* mat = NEW_MATRIX();
    if (mat == NULL) { return NULL; }

    // Move elements array pointer
    mat->rows = other->rows;
    mat->columns = other->columns;
    mat->elements = other->elements;

    // Invalidate other elements array pointer
    other->rows = 0;
    other->columns = 0;
    other->elements = NULL;
    
    // Return
    return mat;
}
bool dsp_matrix_move_assign(dsp_matrix_t* const dest, dsp_matrix_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    dsp_matrix_release_internal_array(dest);

    // Move elements array pointer
    dest->rows = src->rows;
    dest->columns = src->columns;
    dest->elements = src->elements;

    // Invalidate other elements array pointer
    src->rows = 0;
    src->columns = 0;
    src->elements = NULL;
    
    // Return
    return true;
}

// Swap
bool dsp_matrix_swap(dsp_matrix_t* const a, dsp_matrix_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    if (a == b) { return false; }
    dsp_matrix_t temp = *a;
    *a = *b;
    *b = temp;
    return true;
}

// Destroy
bool dsp_matrix_allocate_internal_array(dsp_matrix_t* const mat, const size_t rows, const size_t columns) {
    if (mat == NULL) { return false; }
    if (mat->elements != NULL) { return false; }
    if (rows * columns == 0) { return false; }

    // Allocate space for the elements
    mat->elements = NEW_ARRAY(rows, columns);
    if (mat->elements != NULL) { 
        mat->rows = rows;
        mat->columns = columns;
        return true; 
    }
    else { 
        return false; 
    }
}
bool dsp_matrix_release_internal_array(dsp_matrix_t* const mat) {
    if (mat == NULL) { return false; }
    if (mat->elements != NULL) {
        free(mat->elements);
        mat->elements = NULL;
        mat->rows = 0;
        mat->columns = 0;
    }
    return true;
}
bool dsp_matrix_destroy(dsp_matrix_t* const mat) {
    if (mat == NULL) { return false; }
    if (mat->elements != NULL) { free(mat->elements); }
    free(mat);
    return true;
}

// Check same size
bool dsp_matrix_has_same_size(dsp_matrix_t* const a, dsp_matrix_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    return ((a->rows == b->rows) && (a->columns == b->columns));
}

// Reshape
bool dsp_matrix_reshape(dsp_matrix_t* const mat, const size_t new_rows, const size_t new_columns) {
    if (mat == NULL) { return false; }
    if ((mat->rows * mat->columns) != (new_rows * new_columns)) { return false; }

    mat->rows = new_rows;
    mat->columns = new_columns;
    return true;
}

// Set
bool dsp_matrix_set_to_zero(dsp_matrix_t* const mat) {
    if (mat == NULL) { return false; }
    if (mat->rows * mat->columns == 0) { return 0; }

    memset(mat->elements, 0, ARRAY_SIZE(mat->rows, mat->columns));
    return true;
}
bool dsp_matrix_set_to_eye(dsp_matrix_t* const mat) {
    if (mat == NULL) { return false; }
    if (mat->rows * mat->columns == 0) { return false; }

    for (size_t i = 0; i < mat->rows; ++i) {
        for (size_t j = 0; j < mat->columns; ++j) {
            ELEMENT(mat, j, i) = (i == j ? 1 : 0);
        }
    }
    return true;
}




// Arithmetic
bool dsp_matrix_add(dsp_matrix_t* result, const dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2) {
    if (result == NULL || mat1 == NULL || mat2 == NULL) { return false; }
    if (result->rows != mat1->rows) { return false; }
    if (result->columns != mat1->columns) { return false; }
    if (mat2->rows != mat1->rows) { return false; }
    if (mat2->columns != mat1->columns) { return false; }

    for (size_t i = 0; i < result->rows; ++i) {
        for (size_t j = 0; j < result->columns; ++j) {
            ELEMENT(result, i, j) = ELEMENT(mat1, i, j) + ELEMENT(mat2, i, j);
        }
    }
    return true;
}
bool dsp_matrix_add_and_assign(dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2) {
    if (mat1 == NULL || mat2 == NULL) { return false; }
    if (mat2->rows != mat1->rows) { return false; }
    if (mat2->columns != mat1->columns) { return false; }

    for (size_t i = 0; i < mat1->rows; ++i) {
        for (size_t j = 0; j < mat1->columns; ++j) {
            ELEMENT(mat1, i, j) += ELEMENT(mat2, i, j);
        }
    }
    return true;
}
bool dsp_matrix_subtract(dsp_matrix_t* result, const dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2) {
    if (result == NULL || mat1 == NULL || mat2 == NULL) { return false; }
    if (result->rows != mat1->rows) { return false; }
    if (result->columns != mat1->columns) { return false; }
    if (mat2->rows != mat1->rows) { return false; }
    if (mat2->columns != mat1->columns) { return false; }

    for (size_t i = 0; i < result->rows; ++i) {
        for (size_t j = 0; j < result->columns; ++j) {
            ELEMENT(result, i, j) = ELEMENT(mat1, i, j) - ELEMENT(mat2, i, j);
        }
    }
    return true;
}
bool dsp_matrix_subtract_and_assign(dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2) {
    if (mat1 == NULL || mat2 == NULL) { return false; }
    if (mat2->rows != mat1->rows) { return false; }
    if (mat2->columns != mat1->columns) { return false; }

    for (size_t i = 0; i < mat1->rows; ++i) {
        for (size_t j = 0; j < mat1->columns; ++j) {
            ELEMENT(mat1, i, j) -= ELEMENT(mat2, i, j);
        }
    }
    return true;
}
bool dsp_matrix_multiply(dsp_matrix_t* result, const dsp_matrix_t* const mat1, const dsp_matrix_t* const mat2) {
    if (result == NULL || mat1 == NULL || mat2 == NULL) { return false; }
    if (result->rows != mat1->rows) {return false; }
    if (result->columns != mat2->columns) {return false; }
    if (mat1->columns != mat2->rows) {return false; }

    for (size_t i = 0; i < result->rows; ++i) {
        for (size_t j = 0; j < result->columns; ++j) {

            // Calculate element (i,j)
            ELEMENT(result, i, j) = ELEMENT(mat1, i, 0) * ELEMENT(mat2, 0, j);
            for (size_t k = 1; k < mat1->columns; k++) {
                ELEMENT(result, i, j) += ELEMENT(mat1, i, k) * ELEMENT(mat2, k, j);
            }

        }
    }
    return true;
}

// Arithmetic
bool dsp_matrix_multiply_by_scalar(dsp_matrix_t* result, const dsp_matrix_t* const mat, const real_t scalar) {
    if (result == NULL || mat == NULL) { return false; }
    if (result->rows != mat->rows) { return false; }
    if (result->columns != mat->columns) { return false; }

    for (size_t i = 0; i < mat->rows; ++i) {
        for (size_t j = 0; j < mat->columns; ++j) {
            ELEMENT(result, i, j) = ELEMENT(mat, i, j) * scalar;
        }
    }
    return true;
}
bool dsp_matrix_multiply_by_scalar_and_assign(dsp_matrix_t* mat, const real_t scalar) {
    if (mat == NULL) { return false; }

    for (size_t i = 0; i < mat->rows; ++i) {
        for (size_t j = 0; j < mat->columns; ++j) {
            ELEMENT(mat, i, j) *= scalar;
        }
    }
    return true;
}
bool dsp_matrix_divide_by_scalar(dsp_matrix_t* result, const dsp_matrix_t* const mat, const real_t scalar) {
    if (result == NULL || mat == NULL) { return false; }
    if (result->rows != mat->rows) { return false; }
    if (result->columns != mat->columns) { return false; }
    if (scalar == 0) { return false; }

    for (size_t i = 0; i < mat->rows; ++i) {
        for (size_t j = 0; j < mat->columns; ++j) {
            ELEMENT(result, i, j) = ELEMENT(mat, i, j) / scalar;
        }
    }
    return true;
}
bool dsp_matrix_divide_by_scalar_and_assign(dsp_matrix_t* mat, const real_t scalar) {
    if (mat == NULL) { return false; }
    if (scalar == 0) { return false; }

    for (size_t i = 0; i < mat->rows; ++i) {
        for (size_t j = 0; j < mat->columns; ++j) {
            ELEMENT(mat, i, j) /= scalar;
        }
    }
    return true;
}



// Determinante
real_t dsp_matrix_det(const dsp_matrix_t* const mat) {
    if (mat == NULL) { return 0; }

    // Copy mat
    dsp_matrix_t* A = dsp_matrix_create_copy(mat);
    if (A == NULL) { return 0;}

    // gauß
    const real_t det_A = dsp_matrix_gauss_jordan(A, NULL);

    // destroy copy
    dsp_matrix_destroy(A);

    // return
    return det_A;
}

// Transpose
bool dsp_matrix_transpose(dsp_matrix_t* const result, const dsp_matrix_t* const mat) {
    if (result == NULL || mat == NULL) { return false; }
    if (result->rows != mat->columns) { return false; }
    if (result->columns != mat->rows) { return false; }

    for (size_t i = 0; i < result->rows; ++i) {
        for (size_t j = 0; j < result->columns; ++j) {
            ELEMENT(result, i, j) = ELEMENT(mat, j, i);
        }
    }
    return true;
}

// Inverse
bool dsp_matrix_inv(dsp_matrix_t* const result, const dsp_matrix_t* const mat) {
    if (result == NULL || mat == NULL) { return false; }
    if (result->rows != mat->rows) { return false; }
    if (result->columns != mat->columns) { return false; }
    if (mat->rows != mat->columns) { return false; }

    // Create copy of mat
    dsp_matrix_t* A = dsp_matrix_create_copy(mat);
    if (A == NULL) { return false; }

    // Set to eye
    dsp_matrix_set_to_eye(result);

    // gauß
    const real_t det_A = dsp_matrix_gauss_jordan(A, result);

    // destroy copy
    dsp_matrix_destroy(A);

    // return
    return (det_A != 0);
}
bool dsp_matrix_pinv(dsp_matrix_t* const result, const dsp_matrix_t* const mat) {
    if (result == NULL || mat == NULL) { return false; }
    if (result->rows != mat->columns) { return false; }
    if (result->columns != mat->rows) { return false; }

    // Überbestimmt: ~C = ~(C^T * C) * C^T
    if (mat->rows > mat->columns) {

        // Transpose matrix
        dsp_matrix_t* mat_transposed = dsp_matrix_create_transpose(mat);
        if (mat_transposed == NULL) { return false; }

        // Calculate a square matrix
        dsp_matrix_t* mat_n = dsp_matrix_create_product(mat_transposed, mat);
        if (mat_n == NULL) { dsp_matrix_destroy(mat_transposed); return false; }

        // Inverse the square matrix
        dsp_matrix_t* inv_mat_n = dsp_matrix_create_inv(mat_n);
        if (inv_mat_n == NULL) { dsp_matrix_destroy(mat_n); dsp_matrix_destroy(mat_transposed); return false; }

        // Calculate the product
        dsp_matrix_multiply(result, inv_mat_n, mat_transposed);

        // Release
        dsp_matrix_destroy(inv_mat_n);
        dsp_matrix_destroy(mat_n);
        dsp_matrix_destroy(mat_transposed);
        return true;
        
    }
    // Unterbestimmt: ~C = C^T * ~(C * C^T)
    else if (mat->rows < mat->columns) {

        // Transpose matrix
        dsp_matrix_t* mat_transposed = dsp_matrix_create_transpose(mat);
        if (mat_transposed == NULL) { return false; }

        // Calculate a square matrix
        dsp_matrix_t* mat_n = dsp_matrix_create_product(mat, mat_transposed);
        if (mat_n == NULL) { dsp_matrix_destroy(mat_transposed); return false; }

        // Inverse the square matrix
        dsp_matrix_t* inv_mat_n = dsp_matrix_create_inv(mat_n);
        if (inv_mat_n == NULL) { dsp_matrix_destroy(mat_n); dsp_matrix_destroy(mat_transposed); return false; }

        // Calculate the product
        dsp_matrix_multiply(result, mat_transposed, inv_mat_n);

        // Release
        dsp_matrix_destroy(inv_mat_n);
        dsp_matrix_destroy(mat_n);
        dsp_matrix_destroy(mat_transposed);
        return true;

    }
    // row == columns -> pinv(C) == inv(C)
    else {
        return dsp_matrix_inv(result, mat);
    }
}



// Concat
bool dsp_matrix_horzcat(dsp_matrix_t* const result, const dsp_matrix_t* const L, const dsp_matrix_t* const R) {
    if (result == NULL || L == NULL || R == NULL) { return false; }
    if (L->rows != R->rows || R->rows != result->rows) { return false; }
    if (result->columns != L->columns + R->columns) { return false; }

    // Copy L
    for (size_t i = 0; i < L->rows; ++i) {
        for (size_t j = 0; j < L->columns; ++j) {
            ELEMENT(result, i, j) = ELEMENT(L, i, j);
        }
    }

    // Copy R
    for (size_t i = 0; i < R->rows; ++i) {
        for (size_t j = 0; j < R->columns; ++j) {
            ELEMENT(result, i, j + L->columns) = ELEMENT(R, i, j);
        }
    }

    return true;
}
bool dsp_matrix_vertcat(dsp_matrix_t* const result, const dsp_matrix_t* const T, const dsp_matrix_t* const B) {
    if (result == NULL || T == NULL || B == NULL) { return false; }
    if (T->columns != B->columns || B->columns != result->columns) { return false; }
    if (result->rows != T->rows + B->rows) { return false; }

    // Copy T
    memcpy(result->elements, T->elements, ARRAY_SIZE(T->rows, T->columns));

    // Copy B
    memcpy(&(result->elements[T->rows * T->columns]), B->elements, ARRAY_SIZE(B->rows, B->columns));

    return true;
}
bool dsp_matrix_blockcat(dsp_matrix_t* const result, const dsp_matrix_t* const TL, const dsp_matrix_t* const TR, const dsp_matrix_t* const BL, const dsp_matrix_t* const BR) {
    if (result == NULL) { return false; }
    else if (BL == NULL && BR == NULL) { return dsp_matrix_horzcat(result, TL, TR); }
    else if (TR == NULL && BR == NULL) { return dsp_matrix_vertcat(result, TL, BL); }
    else if (TL == NULL && TR == NULL) { return dsp_matrix_horzcat(result, BL, BR); }
    else if (TL == NULL && BL == NULL) { return dsp_matrix_vertcat(result, TR, BR); }
    else {
        if (TL == NULL && BR == NULL) {
            // result == [0, TR; BL, 0]
            // if (TR == NULL || BL == NULL) { return false; }
            if (result->rows != TR->rows + BL->rows) { return false; }
            if (result->columns != TR->columns + BL->columns) { return false; }


            // TL = 0
            for (size_t i = 0; i < TR->rows; ++i) {
                for (size_t j = 0; j < BL->columns; ++j) {
                    ELEMENT(result, i, j) = 0;
                }
            }


            // TR = TR
            for (size_t i = 0; i < TR->rows; ++i) {
                for (size_t j = 0; j < TR->columns; ++j) {
                    ELEMENT(result, i, j + BL->columns) = ELEMENT(TR, i, j);
                }
            }

            // BL = BL
            for (size_t i = 0; i < BL->rows; ++i) {
                for (size_t j = 0; j < BL->columns; ++j) {
                    ELEMENT(result, i + TR->rows, j) = ELEMENT(BL, i, j);
                }
            }

            // BR = 0
            for (size_t i = 0; i < BL->rows; ++i) {
                for (size_t j = 0; j < TR->columns; ++j) {
                    ELEMENT(result, i + TR->rows, j + BL->columns) = 0;
                }
            }

            // Return
            return true;
        }
        else if (TR == NULL && BL == NULL) {
            // result == [TL, 0; 0; BR]
            // if (TL == NULL || BR == NULL) { return false; }
            if (result->rows != TL->rows + BR->rows) { return false; }
            if (result->columns != TL->columns + BR->columns) { return false; }

            // TL = TL
            for (size_t i = 0; i < TL->rows; ++i) {
                for (size_t j = 0; j < TL->columns; ++j) {
                    ELEMENT(result, i, j) = ELEMENT(TL, i, j);
                }
            }

            // TR = 0
            for (size_t i = 0; i < TL->rows; ++i) {
                for (size_t j = 0; j < BR->columns; ++j) {
                    ELEMENT(result, i, j + TL->columns) = 0;
                }
            }

            // BL = 0
            for (size_t i = 0; i < BR->rows; ++i) {
                for (size_t j = 0; j < TL->columns; ++j) {
                    ELEMENT(result, i + TL->rows, j) = 0;
                }
            }

            // BR = BR
            for (size_t i = 0; i < BR->rows; ++i) {
                for (size_t j = 0; j < BR->columns; ++j) {
                    ELEMENT(result, i + TL->rows, j + TL->columns) = ELEMENT(BR, i, j);
                }
            }

            // Return
            return true;
        }
        else {
            if (TL == NULL) {
                // result == [0, TR; BL, BR]
                if (TR->columns != BR->columns) { return false; }
                if (BL->rows != BR->rows) { return false; }
                if (result->rows != TR->rows + BR->rows) { return false; }
                if (result->columns != BL->columns + BR->columns) { return false; }

                // TL = 0
                for (size_t i = 0; i < TR->rows; ++i) {
                    for (size_t j = 0; j < BL->columns; ++j) {
                        ELEMENT(result, i, j) = 0;
                    }
                }

                // TR = TR
                for (size_t i = 0; i < TR->rows; ++i) {
                    for (size_t j = 0; j < TR->columns; ++j) {
                        ELEMENT(result, i, j + BL->columns) = ELEMENT(TR, i, j);
                    }
                }

                // BL = BL
                for (size_t i = 0; i < BL->rows; ++i) {
                    for (size_t j = 0; j < BL->columns; ++j) {
                        ELEMENT(result, i + TR->rows, j) = ELEMENT(BL, i, j);
                    }
                }

                // BR = BR
                for (size_t i = 0; i < BR->rows; ++i) {
                    for (size_t j = 0; j < BR->columns; ++j) {
                        ELEMENT(result, i + TR->rows, j + BL->columns) = ELEMENT(BR, i, j);
                    }
                }

                // Return
                return true;
            }
            else if (TR == NULL) {
                // result == [TL, 0; BL, BR]
                if (TL->columns != BL->columns) { return false; }
                if (BL->rows != BR->rows) { return false; }
                if (result->rows != TL->rows + BL->rows) { return false; }
                if (result->columns != BL->columns + BR->columns) { return false; }

                // TL = TL
                for (size_t i = 0; i < TL->rows; ++i) {
                    for (size_t j = 0; j < TL->columns; ++j) {
                        ELEMENT(result, i, j) = ELEMENT(TL, i, j);
                    }
                }

                // TR = 0
                for (size_t i = 0; i < TL->rows; ++i) {
                    for (size_t j = 0; j < BR->columns; ++j) {
                        ELEMENT(result, i, j + TL->columns) = 0;
                    }
                }

                // BL = BL
                for (size_t i = 0; i < BL->rows; ++i) {
                    for (size_t j = 0; j < BL->columns; ++j) {
                        ELEMENT(result, i + TL->rows, j) = ELEMENT(BL, i, j);
                    }
                }

                // BR = BR
                for (size_t i = 0; i < BR->rows; ++i) {
                    for (size_t j = 0; j < BR->columns; ++j) {
                        ELEMENT(result, i + TL->rows, j + TL->columns) = ELEMENT(BR, i, j);
                    }
                }

                // Return
                return true;

            }
            else if (BL == NULL) {
                // result == [TL, TR; 0; BR]
                if (TL->rows != TR->rows) { return false; }
                if (TR->columns != BR->columns) { return false; }
                if (result->rows != TR->rows + BR->rows) { return false; }
                if (result->columns != TL->columns + TR->columns) { return false; }

                // TL = TL
                for (size_t i = 0; i < TL->rows; ++i) {
                    for (size_t j = 0; j < TL->columns; ++j) {
                        ELEMENT(result, i, j) = ELEMENT(TL, i, j);
                    }
                }

                // TR = TR
                for (size_t i = 0; i < TR->rows; ++i) {
                    for (size_t j = 0; j < TR->columns; ++j) {
                        ELEMENT(result, i, j + TL->columns) = ELEMENT(TR, i, j);
                    }
                }

                // BL = 0
                for (size_t i = 0; i < BR->rows; ++i) {
                    for (size_t j = 0; j < TL->columns; ++j) {
                        ELEMENT(result, i + TL->rows, j) = 0;
                    }
                }

                // BR = BR
                for (size_t i = 0; i < BR->rows; ++i) {
                    for (size_t j = 0; j < BR->columns; ++j) {
                        ELEMENT(result, i + TL->rows, j + TL->columns) = ELEMENT(BR, i, j);
                    }
                }

                // Return
                return true;

            }
            else if (BR == NULL) {
                // result == [TL, TR; BL; 0]
                if (TL->rows != TR->rows) { return false; }
                if (TL->columns != BL->columns) { return false; }
                if (result->rows != TL->rows + BL->rows) { return false; }
                if (result->columns != TL->columns + TR->columns) { return false; }


                // TL = TL
                for (size_t i = 0; i < TL->rows; ++i) {
                    for (size_t j = 0; j < TL->columns; ++j) {
                        ELEMENT(result, i, j) = ELEMENT(TL, i, j);
                    }
                }

                // TR = TR
                for (size_t i = 0; i < TR->rows; ++i) {
                    for (size_t j = 0; j < TR->columns; ++j) {
                        ELEMENT(result, i, j + TL->columns) = ELEMENT(TR, i, j);
                    }
                }

                // BL = BL
                for (size_t i = 0; i < BL->rows; ++i) {
                    for (size_t j = 0; j < BL->columns; ++j) {
                        ELEMENT(result, i + TL->rows, j) = ELEMENT(BL, i, j);
                    }
                }

                // BR = 0
                for (size_t i = 0; i < BL->rows; ++i) {
                    for (size_t j = 0; j < TR->columns; ++j) {
                        ELEMENT(result, i + TL->rows, j + TL->columns) = 0;
                    }
                }

                // Return
                return true;

            }
            else {
                // full concat
                // result == [TL, TR; BL, BR]
                if (TL->rows != TR->rows) { return false; }
                if (TL->columns != BL->columns) { return false; }
                if (BL->rows != BR->rows) { return false; }
                if (TR->columns != BR->columns) { return false; }
                if (result->rows != TL->rows + BL->rows) { return false; }
                if (result->columns != TL->columns + TR->columns) { return false; }


                // TL = TL
                for (size_t i = 0; i < TL->rows; ++i) {
                    for (size_t j = 0; j < TL->columns; ++j) {
                        ELEMENT(result, i, j) = ELEMENT(TL, i, j);
                    }
                }

                // TR = TR
                for (size_t i = 0; i < TR->rows; ++i) {
                    for (size_t j = 0; j < TR->columns; ++j) {
                        ELEMENT(result, i, j + TL->columns) = ELEMENT(TR, i, j);
                    }
                }

                // BL = BL
                for (size_t i = 0; i < BL->rows; ++i) {
                    for (size_t j = 0; j < BL->columns; ++j) {
                        ELEMENT(result, i + TL->rows, j) = ELEMENT(BL, i, j);
                    }
                }

                // BR = BR
                for (size_t i = 0; i < BR->rows; ++i) {
                    for (size_t j = 0; j < BR->columns; ++j) {
                        ELEMENT(result, i + TL->rows, j + TL->columns) = ELEMENT(BR, i, j);
                    }
                }

                // Return
                return true;
            }
        }
    }
}

// Split 
bool dsp_matrix_horzsplit(dsp_matrix_t* const L, dsp_matrix_t* const R, const dsp_matrix_t* const mat, const size_t split_after_column_num) {
    if (L == NULL || R == NULL || mat == NULL) { return false; }
    if (split_after_column_num == 0 || split_after_column_num >= mat->columns) { return false; }
    if (L->columns != split_after_column_num ) { return false;}
    if (R->columns != (mat->columns - split_after_column_num)) { return false;}
    if (L->rows != mat->rows || R->rows != mat->rows) { return false; }

    // L
    for (size_t  i = 0; i < L->rows; ++i) {
        for (size_t j = 0; j < L->columns; ++ j) {
            ELEMENT(L, i, j) = ELEMENT(mat, i, j);
        }
    }

    // R
    for (size_t  i = 0; i < R->rows; ++i) {
        for (size_t j = 0; j < R->columns; ++ j) {
            ELEMENT(R, i, j) = ELEMENT(mat, i, j + split_after_column_num);
        }
    }

    return true;
}
bool dsp_matrix_vertsplit(dsp_matrix_t* const T, dsp_matrix_t* const B, const dsp_matrix_t* const mat, const size_t split_after_row_num) {
    if (T == NULL || B == NULL || mat == NULL) { return false; }
    if (split_after_row_num == 0 || split_after_row_num >= mat->rows) { return false; }
    if (T->rows != split_after_row_num) { return false; }
    if (B->rows != (mat->rows - split_after_row_num)) { return false; }
    if (T->columns != mat->columns || B->columns != mat->columns) { return false; }

    // // T
    // for (size_t  i = 0; i < T->rows; ++i) {
    //     for (size_t j = 0; j < T->columns; ++ j) {
    //         ELEMENT(T, i, j) = ELEMENT(mat, i, j);
    //     }
    // }

    // // B
    // for (size_t  i = 0; i < B->rows; ++i) {
    //     for (size_t j = 0; j < B->columns; ++ j) {
    //         ELEMENT(B, i, j) = ELEMENT(mat, i + split_after_row_num, j);
    //     }
    // }

    memcpy(T->elements, mat->elements, ARRAY_SIZE(T->rows, T->columns));
    memcpy(B->elements, &(mat->elements[T->rows * T->columns]), ARRAY_SIZE(B->rows, B->columns));
    return true;
}
bool dsp_matrix_blocksplit(dsp_matrix_t* const TL, dsp_matrix_t* const TR, dsp_matrix_t* const BL, dsp_matrix_t* const BR, const dsp_matrix_t* const mat, const size_t split_after_row_num, const size_t split_after_column_num) {
    if (TL == NULL || TR == NULL || BL == NULL || BR == NULL || mat == NULL) { return false; }
    if (split_after_column_num == 0 || split_after_column_num >= mat->columns) { return false; }
    if (split_after_row_num == 0 || split_after_row_num >= mat->rows) { return false; }
    if (TL->rows != split_after_row_num || TL->columns != split_after_column_num) { return NULL; }
    if (TR->rows != split_after_row_num || TR->columns != (mat->columns - split_after_column_num)) { return false; }
    if (BL->rows != (mat->rows - split_after_row_num) || BL->columns != split_after_column_num) { return false; }
    if (BR->rows != (mat->rows - split_after_row_num) || BR->columns != (mat->columns - split_after_column_num)) { return false; }


    // TL
    for (size_t  i = 0; i < TL->rows; ++i) {
        for (size_t j = 0; j < TL->columns; ++ j) {
            ELEMENT(TL, i, j) = ELEMENT(mat, i, j);
        }
    }

    // BL
    for (size_t  i = 0; i < BL->rows; ++i) {
        for (size_t j = 0; j < BL->columns; ++ j) {
            ELEMENT(BL, i, j) = ELEMENT(mat, i + split_after_row_num, j);
        }
    }

    // TR
    for (size_t  i = 0; i < TR->rows; ++i) {
        for (size_t j = 0; j < TR->columns; ++ j) {
            ELEMENT(TR, i, j) = ELEMENT(mat, i, j + split_after_column_num);
        }
    }

    // BR
    for (size_t  i = 0; i < BR->rows; ++i) {
        for (size_t j = 0; j < BR->columns; ++ j) {
            ELEMENT(BR, i, j) = ELEMENT(mat, i + split_after_row_num, j + split_after_column_num);
        }
    }

    // Return
    return true;
}


// Gauß-Jordan
real_t dsp_matrix_gauss_jordan(dsp_matrix_t* const A, dsp_matrix_t* const B) {
    if (A == NULL) { return 0; }
    if (A->rows == 0 || A->rows != A->columns) { return 0; } 

    // Vorbereitung
    const size_t size = A->columns;
    real_t alpha = 1, beta;


    // Vorwärts-Elimination
    for (size_t k = 0; k < size - 1; ++k) {

        // Wenn das Element auf der Hauptdiagonalen Null ist
        if (ELEMENT(A, k, k) == 0) {

            // Suche Zeile
            for (size_t j = k + 1; j < size; ++j) {

                // Wenn Element nicht Null
                if (ELEMENT(A, j, k) != 0) {

                    // Tausche Zeilen
                    dsp_matrix_swap_rows(A, k, j);
                    if (B != NULL) { dsp_matrix_swap_rows(B, k, j); }

                    // Beim Zeilen tauschen ändert sich das Vorzeichen der Determinante
                    alpha *= (-1); 
                    break;
                }
            }

            // Nicht lösbar
            if (ELEMENT(A, k, k) == 0) { return 0; } 
        }

        // Nullen unterhalb der Haupdiagonalen erzeugen
        for (size_t j = k + 1; j < size; ++j) {
            beta = (-1) * (ELEMENT(A, j, k) / ELEMENT(A, k, k));

            // Null in Spalte k Zeile j erzeugen
            ELEMENT(A, j, k) = 0;
            dsp_matrix_modify_row(A, j, k, k+1, size-1, beta);
            if (B != NULL) { dsp_matrix_modify_row(B, j, k, 0, B->columns-1, beta); }
        }
    }

    // Determinante berechnen
    for (size_t k = 0; k < size; ++k) { alpha *= ELEMENT(A, k, k); }

    // Falls nur die Determinante berechnet werden sollte 
    // oder die Determinante Null ist, stoppe hier.
    if (alpha == 0 || B == NULL) { return alpha; }


    // Rückwärts-Elimination
    for (size_t k = size-1; k > 0; --k) {

        // Nullen oberhalb der Haupdiagonalen erzeugen
        for (size_t j = k; j > 0; /*--j*/) {

            // Do this at the start of the loop
            --j;

            // Null in Spalte k Zeile j erzeugen
            beta = (-1) * (ELEMENT(A, j, k) / ELEMENT(A, k, k));
            ELEMENT(A, j, k) = 0;
            // dsp_matrix_modify_row(A, j, k, k, size-1, beta);
            dsp_matrix_modify_row(B, j, k, 0, B->columns-1, beta);
        }
    }

    // Elemente auf der Hauptdiagonalen zu 1 umformen
    for (size_t k = 0; k < size; ++k) {
        dsp_matrix_scale_down_row(B, k, 0, B->columns-1, ELEMENT(A, k, k));
        ELEMENT(A, k, k) = 1;
    }

    // Wir sind fertig.
    // A enthält jetzt die Einheitsmatrix
    // und B die Lösung für das LGS bzw. die inverse Matrix
    return alpha; 
}
bool dsp_matrix_swap_rows(dsp_matrix_t* const mat, const size_t row_a_index, const size_t row_b_index) {
    if (mat == NULL) { return false; }
    if (row_a_index >= mat->rows) { return false; }
    if (row_b_index >= mat->rows) { return false; }

    real_t temp;
    for (size_t k = 0; k < mat->columns; ++k) {
        temp = ELEMENT(mat, row_a_index, k);
        ELEMENT(mat, row_a_index, k) = ELEMENT(mat, row_b_index, k);
        ELEMENT(mat, row_b_index, k) = temp;
    }
    return true;
}
bool dsp_matrix_swap_columns(dsp_matrix_t* const mat, const size_t column_a_index, const size_t column_b_index) {
    if (mat == NULL) { return false; }
    if (column_a_index >= mat->columns) { return false; }
    if (column_b_index >= mat->columns) { return false; }

    real_t temp;
    for (size_t k = 0; k < mat->rows; ++k) {
        temp = ELEMENT(mat, k, column_a_index);
        ELEMENT(mat, k, column_a_index) = ELEMENT(mat, k, column_b_index);
        ELEMENT(mat, k, column_b_index) = temp;
    }
    return true;
}
bool dsp_matrix_scale_up_row(dsp_matrix_t* const mat, const size_t row_index, const size_t column_a_index, const size_t column_b_index, const real_t s) {
    if (mat == NULL) { return false; }
    if (row_index >= mat->rows) { return false; }
    if (column_a_index >= mat->columns) { return false; }
    if (column_b_index >= mat->columns) { return false; }
    if (column_a_index > column_b_index) { return false; }

    for (size_t k = column_a_index; k <= column_b_index; ++k) {
        ELEMENT(mat, row_index, k) *= s;
    }
    return true;
}
bool dsp_matrix_scale_up_column(dsp_matrix_t* const mat, const size_t column_index, const size_t row_a_index, const size_t row_b_index, const real_t s) {
    if (mat == NULL) { return false; }
    if (column_index >= mat->columns) { return false; }
    if (row_a_index >= mat->rows) { return false; }
    if (row_b_index >= mat->rows) { return false; }
    if (row_a_index > row_b_index) { return false; }

    for (size_t k = row_a_index; k <= row_b_index; ++k) {
        ELEMENT(mat, k, column_index) *= s;
    }
    return true;
}
bool dsp_matrix_scale_down_row(dsp_matrix_t* const mat, const size_t row_index, const size_t column_a_index, const size_t column_b_index, const real_t s) {
    if (mat == NULL) { return false; }
    if (row_index >= mat->rows) { return false; }
    if (column_a_index >= mat->columns) { return false; }
    if (column_b_index >= mat->columns) { return false; }
    if (column_a_index > column_b_index) { return false; }

    for (size_t k = column_a_index; k <= column_b_index; ++k) {
        ELEMENT(mat, row_index, k) /= s;
    }
    return true;
}
bool dsp_matrix_scale_down_column(dsp_matrix_t* const mat, const size_t column_index, const size_t row_a_index, const size_t row_b_index, const real_t s) {
    if (mat == NULL) { return false; }
    if (column_index >= mat->columns) { return false; }
    if (row_a_index >= mat->rows) { return false; }
    if (row_b_index >= mat->rows) { return false; }
    if (row_a_index > row_b_index) { return false; }

    for (size_t k = row_a_index; k <= row_b_index; ++k) {
        ELEMENT(mat, k, column_index) /= s;
    }
    return true;
}
bool dsp_matrix_modify_row(dsp_matrix_t* const mat, const size_t row_a_index, const size_t row_b_index, const size_t column_a_index, const size_t column_b_index, const real_t s) {
    if (mat == NULL) { return false; }
    if (row_a_index >= mat->rows) { return false; }
    if (row_b_index >= mat->rows) { return false; }
    if (column_a_index >= mat->columns) { return false; }
    if (column_b_index >= mat->columns) { return false; }
    if (column_a_index > column_b_index) { return false; }

    for (size_t k = column_a_index; k <= column_b_index; ++k) {
        ELEMENT(mat, row_a_index, k) += s * ELEMENT(mat, row_b_index, k);
    }
    return true;
}
bool dsp_matrix_modify_column(dsp_matrix_t* const mat, const size_t column_a_index, const size_t column_b_index, const size_t row_a_index, const size_t row_b_index, const real_t s) {
    if (mat == NULL) { return false; }
    if (column_a_index >= mat->columns) { return false; }
    if (column_b_index >= mat->columns) { return false; }
    if (row_a_index >= mat->rows) { return false; }
    if (row_b_index >= mat->rows) { return false; }
    if (row_a_index > row_b_index) { return false; }

    for (size_t k = row_a_index; k <= row_b_index; ++k) {
        ELEMENT(mat, k, column_a_index) += s * ELEMENT(mat, k, column_b_index);
    }
    return true;
}
