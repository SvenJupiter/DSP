#include <stdlib.h> // malloc, realloc, free
#include <string.h> // memset, memcpy, memmove
#include "DSP/Discrete/Signal.h"

#define SIGNAL_SIZE sizeof(dsp_signal_t)
#define NEW_SIGNAL() malloc(SIGNAL_SIZE)

#define REAL_SIZE sizeof(real_t)
#define ARRAY_SIZE(size) ((size) * REAL_SIZE)
#define ELEMENT(sig, index) ((sig)->elements[(index)])

#define SIGNAL_MAX_CAPACITY SIZE_MAX




// Returns the scalar dot product of u and v
real_t dsp_dot_product(
    const real_t* const u, 
    const real_t* const v, 
    const size_t size) {

    if (u == NULL || v == NULL) { return 0; }
    if (size == 0) { return 0; }

    // Init
    real_t sum = v[0] * u[0];

    // Add
    for (size_t k = 1; k < size; ++k) {
        sum += v[k] * u[k];
    }

    // Return
    return sum;
}



// Convolution and polynomial multiplication
// w = conv(u,v) returns the convolution of vectors u and v. 
// If u and v are vectors of polynomial coefficients, 
// convolving them is equivalent to multiplying the two polynomials.
size_t dsp_conv(
    const real_t* const u, const size_t u_size,
    const real_t* const v, const size_t v_size,
    real_t* const w, const size_t w_size) {

    // Check
    if (u == NULL || v == NULL || w == NULL) { return 0; }
    if (u_size == 0 || v_size == 0) { return 0; }
    const size_t conv_size = (u_size + v_size - 1);
    if (w_size < conv_size) { return 0; }


    // From Matlab:
    // w = conv(u,v) returns the convolution of vectors u and v. 
    // If u and v are vectors of polynomial coefficients, 
    // convolving them is equivalent to multiplying the two polynomials.
    size_t start, end;
    for (size_t	k = 0; k < conv_size; k++) {

        // Determine start and end
        start = (k < u_size ? 0 : k - (u_size - 1));
        end = (k < v_size ? k : (v_size - 1));

        // Init
        w[k] = 0;

        // Calculate k-th element
        for (size_t j = start; j <= end; ++j) {
            w[k] += v[j] * u[k-j];
        }
    }

    return conv_size;
}

// Deconvolution and polynomial division
// [q,r] = deconv(u,v) deconvolves a vector v out of a vector u using long division, 
// and returns the quotient q and remainder r such that u = conv(v,q) + r. 
// If u and v are vectors of polynomial coefficients, then deconvolving them is equivalent to 
// dividing the polynomial represented by u by the polynomial represented by v.
size_t dsp_deconv(
    const real_t* const u, const size_t u_size,
    const real_t* const v, const size_t v_size,
    real_t* const q, size_t q_size,
    real_t* const r, size_t r_size) {


    // Check
    if (u == NULL || v == NULL || q == NULL || r == NULL) { return 0; }
    if (u_size == 0 || v_size == 0) { return 0; }
    if (v[0] == 0) { return 0; }
    if (u_size < v_size) { return 0; }        
    if (q_size < (u_size - v_size + 1)) { return 0; }

    // we copy 'u' into 'r' at the start
    if (r_size < u_size) { return 0; } 
    if (r != u) { memcpy(r, u, u_size * sizeof(real_t)); };

    // update sizes
    q_size = u_size - v_size + 1;
    r_size = v_size - 1;

    // Deconvolution 
    for (size_t k = 0; k < q_size; ++k) {

        // Divide
        q[k] = r[k] / v[0];

        // Subtract
        r[k] = 0;
        for (size_t j = 1; j < v_size; ++j) {
            r[k+j] -= q[k] * v[j]; 
        }
    }

    return r_size;
}






// Create
dsp_signal_t* dsp_signal_create(const size_t initial_capacity) {

    dsp_signal_t* const signal = NEW_SIGNAL();
    *signal = dsp_signal_construct(initial_capacity);
    if (initial_capacity > 0 && signal->elements != NULL) {
        return signal;
    }
    else{
        free(signal);
        return NULL;
    }
}

