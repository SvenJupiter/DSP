#include <stdlib.h> // malloc, free
#include <string.h> // memcpy, memset
#include "DSP/Discrete/Integrator.h"
#include "DSP/Discrete/Discontinuous.h"


#define CLIP(x, UL, LL) ((x) >= (UL) ? (UL) : ((x) <= (LL) ? (LL) : (x)))
#define CLIPIF(x, CL, UL, LL) ((CL) ? CLIP(x, UL, LL) : (x))

dsp_integrator_t* dsp_integrator_create(const real_t K, const real_t Ts, const s_approximation_t IF, const bool limit_output, const real_t upper_limit, const real_t lower_limit) {

    // Create a new integrator
    dsp_integrator_t* const integrator = malloc(sizeof(dsp_integrator_t));
    if (integrator == NULL) { return NULL; }

    if (dsp_integrator_configure(integrator, K, Ts, IF, limit_output, upper_limit, lower_limit)) {
        return integrator;
    }
    else {
        free(integrator);
        return NULL;
    }
}

dsp_integrator_t* dsp_integrator_create_copy(const dsp_integrator_t* const other) {
    if (other == NULL) { return NULL; }

    // Create a new integrator
    dsp_integrator_t* const integrator = malloc(sizeof(dsp_integrator_t));
    if (integrator == NULL) { return NULL; }

    if (dsp_integrator_copy_assign(integrator, other)) {
        return integrator;
    }
    else {
        free(integrator);
        return NULL;
    }
}
bool dsp_integrator_copy_assign(dsp_integrator_t* const dest, const dsp_integrator_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }

    *dest = *src;
    return true;
}

dsp_integrator_t* dsp_integrator_create_move(dsp_integrator_t* const other) {
    if (other == NULL) { return NULL; }

    // Create a new integrator
    dsp_integrator_t* const integrator = dsp_integrator_create_copy(other);

    if (integrator != NULL) { memset(other, 0, sizeof(dsp_integrator_t)); }
    return integrator;
}
bool dsp_integrator_move_assign(dsp_integrator_t* const dest, dsp_integrator_t* const src) {
    if (dest == NULL || src == NULL) { return false; }
    if (dest == src) { return false; }

    dsp_integrator_copy_assign(dest, src);
    memset(src, 0, sizeof(dsp_integrator_t));
    return true;
}

bool dsp_integrator_swap(dsp_integrator_t* const a, dsp_integrator_t* const b) {
    if (a == NULL || b == NULL) { return true; }
    if (a == b) { return false; }

    // Swap
    dsp_integrator_t temp = *a;
    *a = *b;
    *b = temp;
    return true;
}

bool dsp_integrator_destroy(dsp_integrator_t* const integrator) {
    if (integrator == NULL) { return false; }

    free(integrator);
    return true;
}

bool dsp_integrator_configure(dsp_integrator_t* const integrator, const real_t K, const real_t Ts, const s_approximation_t IF, const bool limit_output, const real_t upper_limit, const real_t lower_limit) {
    if (integrator == NULL) { return false; }

    dsp_integrator_set_parameters(integrator, K, Ts, IF);
    dsp_integrator_set_output_saturation(integrator, limit_output, upper_limit, lower_limit);
    dsp_integrator_reset(integrator);
    return true;
}

bool dsp_integrator_set_parameters(dsp_integrator_t* const integrator, const real_t K, const real_t Ts, const s_approximation_t IF) {
    if (integrator == NULL) { return false; }

    integrator->K = K;
    integrator->Ts = Ts;
    integrator->IF = IF;

    return true;
}

bool dsp_integrator_set_output_saturation(dsp_integrator_t* const integrator, const bool limit_output, const real_t upper_limit, const real_t lower_limit) {
    if (integrator == NULL) { return false; }

    integrator->limit_output = limit_output;
    integrator->upper_limit = upper_limit;
    integrator->lower_limit = lower_limit;
    return true;
}


real_t dsp_integrator_get_state(dsp_integrator_t* const integrator) {
    if (integrator == NULL) { return 0; }
    return integrator->x;
}

bool dsp_integrator_set_state(dsp_integrator_t* const integrator, const real_t x0) {
    if (integrator == NULL) { return false; }

    integrator->x = dsp_discontinuous_saturate_if(integrator->limit_output, x0, integrator->upper_limit, integrator->lower_limit);
    return true;
}

bool dsp_integrator_reset(dsp_integrator_t* const integrator) {
    return dsp_integrator_set_state(integrator, 0);
}



real_t dsp_integrator_get_output(dsp_integrator_t* const integrator, const real_t u) {
    if (integrator == NULL) { return 0; }

    real_t output;
    switch (integrator->IF) {
        case ForwardEuler:

            // y(n) = x(n)
            return integrator->x;

        case BackwardEuler:

            // y(n) = x(n) + K * Ts * u(n)
            output = integrator->x + integrator->K * integrator->Ts * u;

            // Saturation
            return dsp_discontinuous_saturate_if(integrator->limit_output,
                output, integrator->upper_limit, integrator->lower_limit
            );

        case Trapezoidal: 

            // y(n) = x(n) + K * Ts/2 * u(n)
            output = integrator->x + integrator->K * integrator->Ts * u / 2.0f;

            // Saturation
            return dsp_discontinuous_saturate_if(integrator->limit_output,
                output, integrator->upper_limit, integrator->lower_limit
            );

        default:
            return 0;
    }
}

