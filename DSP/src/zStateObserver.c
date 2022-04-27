#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset, memmove
#include "DSP/Discrete/zStateObserver.h"

#define ZSO_SIZE sizeof(dsp_zso_t)
#define NEW_ZSO() malloc(ZSO_SIZE)

#define ARRAY_ELEMEMT(array, index) ((array)[(index)])
#define VECTOR_ELEMENT(vec, index) ((vec)->elements[(index)])
#define MATRIX_ELEMENT(mat, row_index, column_index) ((mat)->elements[(row_index) * mat->columns + (column_index)])


// Create state observer but don't initilize its internal arrrays
dsp_zso_t* dsp_zso_create(const size_t nx,const size_t nu, const size_t ny) {
    if (nx == 0 || nu == 0 || ny == 0) { return NULL; }

    // Allocate Space for a new StateObserver struct
    dsp_zso_t* const zso = NEW_ZSO();
    if (zso == NULL) { return NULL; }

    // Initilize
    zso->A = NULL;
    zso->B = NULL;
    zso->C = NULL;
    zso->D = NULL;
    zso->L = NULL;
    zso->xh = NULL;
    zso->xn = NULL;
    zso->yh = NULL;
    zso->e = NULL;

    if (dsp_zso_allocate_internal_arrays(zso, nx, nu, ny)) {
        return zso;
    }
    else {
        free(zso);
        return NULL;
    }
}

// Create from arrays
dsp_zso_t* dsp_zso_create_from_arrays(const size_t nx, const size_t nu, const size_t ny,
    const real_t* const a, const real_t* const b,
    const real_t* const c, const real_t* const d,
    const real_t* const l, const real_t* const x0) {

    if (a == NULL || b == NULL || l == NULL) { return NULL; }
    if (nx == 0 || nu == 0 || ny == 0) { return NULL; }

    // Allocate Space for a new StateObserver struct
    dsp_zso_t* const zso = NEW_ZSO();
    if (zso == NULL) { return NULL; }

    // Allocate space for internal arrays
    zso->A = dsp_matrix_create_from_array(nx, nx, a);
    zso->B = dsp_matrix_create_from_array(nx, nu, b);
    zso->C = ((c != NULL) ? dsp_matrix_create_from_array(ny, nx, c) : dsp_matrix_create_eye(ny, nx));
    zso->D = ((d != NULL) ? dsp_matrix_create_from_array(ny, nu, d) : dsp_matrix_create_zeros(ny, nu));
    zso->L = dsp_matrix_create_from_array(nx, ny, l);
    zso->xh = dsp_vector_create_from_array(nx, x0);
    zso->xn = dsp_vector_create_from_array(nx, x0);
    zso->yh = dsp_vector_create(ny);
    zso->e = dsp_vector_create(ny);

    // Check
    if (zso->A == NULL || zso->B == NULL || \
        zso->C == NULL || zso->D == NULL || \
        zso->L == NULL || \
        zso->xh == NULL || zso->xn == NULL || \
        zso->yh == NULL || zso->e == NULL) {

        dsp_matrix_destroy(zso->A);
        dsp_matrix_destroy(zso->B);
        dsp_matrix_destroy(zso->C);
        dsp_matrix_destroy(zso->D);
        dsp_matrix_destroy(zso->L);
        dsp_vector_destroy(zso->xh);
        dsp_vector_destroy(zso->xn);
        dsp_vector_destroy(zso->yh);
        dsp_vector_destroy(zso->e);

        free(zso);
        return NULL;
    }
    else {
        return zso;
    }
}

// Create from matrices
dsp_zso_t* dsp_zso_create_from_matrices(
    const dsp_matrix_t* const A, const dsp_matrix_t* const B,
    const dsp_matrix_t* const C, const dsp_matrix_t* const D,
    const dsp_matrix_t* const L, const real_t* const x0) {

    if (A == NULL || B == NULL || L == NULL) { return NULL; }
    const size_t nx = A->rows;
    const size_t nu = B->columns;
    const size_t ny = L->columns;

    return dsp_zso_create_from_arrays(nx, nu, ny,
        A->elements, B->elements,
        (C != NULL ? C->elements : NULL),
        (D != NULL ? D->elements : NULL),
        L->elements, x0
    );
}

// Create from states space object
dsp_zso_t* dsp_zso_create_from_zss(const dsp_zss_t* const zss, const real_t* const l, const real_t* const x0) {
    if (zss == NULL || l == NULL) { return NULL; }

    const size_t nx = zss->A->rows;
    const size_t nu = zss->B->columns;
    const size_t ny = zss->C->rows;

    return dsp_zso_create_from_arrays(nx, nu, ny,
        zss->A->elements, zss->B->elements,
        zss->C->elements, zss->D->elements,
        l, x0
    );
}