// Create
dsp_signal_t dsp_signal_construct(const size_t initial_capacity) {
    dsp_signal_t vec = {
        .elements = NULL,
        .size = 0,
        .capacity = 0
    };
    if (initial_capacity > 0) {
        vec.elements = malloc(initial_capacity * (sizeof(*(vec.elements))));
        if (vec.elements != NULL) {
            vec.capacity = initial_capacity;
        }
    }
    return vec;
}


// Copy
dsp_signal_t* dsp_signal_create_copy(const dsp_signal_t* const other) {
    if (other == NULL) { return NULL; }

    dsp_signal_t* const signal = dsp_signal_create(other->capacity);
    if (signal == NULL) { return NULL; }

    if (other->size > 0) {
        memcpy(signal->elements, other->elements, ARRAY_SIZE(other->size));
        signal->size = other->size;
    }

    return signal;
}

// Copy
bool dsp_signal_copy_assign(dsp_signal_t* const  dest, const dsp_signal_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    if (dest == src) { return false; }

    dsp_signal_assign(dest, src->elements, src->size);
    return true;
}


// Move
dsp_signal_t* dsp_signal_create_move(dsp_signal_t* const other) {

    dsp_signal_t* const signal = NEW_SIGNAL();
    if (signal == NULL) { return NULL; }

    signal->capacity = other->capacity;
    signal->size = other->size;
    signal->elements = other->elements;

    other->capacity = 0;
    other->size = 0;
    other->elements = NULL;

    return signal;
}

// Move
bool dsp_signal_move_assign(dsp_signal_t* const  dest, dsp_signal_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }
    dsp_signal_destruct(dest);

    dest->capacity = src->capacity;
    dest->size = src->size;
    dest->elements = src->elements;

    src->capacity = 0;
    src->size = 0;
    src->elements = NULL;

    return true;
}


// Destroy
bool dsp_signal_destruct(dsp_signal_t* const signal) {
    if (signal == NULL) { return false; }
    if (signal->elements != NULL) {
        free(signal->elements);
        signal->elements = NULL;
    }
    signal->capacity = 0;
    signal->size = 0;
    return true;
}

// Destroy
bool dsp_signal_destroy(dsp_signal_t* const signal) {
    if (signal == NULL) { return false; }
    dsp_signal_destruct(signal);
    free(signal);
    return true;
}


// dot, conv, deconv
real_t dsp_signal_dot_product(const dsp_signal_t* const u, const dsp_signal_t* const v) {
    if (u == NULL || v == NULL) { return 0; }
    if (u->size != v->size) { return 0; }
    return dsp_dot_product(u->elements, v->elements, v->size);
}
size_t dsp_signal_conv(dsp_signal_t* const w, const dsp_signal_t* const u, const dsp_signal_t* const v) {
    // check
    if (u == NULL || v == NULL || w == NULL) { return 0; }
    if (u->size == 0 || v->size == 0) { return 0; }

    dsp_signal_resize(w, u->size + v->size - 1, NULL);
    if (w->size != (u->size + v->size - 1)) { return 0; }
    return dsp_conv(u->elements, u->size, v->elements, v->size, w->elements, w->size);
}
size_t dsp_signal_deconv(dsp_signal_t* const q, dsp_signal_t* const r, const dsp_signal_t* const u, const dsp_signal_t* const v) {
    // Check
    if (u == NULL || v == NULL || q == NULL || r == NULL) { return 0; }
    if (u->size == 0 || v->size == 0) { return 0; }
    if (v->elements[0] == 0) { return 0; }
    if (u->size < v->size) { return 0; }

    dsp_signal_resize(q, (u->size - v->size + 1), NULL);
    if (q->size < (u->size - v->size + 1)) { return 0; }

    dsp_signal_resize(r, u->size, NULL);
    if (r->size != u->size) { return 0; }

    return dsp_deconv(u->elements, u->size, v->elements, v->size, q->elements, q->size, r->elements, r->size);
}





// ----- std::vector<real_t> functions -----

void dsp_signal_reserve(dsp_signal_t* const signal, const size_t new_capacity) {
    if (signal == NULL) { return; }
    if (new_capacity <= signal->capacity) { return; }
    real_t* const reallocated_elements = realloc(signal->elements, new_capacity * sizeof(*(signal->elements)));
    if (reallocated_elements != NULL) {
        signal->elements = reallocated_elements;
        signal->capacity = new_capacity;
    }
}


