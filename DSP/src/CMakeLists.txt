
# define Library "DSP"
target_sources(DSPc PRIVATE 
    Polynomial.c
    Matrix.c
    Vector.c
    Signal.c
    zTransferFunction.c
    zStateSpace.c
    zStateObserver.c
    Discontinuous.c
    Integrator.c
    Derivative.c
    pidController.c
)
