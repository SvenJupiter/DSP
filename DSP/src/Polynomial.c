
#include <stdlib.h> // malloc, realloc, free
#include <string.h> // memcpy, memset, memmove
#include "DSP/Math/Polynomial.h"
#include "DSP/Math/Vector.h" // dsp_vector_solve

#define POLYNOMIAL_SIZE sizeof(dsp_poly_t)
#define NEW_POLY() malloc(POLYNOMIAL_SIZE)

#define REAL_SIZE sizeof(real_t)
#define ARRAY_SIZE(order) (((order)+1) * REAL_SIZE)
#define NEW_ARRAY(order) malloc(ARRAY_SIZE(order))

// Create a polynomial of size 'order' but don't initilize its coeffs
dsp_poly_t* dsp_polynomial_create(const size_t order) {

    // Allocate space for a new polynomial
    dsp_poly_t* const p = NEW_POLY();
    if (p != NULL) {

        // Allocate space for the coeffs
        p->a = NEW_ARRAY(order);
        if (p->a != NULL) { p->order = order; return p; }
        else { free(p); return NULL; }
    }
    else {
        return NULL;
    }
}

// Create a polynomial from a givan array (if 'coeffs' is NULL, set all coeffs to 0)
dsp_poly_t* dsp_polynomial_create_from_array(const size_t order, const real_t* const coeffs) {

    // Create a new polynomial
    dsp_poly_t* p = dsp_polynomial_create(order);
    if (p != NULL) {

        // Initilize its coeffs
        if (coeffs != NULL) { memcpy(p->a, coeffs, ARRAY_SIZE(order)); }
        else { memset(p->a, 0, ARRAY_SIZE(order)); }
    }
    return p;
}

// Create a polynomial for a given set of points
dsp_poly_t* dsp_polynomial_create_fit(const size_t order, const real_t* const x, const real_t* const y, const size_t size) {
    if (x == NULL || y == NULL || size == 0) { return NULL; }

    // Create a new polynomial
    dsp_poly_t* const p = dsp_polynomial_create(order);
    if (p == NULL) { return NULL; }

    // fit
    if (dsp_polynomial_fit(p, x, y, size)) {
        return p;
    }
    else {
        dsp_polynomial_destroy(p);
        return NULL;
    }
}

// Copy
dsp_poly_t* dsp_polynomial_create_copy(const dsp_poly_t* const other) {
    if (other == NULL) { return NULL; }
    return dsp_polynomial_create_from_array(other->order, other->a);
}
bool dsp_polynomial_copy_assign(dsp_poly_t* const dest, const dsp_poly_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    if (dest->a == src->a) { return false; }
    if (dest->order != src->order) { return false; }

    // Copy array
    memcpy(dest->a, src->a, ARRAY_SIZE(src->order));
    return true;
}
bool dsp_polynomial_copy_assign_array(dsp_poly_t* const dest, const real_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest->a == src) { return false; }

    // Copy array
    memcpy(dest->a, src, ARRAY_SIZE(dest->order));
    return true;
}
// Move
dsp_poly_t* dsp_polynomial_create_move(dsp_poly_t* const other) {
    if (other == NULL) { return NULL; }

    // Allocate space for a new polynomial
    dsp_poly_t* p = NEW_POLY();
    if (p == NULL) { return NULL; }

    // Move coeffs array pointer
    p->order = other->order;
    p->a = other->a;

    // Invalidate other coeffs array pointer
    other->order = 0;
    other->a = NULL;
    
    // Return
    return p;
}
bool dsp_polynomial_move_assign(dsp_poly_t* const dest, dsp_poly_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    dsp_polynomial_release_internal_array(dest);

    // Move coeffs array pointer
    dest->order = src->order;
    dest->a = src->a;

    // Invalidate other coeffs array pointer
    src->order = 0;
    src->a = NULL;
    
    // Return
    return true;
}

// Swap
bool dsp_polynomial_swap(dsp_poly_t* const a, dsp_poly_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    if (a == b) { return false; }
    dsp_poly_t temp = *a;
    *a = *b;
    *b = temp;
    return true;
}

// Destroy
bool dsp_polynomial_release_internal_array(dsp_poly_t* const p) {
    if (p == NULL) { return false; }
    if (p->a != NULL) {
        free(p->a);
        p->a = NULL;
        p->order = 0;
    }
    return true;
}
bool dsp_polynomial_destroy(dsp_poly_t* const p) {
    if (p == NULL) { return false; }
    dsp_polynomial_release_internal_array(p);
    free(p);
    return true;
}