void dsp_signal_resize(dsp_signal_t* const signal, const size_t new_size, const real_t* const value_ptr) {
    if (signal == NULL) { return; }
    const size_t old_size = signal->size;
    if (new_size == old_size) { return; }
    else if (new_size < old_size) {
        signal->size = new_size;
        memset(&(signal->elements[new_size]), 0, (old_size - new_size) * sizeof(real_t));
    }
    else if (new_size <= signal->capacity) {
        signal->size = new_size;
        if (value_ptr == NULL) {
            memset(&(signal->elements[old_size]), 0, (new_size - old_size) * sizeof(real_t));
        }
        else {
            for (size_t i = old_size; i < new_size; ++i) {
                memcpy(&(signal->elements[i]), value_ptr, sizeof(real_t));
            }
        }
    }
    else {
        const size_t new_capacity = new_size;
        real_t* const reallocated_elements = realloc(signal->elements, new_capacity * sizeof(real_t));
        if (reallocated_elements != NULL) {
            signal->elements = reallocated_elements;
            signal->capacity = new_capacity;
            signal->size = new_size;
            if (value_ptr == NULL) {
                memset(&(signal->elements[old_size]), 0, (new_size - old_size) * sizeof(real_t));
            }
            else {
                for (size_t i = old_size; i < new_size; ++i) {
                    memcpy(&(signal->elements[i]), value_ptr, sizeof(real_t));
                }
            }
        }
    }
}


void dsp_signal_shrink_to_fit(dsp_signal_t* const signal) {
    if (signal == NULL) { return; }
    const size_t new_capacity = signal->size;
    if (new_capacity == 0) {
        free(signal->elements);
        signal->elements = NULL;
        signal->capacity = 0;
    }
    else {
        real_t* const reallocated_elements = realloc(signal->elements, new_capacity * sizeof(*(signal->elements)));
        if (reallocated_elements != NULL) {
            signal->elements = reallocated_elements;
            signal->capacity = new_capacity;
        }
    }
}


real_t* dsp_signal_at(dsp_signal_t* const signal, const size_t position) {
    if (signal == NULL) { return NULL; }
    if (signal->elements == NULL) { return NULL; }
    if (position >= signal->size) { return NULL; }
    return &(signal->elements[position]);
}


real_t* dsp_signal_front(dsp_signal_t* const signal) {
    if (signal == NULL) { return NULL;}
    if (signal->size == 0) { return NULL; }
    return &(signal->elements[0]);
}


real_t* dsp_signal_back(dsp_signal_t* const signal) {
    if (signal == NULL) { return NULL; }
    if (signal->elements == NULL) { return NULL; }
    if (signal->size == 0) { return NULL; }
    return &(signal->elements[signal->size - 1]);
}


real_t* dsp_signal_data(dsp_signal_t* const signal) {
    if (signal == NULL) { return NULL; }
    return signal->elements;
}


size_t dsp_signal_size(dsp_signal_t* const signal) {
    if (signal == NULL) { return 0; }
    return signal->size;
}


size_t dsp_signal_capacity(dsp_signal_t* const signal) {
    if (signal == NULL) { return 0; }
    return signal->capacity;
}


bool dsp_signal_empty(dsp_signal_t* const signal) {
    if (signal == NULL) { return true; }
    return (signal->elements == NULL || signal->size == 0);
}


void dsp_signal_assign(dsp_signal_t* const signal, const real_t* const new_elements, const size_t new_size) {
    if (signal == NULL) { return; }
    const size_t old_size = signal->size;
    if (new_elements == NULL || new_size == 0) {
        if (signal->elements != NULL && old_size > 0) {
            memset(signal->elements, 0, old_size * sizeof(real_t));
            signal->size = 0;
        }
    }
    else if (new_size < old_size) {
        memcpy(signal->elements, new_elements, new_size * sizeof(real_t));
        memset(&(signal->elements[new_size]), 0, (old_size - new_size) * sizeof(real_t));
        signal->size = new_size;
    }
    else if (new_size <= signal->capacity) {
        memcpy(signal->elements, new_elements, new_size * sizeof(real_t));
        signal->size = new_size;
    }
    else {
        real_t* const reallocated_array = malloc(new_size * sizeof(real_t));
        if (reallocated_array != NULL) {
            if (signal->elements != NULL) { free(signal->elements); }
            signal->elements = reallocated_array;
            memcpy(signal->elements, new_elements, new_size * sizeof(real_t));
            signal->size = new_size;
            signal->capacity = new_size;
        }
    }
}


