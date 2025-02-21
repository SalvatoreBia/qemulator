#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <complex.h>
#include "qemulator.h"

#define N 2

// Oracle matrices
const double complex ORACLE_CONST0[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

const double complex ORACLE_CONST1[16] = {
    0, 1, 0, 0,
    1, 0, 0, 0,
    0, 0, 0, 1,
    0, 0, 1, 0
};

const double complex ORACLE_BAL_X[16] = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 0, 1,
    0, 0, 1, 0
};

const double complex ORACLE_BAL_NOTX[16] = {
    0, 1, 0, 0,
    1, 0, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1
};

void print_reg_state(qureg* reg) 
{
    if ( !reg ) 
        return;

    printf("\n[DEBUG] Quantum Register State (basis amplitudes):\n");
    for ( int i = 0; i < reg->len; i++ )
    {
        double re = creal(reg->bits[i]);
        double im = cimag(reg->bits[i]);
        printf("  |%d⟩: (%.4f + %.4fi)\n", i, re, im);
    }
}

int deutsch_alg(qureg* reg, const double complex* oracle)
{
    double complex* HxH = tensor(H, H, 2, 2, 2, 2);
    double complex* HxI = tensor(H, In, 2, 2, 2, 2);

    if ( !HxH || !HxI ) 
    {
        if ( HxH ) 
            free(HxH);
        if ( HxI ) 
            free(HxI);
        return -1;
    }

    apply_gate(reg, HxH, N, 0);
    free(HxH);

    apply_gate(reg, oracle, N, 0);
    
    apply_gate(reg, HxI, N, 0);
    free(HxI);

    int outcome = measure(reg, 0);

    // print_reg_state(reg);
    return outcome / 2;
}

int main()
{
    srand(time(NULL));

    qureg* reg = init_reg_custom_bits(N, "01");
    if ( !reg )
        return 1;

    int choice = rand() % 4;
    int first_qubit = -1;

    printf("[INFO] Running Deutsch's Algorithm...\n");
    switch ( choice ) 
    {
        case 0:
            printf("[INFO] Using f(x)=0 (constant)\n");
            first_qubit = deutsch_alg(reg, ORACLE_CONST0);
            break;
        case 1:
            printf("[INFO] Using f(x)=1 (constant)\n");
            first_qubit = deutsch_alg(reg, ORACLE_CONST1);
            break;
        case 2:
            printf("[INFO] Using f(x)=x (balanced)\n");
            first_qubit = deutsch_alg(reg, ORACLE_BAL_X);
            break;
        case 3:
            printf("[INFO] Using f(x)=¬x (balanced)\n");
            first_qubit = deutsch_alg(reg, ORACLE_BAL_NOTX);
            break;
    }

    if ( first_qubit == 0 )
        printf("\n[INFO] First qubit is |0⟩ -> f(x) is CONSTANT.\n");
    else if ( first_qubit == 1 )
        printf("\n[INFO] First qubit is |1⟩ -> f(x) is BALANCED.\n");
    else
        printf("\n[ERROR] Measurement failed.\n");

    free_reg(reg);
    return 0;
}