// Check same size
bool dsp_polynomial_has_same_size(const dsp_poly_t* const a, const dsp_poly_t* const b) {
    if (a == NULL || b == NULL) { return false; }
    return (a->order == b->order);
}

// Arithmetic
bool dsp_polynomial_add(dsp_poly_t* const result, const dsp_poly_t* const p1, const dsp_poly_t* const p2) {
    if (result == NULL || p1 == NULL || p2 || NULL) { return false; }
    if (p1->order >= p2->order) {
        if (result->order < p1->order) { return false; }

        // Add
        for (size_t k = 0; k <= p2->order; ++k) { result->a[k] = p1->a[k] + p2->a[k]; }

        // Copy
        for (size_t k = p2->order + 1; k <= p1->order; ++k) { result->a[k] = p1->a[k]; }

        // Set to zerro
        for (size_t k = p1->order + 1; k <= result->order; ++k) { result->a[k] = 0; }

        // Return
        return true;
    }
    else {
        if (result->order < p2->order) { return false; }

        // Add
        for (size_t k = 0; k <= p1->order; ++k) { result->a[k] = p1->a[k] + p2->a[k]; }

        // Copy
        for (size_t k = p1->order + 1; k <= p2->order; ++k) { result->a[k] = p2->a[k]; }

        // Set to zerro
        for (size_t k = p2->order + 1; k <= result->order; ++k) { result->a[k] = 0; }

        // Return
        return true;
    }
}
bool dsp_polynomial_add_and_assign(dsp_poly_t* const p1, const dsp_poly_t* const p2) {
    if (p1 == NULL || p2 || NULL) { return false; }
    if (p1->order < p2->order) { return false; }
 
    // Add
    for (size_t k = 0; k <= p2->order; ++k) { p1->a[k] += p2->a[k]; }

    // Return
    return true;
}
bool dsp_polynomial_subtract(dsp_poly_t* const result, const dsp_poly_t* const p1, const dsp_poly_t* const p2) {
    if (result == NULL || p1 == NULL || p2 || NULL) { return false; }
    if (p1->order >= p2->order) {
        if (result->order < p1->order) { return false; }

        // Add
        for (size_t k = 0; k <= p2->order; ++k) { result->a[k] = p1->a[k] - p2->a[k]; }

        // Copy
        for (size_t k = p2->order + 1; k <= p1->order; ++k) { result->a[k] = p1->a[k]; }

        // Set to zerro
        for (size_t k = p1->order + 1; k <= result->order; ++k) { result->a[k] = 0; }

        // Return
        return true;
    }
    else {
        if (result->order < p2->order) { return false; }

        // Add
        for (size_t k = 0; k <= p1->order; ++k) { result->a[k] = p1->a[k] - p2->a[k]; }

        // Copy
        for (size_t k = p1->order + 1; k <= p2->order; ++k) { result->a[k] = -(p2->a[k]); }

        // Set to zerro
        for (size_t k = p2->order + 1; k <= result->order; ++k) { result->a[k] = 0; }

        // Return
        return true;
    }
}
bool dsp_polynomial_subtract_and_assign(dsp_poly_t* const p1, const dsp_poly_t* const p2) {
    if (p1 == NULL || p2 || NULL) { return false; }
    if (p1->order < p2->order) { return false; }
 
    // Add
    for (size_t k = 0; k <= p2->order; ++k) { p1->a[k] -= p2->a[k]; }

    // Return
    return true;
}