void dsp_signal_push_back(dsp_signal_t* const signal, const real_t* const new_element) {
    if (signal == NULL) { return; }
    if (new_element == NULL) { return; }
    if (signal->size == SIGNAL_MAX_CAPACITY) { return; }
    if (signal->capacity == 0) {
        signal->elements = malloc(sizeof(real_t));
        if (signal->elements != NULL) {
            memcpy(signal->elements, new_element, sizeof(real_t));
            signal->size = 1;
            signal->capacity = 1;
        }
    }
    else if (signal->size < signal->capacity) {
        memcpy(&(signal->elements[signal->size]), new_element, sizeof(real_t));
        signal->size += 1;
    }
    else {
        const size_t new_capacity = (((SIGNAL_MAX_CAPACITY / 2) < signal->capacity) ? SIGNAL_MAX_CAPACITY : 2 * signal->capacity);
        real_t* const reallocated_elements = realloc(signal->elements, new_capacity * sizeof(real_t));
        if (reallocated_elements != NULL) {
            signal->elements = reallocated_elements;
            signal->capacity = new_capacity;
            memcpy(&(signal->elements[signal->size]), new_element, sizeof(real_t));
            signal->size += 1;
        }
    }
}


void dsp_signal_pop_back(dsp_signal_t* const signal) {
    if (signal == NULL) { return; }
    if (signal->size > 0) {
        if (signal->capacity <= 4) {
            memset(&(signal->elements[signal->size - 1]), 0, sizeof(real_t));
            signal->size -= 1;
        }
        else {
            const size_t new_size = signal->size - 1;
            const size_t new_capacity = signal->capacity / 2;
            if (new_capacity > new_size) {
                real_t* const new_array = realloc(signal->elements, new_capacity * sizeof(real_t));
                if (new_array != NULL) {
                    signal->elements = new_array;
                    signal->size = new_size;
                    signal->capacity = new_capacity;
                }
                else {
                    memset(&(signal->elements[signal->size - 1]), 0, sizeof(real_t));
                    signal->size -= 1;
                }
            }
            else {
                memset(&(signal->elements[signal->size - 1]), 0, sizeof(real_t));
                signal->size -= 1;
            }
        }
    }
}


real_t* dsp_signal_insert(dsp_signal_t* const signal, const size_t position, const real_t* const new_element) {
    if (signal == NULL) { return NULL; }
    if (new_element == NULL) { return NULL; }
    if (signal->size == SIGNAL_MAX_CAPACITY) { return NULL; }
    if (position > signal->size) { return NULL; }
    else if (position == signal->size) {
        if (signal->capacity == 0) {
            signal->elements = malloc(sizeof(real_t));
            if (signal->elements != NULL) {
                memcpy(signal->elements, new_element, sizeof(real_t));
                signal->size = 1;
                signal->capacity = 1;
                return &(signal->elements[0]);
            }
            else {
                return NULL;
            }
        }
        else if (signal->size < signal->capacity) {
            memcpy(&(signal->elements[signal->size]), new_element, sizeof(real_t));
            signal->size += 1;
            return &(signal->elements[signal->size - 1]);
        }
        else {
            const size_t new_capacity = (((SIGNAL_MAX_CAPACITY / 2) < signal->capacity) ? SIGNAL_MAX_CAPACITY : 2 * signal->capacity);
            real_t* const reallocated_elements = realloc(signal->elements, new_capacity * sizeof(real_t));
            if (reallocated_elements != NULL) {
                signal->elements = reallocated_elements;
                signal->capacity = new_capacity;
                memcpy(&(signal->elements[signal->size]), new_element, sizeof(real_t));
                signal->size += 1;
                return &(signal->elements[signal->size - 1]);
            }
            else {
                return NULL;
            }
        }
    }
    else {
        if (signal->size < signal->capacity) {
            memmove(&(signal->elements[position+1]), &(signal->elements[position]), (signal->size - position) * sizeof(real_t));
            memcpy(&(signal->elements[position]), new_element, sizeof(real_t));
            signal->size += 1;
            return &(signal->elements[position]);
        }
        else {
            const size_t new_capacity = (((SIGNAL_MAX_CAPACITY / 2) < signal->capacity) ? SIGNAL_MAX_CAPACITY : 2 * signal->capacity);
            real_t* const reallocated_array = malloc(new_capacity * sizeof(real_t));
            if (reallocated_array != NULL) {
                if (position > 0) {
                    memcpy(reallocated_array, signal->elements, position * sizeof(real_t));
                }
                memcpy(&(reallocated_array[position]), new_element, sizeof(real_t));
                if (position < signal->size) {
                    memcpy(&(reallocated_array[position+1]), &(signal->elements[position]), (signal->size - position) * sizeof(real_t));
                }
                free(signal->elements);
                signal->elements = reallocated_array;
                signal->capacity = new_capacity;
                signal->size += 1;
                return &(signal->elements[position]);
            }
            else {
                return NULL;
            }
        }
    }
}