real_t dsp_integrator_update_state(dsp_integrator_t* const integrator, const real_t u) {
    if (integrator == NULL) { return 0; }

    switch (integrator->IF) {
        case ForwardEuler:

            // x(n+1) = x(n) + K * Ts * u(n)
            integrator->x = integrator->x + integrator->K * integrator->Ts * u;

            // Saturation
            integrator->x = dsp_discontinuous_saturate_if(integrator->limit_output, integrator->x, integrator->upper_limit, integrator->lower_limit);

            break;

        case BackwardEuler:

            // x(n+1) = x(n) + K * T * u(n)
            integrator->x = integrator->x + integrator->K * integrator->Ts * u;

            // Saturation
            integrator->x = dsp_discontinuous_saturate_if(integrator->limit_output, integrator->x, integrator->upper_limit, integrator->lower_limit);

            break;

        case Trapezoidal: 

            // x(n+1) = x(n) + K * T * u(n)
            integrator->x = integrator->x + integrator->K * integrator->Ts * u;

            // Saturation
            integrator->x = dsp_discontinuous_saturate_if(integrator->limit_output, integrator->x, integrator->upper_limit, integrator->lower_limit);

            break;

        default:
            break;
    }


    return integrator->x;
}

real_t dsp_integrator_update(dsp_integrator_t* const integrator, real_t u) {
    if (integrator == NULL) { return 0; }

    // // Get Output
    // const real_t output = dsp_integrator_get_output(integrator, u);

    // // Update state
    // dsp_integrator_update_state(integrator, u);

    // return output;

    real_t output; 
    switch (integrator->IF) {
        case ForwardEuler:

            // y(n) = x(n)
            output = integrator->x;

            // x(n+1) = x(n) + K * Ts * u(n)
            integrator->x = integrator->x + integrator->K * integrator->Ts * u;

            // Saturation
            integrator->x = dsp_discontinuous_saturate_if(integrator->limit_output, integrator->x, integrator->upper_limit, integrator->lower_limit);

            break;

        case BackwardEuler:

            // y(n) = x(n) + K * Ts * u(n)
            output = integrator->x + integrator->K * integrator->Ts * u;

            // Saturation
            output = dsp_discontinuous_saturate_if(integrator->limit_output,
                output, integrator->upper_limit, integrator->lower_limit
            );

            // x(n+1) = x(n) + K * T * u(n) = y(n)
            integrator->x = output;

            break;

        case Trapezoidal: 

            // y(n) = x(n) + K * Ts/2 * u(n)
            output = integrator->x + integrator->K * integrator->Ts * u / 2.0f;

            // Saturation
            output = dsp_discontinuous_saturate_if(integrator->limit_output,
                output, integrator->upper_limit, integrator->lower_limit
            );

            // x(n+1) = x(n) + K * T * u(n)
            integrator->x = integrator->x + integrator->K * integrator->Ts * u;

            // Saturation
            integrator->x = dsp_discontinuous_saturate_if(integrator->limit_output, integrator->x, integrator->upper_limit, integrator->lower_limit);

            break;

        default:
            break;
    }

    return output;


    // switch (integrator->IF) {
    //     case ForwardEuler:

    //         // y(n) = x(n)
    //         integrator->y = integrator->x;

    //         // x(n+1) = x(n) + K * Ts * u(n)
    //         integrator->x = integrator->x + integrator->K * integrator->Ts * u;

    //         // Saturation
    //         integrator->x = CLIPIF(integrator->x, integrator->limit_output, integrator->upper_limit, integrator->lower_limit);
    //         break;

    //     case BackwardEuler:

    //         // // x(n) = y(n-1)
    //         // integrator->x = integrator->y;

    //         // // y(n) = y(n-1) + K * T * u(n)
    //         // integrator->y = integrator->x + integrator->K * integrator->Ts * u;

    //         // // Saturation
    //         // integrator->y = CLIPIF(integrator->y, integrator->limit_output, integrator->upper_limit, integrator->lower_limit);



    //         // y(n) = x(n) + K * T * u(n)
    //         integrator->y = integrator->x + integrator->K * integrator->Ts * u;

    //         // Saturation
    //         integrator->y = CLIPIF(integrator->y, integrator->limit_output, integrator->upper_limit, integrator->lower_limit);

    //         // x(n+1) = y(n)
    //         integrator->x = integrator->y;

    //         break;

    //     case Trapezoidal: 

    //         // // x(n) = y(n-1) + K * T/2 * u(n-1)
    //         // integrator->x = integrator->y + integrator->K * integrator->Ts * integrator->u / 2.0f;
    //         // integrator->x = CLIPIF(integrator->x, integrator->limit_output, integrator->upper_limit, integrator->lower_limit);

    //         // // y(n) = x(n) + K * T/2 * u(n)
    //         // integrator->y = integrator->x + integrator->K * integrator->Ts * u / 2.0f;
    //         // integrator->y = CLIPIF(integrator->y, integrator->limit_output, integrator->upper_limit, integrator->lower_limit);

    //         // y(n) = x(n) + K * T/2 * u(n)
    //         integrator->y = integrator->x + integrator->K * integrator->Ts * u / 2.0f;
    //         integrator->y = CLIPIF(integrator->y, integrator->limit_output, integrator->upper_limit, integrator->lower_limit);

    //         // x(n+1) = x(n) + K * T * u(n)
    //         integrator->x = integrator->x + integrator->K * integrator->Ts * u;
    //         integrator->x = CLIPIF(integrator->x, integrator->limit_output, integrator->upper_limit, integrator->lower_limit);

    //         integrator->u = u;
    //         break;

    //     default:
    //         integrator->x = 0;
    //         integrator->y = 0;
    //         integrator->u = 0;
    // }


    // return integrator->y;
}
