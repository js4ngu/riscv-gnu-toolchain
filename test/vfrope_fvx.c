#include <stdio.h>
#define VECTOR_SIZE 4

void main() {
    // Define two vectors of 32-bit floating point numbers
    float Token[VECTOR_SIZE] = {1.0, 2.0, 3.0, 4.0};
    float newToken[VECTOR_SIZE] = {0}; // Initialize the newToken array
    float theta = 0.25;
    int baseIndex = 0;
    int m_pos = 10;

    asm volatile (
        "fmv.s f30, %5\n\t"  // Changed fa30 to f30
        "mv x31, %4\n\t"     // Set M using the variable
        "vsetvli t0, %2, e32, m1\n"
        "vle32.v v0, (%1)\n"
        "vfrope.fvx v2, v0, %0\n"
        "vse32.v v2, (%3)\n"
        : 
        : "r" (baseIndex), "r" (Token), "r" (VECTOR_SIZE), "r" (newToken), "r" (m_pos), "f" (theta)
        : "t0", "v0", "v1", "v2", "x31", "f30"
    );

    // Print the result
    for (int i = 0; i < VECTOR_SIZE; i++) {
        printf("result[%d] = %f\n", i, newToken[i]);
    }
}