void dsp_signal_erase(dsp_signal_t* const signal, const size_t position) {
    if (signal == NULL) { return; }
    if (position >= signal->size) { return; }
    else if (position == (signal->size -1)) {
        if (signal->capacity <= 4) {
            memset(&(signal->elements[signal->size - 1]), 0, sizeof(real_t));
            signal->size -= 1;
        }
        else {
            const size_t new_size = signal->size - 1;
            const size_t new_capacity = signal->capacity / 2;
            if (new_capacity > new_size) {
                real_t* const new_array = realloc(signal->elements, new_capacity * sizeof(real_t));
                if (new_array != NULL) {
                    signal->elements = new_array;
                    signal->size = new_size;
                    signal->capacity = new_capacity;
                }
                else {
                    memset(&(signal->elements[signal->size - 1]), 0, sizeof(real_t));
                    signal->size -= 1;
                }
            }
            else {
                memset(&(signal->elements[signal->size - 1]), 0, sizeof(real_t));
                signal->size -= 1;
            }
        }
    }
    else {
        const size_t new_size = signal->size - 1;
        const size_t new_capacity = signal->capacity / 2;
        if (new_capacity >= 4 && new_capacity > new_size) {
            real_t* const new_array = malloc(new_capacity * sizeof(real_t));
            if (new_array != NULL) {
                if (position > 0) { memcpy(new_array, signal->elements, position * sizeof(real_t)); }
                memcpy(&(new_array[position]), &(signal->elements[position+1]), (signal->size - (position + 1)) * sizeof(real_t));
                free(signal->elements);
                signal->elements = new_array;
                signal->size = new_size;
                signal->capacity = new_capacity;
            }
            else {
                memmove(&(signal->elements[position]), &(signal->elements[position+1]), (signal->size - (position + 1)) * sizeof(real_t));
                memset(&(signal->elements[signal->size -1]), 0, sizeof(real_t));
                signal->size -= 1;  
            }
        }
        else {
            memmove(&(signal->elements[position]), &(signal->elements[position+1]), (signal->size - (position + 1)) * sizeof(real_t));
            memset(&(signal->elements[signal->size -1]), 0, sizeof(real_t));
            signal->size -= 1;
        }
    }
}


void dsp_signal_swap(dsp_signal_t* const vec_a_ptr, dsp_signal_t* const vec_b_ptr) {
    if (vec_a_ptr == NULL) { return; }
    if (vec_b_ptr == NULL) { return; }
    if (vec_a_ptr == vec_b_ptr) { return; }
    const dsp_signal_t temp = *vec_a_ptr;
    *vec_a_ptr = *vec_b_ptr;
    *vec_b_ptr = temp;
}


void dsp_signal_clear(dsp_signal_t* const signal) {
    if (signal == NULL) { return; }
    if (signal->size > 0) {
        memset(signal->elements, 0, signal->size * sizeof(*(signal->elements)));
        signal->size = 0;
    }
}