// Arithmetic
bool dsp_polynomial_multiply(dsp_poly_t* const w, const dsp_poly_t* const u, const dsp_poly_t* const v) {
    if (w == NULL || u == NULL || v == NULL) { return false; }
    if (w->order < u->order + v->order) { return false; }

    // From Matlab:
    // w = conv(u,v) returns the convolution of vectors u and v. 
    // If u and v are vectors of polynomial coefficients, 
    // convolving them is equivalent to multiplying the two polynomials.
    for (size_t	k = 0; k <= (u->order + v->order); k++) {

        const size_t start = (k <= u->order ? 0 : k - u->order);
        const size_t end = (k <= v->order ? k : v->order);
        w->a[k] = 0;

        for (size_t j = start; j <= end; ++j) {
            w->a[k] += v->a[j] * u->a[k-j];
        }
    }

    return true;
}
bool dsp_polynomial_divide(dsp_poly_t* const q, dsp_poly_t* const r, const dsp_poly_t* const u, const dsp_poly_t* const v) {
    if (q == NULL || r == NULL || u == NULL || v == NULL) { return false; }
    if (q->order < u->order - v->order) { return false; }

    // Copy u into r
    if (!dsp_polynomial_copy_assign(r, u)) { return false; }

    size_t n = r->order;
    const size_t m = q->order;
    for (size_t k = q->order + 1; k > 0; /*--k*/) {
        
        // Do this at the start of the loop
        --k;
        
        // Divide
        q->a[k] = r->a[n] / v->a[m];

        // Subtract
        r->a[n] = 0;
        for (size_t j = 0; j < m; ++j) {
            r->a[j + k] -= q->a[k] * v->a[j];
        }

        // Reduce
        --n;
    }

    return true;
}

// Fit
bool dsp_polynomial_fit(dsp_poly_t* const p, const real_t* const x, const real_t* const y, const size_t size) {

    // Create a vandermonde matrix
    dsp_matrix_t* const V = dsp_matrix_create_vandermonde(p->order, x, size);

    // Put y into a Vector struct
    const dsp_vector_t Y = {
        .elements = (real_t*) y,
        .size = size
    };

    // Put p into a Vector struct
    dsp_vector_t P = {
        .elements = p->a,
        .size = p->order + 1
    };

    // Solve
    if (dsp_vector_solve_lse(&P, V, &Y)) {
        dsp_matrix_destroy(V);
        return true;
    }
    else {
        dsp_matrix_destroy(V);
        return false;
    }
}
bool dsp_polyfit(real_t* const p, const size_t order, const real_t* const x, const real_t* const y, const size_t size) {
    if (p == NULL || x == NULL || y == NULL) { return false; }
    if (size == 0) { return false; }

    // Put p into a Polynomial struct
    dsp_poly_t P = {
        .a = p,
        .order = order
    };

    return dsp_polynomial_fit(&P, x, y, size);
}

// Evaluate
real_t dsp_polynomial_val(const dsp_poly_t* const p, const real_t x) {
    if (p == NULL) { return 0; }

    // Horner
    real_t accumulator = p->a[p->order];
    for (size_t k = p->order; k > 0; /*--k*/) {
        accumulator = x * accumulator + p->a[--k /*k-1*/] ;
    }

    // return
    return accumulator;
}
real_t dsp_polyval(const real_t* const p, const size_t order, const real_t x) {
    if (p == NULL) { return 0; }

    // Put p into a Polynomial struct
    const dsp_poly_t P = {
        .a = (real_t*) p,
        .order = order
    };

    return dsp_polynomial_val(&P, x);
}


// Change size
bool dsp_polynomial_shrink_to_fit(dsp_poly_t* const p) {
    if (p == NULL) {return p; }


    // if (an != 0) => Nothing to do
    if ((p->order == 0) || (p->a[p->order] != 0)) { return true; }

    // Don't check a0 => (k >= 1)
    for (size_t k = p->order-1; k >= 1; --k) {

        // find first nonzero element
        if ((p->a[k] != 0)) {

            // shrink array
            real_t* const new_array = (real_t*) realloc(p->a, ARRAY_SIZE(k));

            // check
            if (new_array != NULL) { 
                p->order = k; 
                p->a = new_array;
                return true;
            }
            else {
                return false;
            }
        }
    }

    // shrink array to size 1 (order = 0)
    real_t* const new_array = (real_t*) realloc(p->a, REAL_SIZE);

    // check
    if (new_array != NULL) { 
        p->order = 0; 
        p->a = new_array;
        return true;
    }
    else {
        return false;
    }
}
bool dsp_polynomial_grow_to(dsp_poly_t* const p, const size_t new_order) {
    if (new_order <= p->order) { return false; }
    const size_t old_order = p->order;

    // grow array
    real_t* const new_array = (real_t*) realloc(p->a, ARRAY_SIZE(new_order));

    // check
    if (new_array != NULL) { 

        // initilize new coeffs
        memset(&(new_array[old_order]), 0, (new_order - old_order) * REAL_SIZE);
        p->order = new_order; 
        p->a = new_array;
        return true;
    }
    else {
        return false;
    }
}