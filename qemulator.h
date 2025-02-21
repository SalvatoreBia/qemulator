#ifndef QEMULATOR_H
#define QEMULATOR_H

#include <stdlib.h>
#include <complex.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "gates.h"

typedef struct
{
    int len;
    double complex* bits;
    int index;                // l'indice dove ci sta l'1
} qureg;

qureg* init_reg_null(int n)
{
    if ( n <= 0 ) 
        return NULL;

    qureg* reg = (qureg*) malloc(sizeof(qureg));
    if ( reg == NULL ) 
        return NULL;

    int _len = 1 << n;
    reg->bits = (double complex*) calloc(_len, sizeof(double complex));
    if ( reg->bits == NULL )
    {
        free(reg);
        return NULL;
    }

    reg->bits[0] = 1.0 + 0.0*I;
    reg->len = _len;
    reg->index = 0;    

    return reg;
}

qureg* init_reg_custom_bits(int n, const char* bitstring)
{
    if ( n <= 0 || bitstring == NULL ) return NULL;

    int slen = strlen(bitstring);
    if ( n != slen ) return NULL;

    for ( int i = 0; i < slen; i++ )
        if ( bitstring[i] != '0' && bitstring[i] != '1' )
            return NULL;

    int _index = (int) strtol(bitstring, NULL, 2);

    qureg* reg = (qureg*) malloc(sizeof(qureg));
    if ( reg == NULL ) return NULL;

    int _len = 1 << n;
    reg->bits = (double complex*) calloc(_len, sizeof(double complex));
    if ( reg->bits == NULL )
    {
        free(reg);
        return NULL;
    }

    reg->bits[_index] = 1.0 + 0.0*I;
    reg->len = _len;
    reg->index = _index;

    return reg;
}

void free_reg(qureg* reg)
{
    if ( reg != NULL )
    {
        free(reg->bits);
        free(reg);
    }
}

double complex* tensor(const double complex* left, const double complex* right,
                       int width_left, int width_right, int height_left, int height_right)
{
    if ( left == NULL || right == NULL ) return NULL;

    int width = width_left * width_right;
    int size = width * (height_left * height_right);
    double complex* new_gate = (double complex*) calloc(size, sizeof(double complex));
    if ( new_gate == NULL ) return NULL;
    
    for ( int i = 0; i < height_left; i++ )
        for ( int j = 0; j < width_left; j++ )
            for ( int k = 0; k < height_right; k++ )
                for ( int p = 0; p < width_right; p++ )
                    new_gate[(i * height_right + k) * width + (j * width_right + p)] = left[i * width_left + j] * right[k * width_right + p];

    return new_gate;    
}

double complex* concat(const double complex* left, const double complex* right, 
                       int width_left, int width_right, int height_left, int height_right)
{
    if ( left == NULL || right == NULL || width_left != height_right ) return NULL;

    int height = height_left;
    int width = width_right;
    int size = height * width;

    double complex* new_gate = (double complex*) calloc(size, sizeof(double complex));
    if ( new_gate == NULL ) return NULL;

    for ( int i = 0; i < height_left; i++ )
        for ( int j = 0; j < width_right; j++ ) 
        {
            new_gate[i * width + j] = 0.0 + 0.0*I;
            for ( int k = 0; k < width_left; k++ )
                new_gate[i * width + j] += left[i * width_left + k] * right[k * width_right + j];
        }
    

    return new_gate;
}

void normalize_state(qureg* reg) 
{
    if ( reg == NULL ) 
        return;
    
    double norm = 0.0;
    for ( int i = 0; i < reg->len; i++ )
        norm += creal(reg->bits[i]) * creal(reg->bits[i]) + 
                cimag(reg->bits[i]) * cimag(reg->bits[i]);
    
    norm = sqrt(norm);
    if ( norm > 0.0 )
        for ( int i = 0; i < reg->len; i++ )
            reg->bits[i] /= norm;
}

int measure(qureg* reg, int apply_normalization)
{
    if (reg == NULL) return -1;
    
    if ( apply_normalization ) normalize_state(reg);

    double* probs = (double*) malloc(sizeof(double) * reg->len);
    if (probs == NULL) return -1;

    double sum = 0.0;
    for ( int i = 0; i < reg->len; i++ ) 
    {
        probs[i] = creal(reg->bits[i]) * creal(reg->bits[i]) +
                   cimag(reg->bits[i]) * cimag(reg->bits[i]);
        sum += probs[i];
    }

    if ( sum == 0.0 ) 
    {
        free(probs);
        return -1;
    }

    for ( int i = 0; i < reg->len; i++ )
        probs[i] /= sum;

    for ( int i = 0; i < reg->len; i++ ) 
        if (probs[i] >= 0.99) 
        {
            memset(reg->bits, 0, sizeof(double complex) * reg->len);
            reg->bits[i] = 1.0 + 0.0*I;
            reg->index = i;
            free(probs);
            return i;
        }

    double r = (double) rand() / RAND_MAX;
    double cumulative = 0.0;

    for ( int i = 0; i < reg->len; i++ ) 
    {
        cumulative += probs[i];
        if ( r <= cumulative ) 
        {
            memset(reg->bits, 0, sizeof(double complex) * reg->len);
            reg->bits[i] = 1.0 + 0.0*I;
            reg->index = i;

            free(probs);
            return i;
        }
    }

    free(probs);
    return -1;   // non dovrebbe mai accadere lol
}

void apply_gate(qureg* reg, const double complex* gate, int n, int apply_normalization) 
{
    if ( reg == NULL || gate == NULL ) return;

    int _len = 1 << n;
    double complex* new_state = (double complex*) calloc(_len, sizeof(double complex));

    for ( int i = 0; i < _len; i++ )
        for ( int j = 0; j < _len; j++ )
            new_state[i] += gate[i * _len + j] * reg->bits[j];

    for ( int i = 0; i < _len; i++ )
        reg->bits[i] = new_state[i];

    free(new_state);
    
    if ( apply_normalization ) (reg);
}

#endif // QEMULATOR_H