// Copy
dsp_zso_t* dsp_zso_create_copy(const dsp_zso_t* const other) {
    if (other == NULL) { return false; }
    dsp_zso_t* const zso = dsp_zso_create_from_matrices(other->A, other->B, other->C, other->D, other->L, other->xn->elements);
    if (zso != NULL) {
        if (dsp_vector_copy_assign(zso->xh, other->xh)) {
            return zso;
        }
        else {
            dsp_zso_destroy(zso);
            return NULL;
        }
    }
    else {
        return NULL;
    }
}
bool dsp_zso_copy_assign(dsp_zso_t* const dest, const dsp_zso_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    if (!dsp_matrix_has_same_size(dest->A, src->A)) { return false; }
    if (!dsp_matrix_has_same_size(dest->B, src->B)) { return false; }
    if (!dsp_matrix_has_same_size(dest->C, src->C)) { return false; }
    if (!dsp_matrix_has_same_size(dest->D, src->D)) { return false; }
    if (!dsp_matrix_has_same_size(dest->L, src->L)) { return false; }
    if (!dsp_vector_has_same_size(dest->xh, src->xh)) { return false; }
    if (!dsp_vector_has_same_size(dest->xn, src->xn)) { return false; }
    if (!dsp_vector_has_same_size(dest->yh, src->yh)) { return false; }
    if (!dsp_vector_has_same_size(dest->e, src->e)) { return false; }

    // Copy
    dsp_matrix_copy_assign(dest->A, src->A);
    dsp_matrix_copy_assign(dest->B, src->B);
    dsp_matrix_copy_assign(dest->C, src->C);
    dsp_matrix_copy_assign(dest->D, src->D);
    dsp_matrix_copy_assign(dest->L, src->L);
    dsp_vector_copy_assign(dest->xh, src->xh);
    dsp_vector_copy_assign(dest->xn, src->xn);
    dsp_vector_copy_assign(dest->yh, src->yh);
    dsp_vector_copy_assign(dest->e, src->e);
    return true;
}

// Move
dsp_zso_t* dsp_zso_create_move(dsp_zso_t* const other) {
    if (other == NULL) { return NULL; }

    // Allocate Space for a new StateSpace struct
    dsp_zso_t* const zso = NEW_ZSO();
    if (zso == NULL) { return NULL; }

    // Move elements array pointer
    zso->A = other->A;
    zso->B = other->B;
    zso->C = other->C;
    zso->D = other->D;
    zso->L = other->L;
    zso->xh = other->xh;
    zso->xn = other->xn;
    zso->yh = other->yh;
    zso->e = other->e;

    // Invalidate other elements array pointer
    other->A = NULL;
    other->B = NULL;
    other->C = NULL;
    other->D = NULL;
    other->L = NULL;
    other->xh = NULL;
    other->xn = NULL;
    other->yh = NULL;
    other->e = NULL;

    return zso;
}
bool dsp_zso_move_assign(dsp_zso_t* const dest, dsp_zso_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    dsp_zso_release_internal_arrays(dest);

    // Move elements array pointer
    dest->A = src->A;
    dest->B = src->B;
    dest->C = src->C;
    dest->D = src->D;
    dest->L = src->L;
    dest->xh = src->xh;
    dest->xn = src->xn;
    dest->yh = src->yh;
    dest->e = src->e;

    // Invalidate other elements array pointer
    src->A = NULL;
    src->B = NULL;
    src->C = NULL;
    src->D = NULL;
    src->L = NULL;
    src->xh = NULL;
    src->xn = NULL;
    src->yh = NULL;
    src->e = NULL;

    return true;
}

// Swap
bool dsp_zso_swap(dsp_zso_t* const a, dsp_zso_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    if (a == b) { return false; }
    dsp_zso_t temp = *a;
    *a = *b;
    *b = temp;
    return true;
}

// Destroy
bool dsp_zso_allocate_internal_arrays(dsp_zso_t* const zso, const size_t nx, const size_t nu, const size_t ny) {
    if (zso == NULL) { return false; }
    if(zso->A != NULL) { return false; }
    if(zso->B != NULL) { return false; }
    if(zso->C != NULL) { return false; }
    if(zso->D != NULL) { return false; }
    if(zso->L != NULL) { return false; }
    if(zso->xh != NULL) { return false; }
    if(zso->xn != NULL) { return false; }
    if(zso->yh != NULL) { return false; }
    if(zso->e != NULL) { return false; }
    if (nx == 0 || nu == 0 || ny == 0) { return false; }

    // Allocate
    zso->A = dsp_matrix_create(nx, nx);
    zso->B = dsp_matrix_create(nx, nu);
    zso->C = dsp_matrix_create(ny, nx);
    zso->D = dsp_matrix_create(ny, nu);
    zso->L = dsp_matrix_create(nx, ny);
    zso->xh = dsp_vector_create(nx);
    zso->xn = dsp_vector_create(nx);
    zso->yh = dsp_vector_create(ny);
    zso->e = dsp_vector_create(ny);

    // Check
    if (zso->A == NULL || zso->B == NULL || \
        zso->C == NULL || zso->D == NULL || \
        zso->L == NULL || \
        zso->xh == NULL || zso->xn == NULL || \
        zso->yh == NULL || zso->e == NULL) {

        dsp_matrix_destroy(zso->A);
        dsp_matrix_destroy(zso->B);
        dsp_matrix_destroy(zso->C);
        dsp_matrix_destroy(zso->D);
        dsp_matrix_destroy(zso->L);
        dsp_vector_destroy(zso->xh);
        dsp_vector_destroy(zso->xn);
        dsp_vector_destroy(zso->yh);
        dsp_vector_destroy(zso->e);

        return false;
    }
    else {
        return true;
    }

}
bool dsp_zso_release_internal_arrays(dsp_zso_t* const zso) {
    if (zso == NULL) { return false; }

    dsp_matrix_destroy(zso->A);
    dsp_matrix_destroy(zso->B);
    dsp_matrix_destroy(zso->C);
    dsp_matrix_destroy(zso->D);
    dsp_matrix_destroy(zso->L);
    dsp_vector_destroy(zso->xh);
    dsp_vector_destroy(zso->xn);
    dsp_vector_destroy(zso->yh);
    dsp_vector_destroy(zso->e);

    zso->A = NULL;
    zso->B = NULL;
    zso->C = NULL;
    zso->D = NULL;
    zso->L = NULL;
    zso->xh = NULL;
    zso->xn = NULL;
    zso->yh = NULL;
    zso->e = NULL;
    return true;
}
bool dsp_zso_destroy(dsp_zso_t* const zso) {
    if (zso == NULL) { return NULL; }
    dsp_zso_release_internal_arrays(zso);
    free(zso);
    return true;
}