real_t* dsp_signal_emplace(dsp_signal_t* const signal, const size_t position, const bool fill_zeros) {
    if (signal == NULL) { return NULL; }
    if (signal->size == SIGNAL_MAX_CAPACITY) { return NULL; }
    if (position > signal->size) { return NULL; }
    else if (position == signal->size) {
        if (signal->capacity == 0) {
            signal->elements = malloc(sizeof(real_t));
            if (signal->elements != NULL) {
                if (fill_zeros) { memset(signal->elements, 0, sizeof(real_t)); }
                signal->size = 1;
                signal->capacity = 1;
                return &(signal->elements[0]);
            }
            else {
                return NULL;
            }
        }
        else if (signal->size < signal->capacity) {
            if (fill_zeros) { memset(&(signal->elements[signal->size]), 0, sizeof(real_t)); }
            signal->size += 1;
            return &(signal->elements[signal->size - 1]);
        }
        else {
            const size_t new_capacity = (((SIGNAL_MAX_CAPACITY / 2) < signal->capacity) ? SIGNAL_MAX_CAPACITY : 2 * signal->capacity);
            real_t* const reallocated_elements = realloc(signal->elements, new_capacity * sizeof(real_t));
            if (reallocated_elements != NULL) {
                signal->elements = reallocated_elements;
                signal->capacity = new_capacity;
                if (fill_zeros) { memset(&(signal->elements[signal->size]), 0, sizeof(real_t)); }
                signal->size += 1;
                return &(signal->elements[signal->size - 1]);
            }
            else {
                return NULL;
            }
        }
    }
    else {
        if (signal->size < signal->capacity) {
            memmove(&(signal->elements[position+1]), &(signal->elements[position]), (signal->size - position) * sizeof(real_t));
            if (fill_zeros) { memset(&(signal->elements[position]), 0, sizeof(real_t)); }
            signal->size += 1;
            return &(signal->elements[position]);
        }
        else {
            const size_t new_capacity = (((SIGNAL_MAX_CAPACITY / 2) < signal->capacity) ? SIGNAL_MAX_CAPACITY : 2 * signal->capacity);
            real_t* const reallocated_array = malloc(new_capacity * sizeof(real_t));
            if (reallocated_array != NULL) {
                if (position > 0) {
                    memcpy(reallocated_array, signal->elements, position * sizeof(real_t));
                }
                if (fill_zeros) { memset(&(reallocated_array[position]), 0, sizeof(real_t)); }
                if (position < signal->size) {
                    memcpy(&(reallocated_array[position+1]), &(signal->elements[position]), (signal->size - position) * sizeof(real_t));
                }
                free(signal->elements);
                signal->elements = reallocated_array;
                signal->capacity = new_capacity;
                signal->size += 1;
                return &(signal->elements[position]);
            }
            else {
                return NULL;
            }
        }
    }
}


real_t* dsp_signal_emplace_back(dsp_signal_t* const signal, const bool fill_zeros) {
    if (signal == NULL) { return NULL; }
    if (signal->size == SIGNAL_MAX_CAPACITY) { return NULL; }
    if (signal->capacity == 0) {
        signal->elements = malloc(sizeof(real_t));
        if (signal->elements != NULL) {
            if (fill_zeros) { memset(signal->elements, 0, sizeof(real_t)); }
            signal->size = 1;
            signal->capacity = 1;
            return signal->elements;
        }
        else {
            return NULL;
        }
    }
    else if (signal->size < signal->capacity) {
        if (fill_zeros) { memset(&(signal->elements[signal->size]), 0, sizeof(real_t)); }
        signal->size += 1;
        return &(signal->elements[signal->size-1]);
    }
    else {
        const size_t new_capacity = (((SIGNAL_MAX_CAPACITY / 2) < signal->capacity) ? SIGNAL_MAX_CAPACITY : 2 * signal->capacity);
        real_t* const reallocated_elements = realloc(signal->elements, new_capacity * sizeof(real_t));
        if (reallocated_elements != NULL) {
            signal->elements = reallocated_elements;
            signal->capacity = new_capacity;
            if (fill_zeros) { memset(&(signal->elements[signal->size]), 0, sizeof(real_t)); }
            signal->size += 1;
            return &(signal->elements[signal->size-1]);
        }
        else {
            return NULL;
        }
    }
}




