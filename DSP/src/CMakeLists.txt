
# define Library "DSP"
target_sources(DSP PRIVATE 
    Polynomial.c 
    Matrix.c
    Vector.c
    Signal.c
    zTransferFunction.c
    zStateSpace.c
    zStateObserver.c
    pidController.c
    Discontinuous.c
)