// Set state
bool dsp_zso_set_state_to(dsp_zso_t* const zso, const dsp_vector_t* const x0) {
    if (zso == NULL) { return false; }
    if (x0 != NULL) {
        return \
            dsp_vector_copy_assign(zso->xn, x0) && \
            dsp_vector_copy_assign(zso->xh, x0);
    }
    else { 
        return dsp_zso_reset(zso);
    }
}
bool dsp_zso_set_state(dsp_zso_t* const zso, const real_t* const x0) {
    if (zso == NULL) { return false; }
    if (x0 != NULL) {
        return \
            dsp_vector_copy_assign_array(zso->xn, x0) && \
            dsp_vector_copy_assign_array(zso->xh, x0);
    }
    else { 
        return dsp_zso_reset(zso);
    }
}

// Reset state
bool dsp_zso_reset(dsp_zso_t* const zso) {
    if (zso == NULL) { return false; }
    return \
        dsp_vector_set_to_zero(zso->xn) && \
        dsp_vector_set_to_zero(zso->xh);
}


// Update:
// y^[n] = C * x^[n] + D * u[n]
// e[k] = y[k] - y^[k]
// x^[n+1] = A * x^[n] + B * u[n] + L * e[k]
bool dsp_zso_vector_update_state(dsp_zso_t* const zso, const dsp_vector_t* const u, const dsp_vector_t* const y) {
    if (zso == NULL || u == NULL || y == NULL) { return NULL; }

    // swap states
    dsp_vector_swap(zso->xh, zso->xn);

    // y^[n] = C * x^[n] + D * u[n]
    dsp_matrix_vector_multiply(zso->yh, zso->C, zso->xh);
    dsp_matrix_vector_multiply_and_add_to_vector(zso->yh, zso->D, u);

    // e[k] = y[k] - y^[k]
    dsp_vector_subtract(zso->e, y, zso->yh);

    // x^[n+1] = A * x^[n] + B * u[n] + L * e[k]
    dsp_matrix_vector_multiply(zso->xn, zso->A, zso->xh);
    dsp_matrix_vector_multiply_and_add_to_vector(zso->xn, zso->B, u);
    dsp_matrix_vector_multiply_and_add_to_vector(zso->xn, zso->L, zso->e);
    return true;
}
bool dsp_zso_vector_output(dsp_zso_t* const zso, dsp_vector_t* const xh) {
    if (zso == NULL) { return false; }
    return dsp_vector_copy_assign(xh, zso->xh);
}
bool dsp_zso_vector_update(dsp_zso_t* const zso, const dsp_vector_t* const u, const dsp_vector_t* const y, dsp_vector_t* const xh) {
    return (dsp_zso_vector_update_state(zso, u, y) && dsp_zso_vector_output(zso, xh));
}

// Update
bool dsp_zso_update_state(dsp_zso_t* const zso, const real_t* const u, const real_t* const y) {
    if (zso == NULL) { return false; }
    const dsp_vector_t U = {.size = zso->B->columns, .elements = (real_t* const) u}; // u will not be modified
    const dsp_vector_t Y = {.size = zso->C->rows, .elements = (real_t* const) y}; // y will not be modified
    return dsp_zso_vector_update_state(zso, &U, &Y);
}
bool dsp_zso_output(dsp_zso_t* const zso, real_t* const xh) {
    if (zso == NULL) { return false; }
    if (zso->xh->elements == xh) { return false; }
    memcpy(xh, zso->xh->elements, zso->xh->size * sizeof(real_t));
    return true;
}
bool dsp_zso_update(dsp_zso_t* const zso, const real_t* const u, const real_t* const y, real_t* const xh) {
    return (dsp_zso_update_state(zso, u, y) && dsp_zso_output(zso, xh)); 
}



